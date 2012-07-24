#include <assert.h>
#include <wand/MagickWand.h>

#include "pronisate.h"
#include "_pronisate.h"

PRON_ERROR
pron_context_open(const char *filename, size_t width, size_t height, ssize_t panels_x, ssize_t panels_y, struct pron_context **ctx)
{
	struct pron_context	*lctx;
	MagickBooleanType	 status;
	int			 x = -1, y = -1;
	PRON_ERROR		 error;

	assert(filename != NULL);

	lctx = malloc(sizeof(struct pron_context));
	if (lctx == NULL) {
		error = PRON_MALLOC_ERROR;
		goto error;
	}
	lctx->streams = NULL;
	lctx->wand = NULL;

	lctx->streams = malloc(panels_x * sizeof(unsigned char **));
	if (lctx->streams == NULL) {
		error = PRON_MALLOC_ERROR;
		goto error;
	}
	for (x = 0; x < panels_x; x++) {
		lctx->streams[x] = malloc(panels_y * sizeof(unsigned char *));
		if (lctx->streams[x] == NULL) {
			error = PRON_MALLOC_ERROR;
			goto error;
		}
		for (y = 0; y < panels_y; y++) {
			lctx->streams[x][y] = malloc(width*height);
			if (lctx->streams[x][y] == NULL) {
				error = PRON_MALLOC_ERROR;
				goto error;
			}
		}
	}

	lctx->wand = NewMagickWand();
	status = MagickReadImage(lctx->wand, filename);
	if (status == MagickFalse) {
		error = PRON_READ_IMAGE_ERROR;
		goto error;
	}

	lctx->width = width;
	lctx->height = height;

	lctx->panels_x = panels_x;
	lctx->panels_y = panels_y;

	MagickSetLastIterator(lctx->wand);
	lctx->frame_count = MagickGetIteratorIndex(lctx->wand);
	MagickSetFirstIterator(lctx->wand);

	*ctx = lctx;

	return (PRON_SUCCESS);

error:
	if (lctx != NULL) {
		if (lctx->wand != NULL)
			DestroyMagickWand(lctx->wand);

		for ( ; x >= 0; x--) {
			for ( ; y >= 0; y--) {
				free(lctx->streams[x][y]);
			}
			free(lctx->streams[x]);
		}

		free(lctx->streams);
		free(lctx);
	}

	return (error);
}
