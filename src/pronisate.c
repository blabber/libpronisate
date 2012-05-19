#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <wand/MagickWand.h>

#include "pronisate.h"

struct pron_context {
	size_t		   width;
	size_t		   height;
	ssize_t		   frame_count;
	ssize_t		   panels_x;
	ssize_t		   panels_y;
	unsigned char	***streams;
	MagickWand	  *wand;
};

static int handle_transparency(MagickWand **_image_wand);
static int fill_stream(struct pron_context *_ctx, MagickWand *_image_wand, int _panel_x, int _panel_y);

/* stolen from the MagickWand documentation */
#define ThrowWandException(wand) do {                             \
	char *description;                                        \
	ExceptionType severity;                                   \
	description = MagickGetException(wand, &severity);        \
	fprintf(stderr, "%s %s %lu %s\n",                         \
		GetMagickModule(), description);                  \
	description=(char *) MagickRelinquishMemory(description); \
} while (0)

void
pron_init()
{
	MagickWandGenesis();
}

void
pron_deinit()
{
	MagickWandTerminus();
}

struct pron_context *
pron_context_open(char *filename, size_t width, size_t height, ssize_t panels_x, ssize_t panels_y)
{
	struct pron_context	*ctx = NULL;
	MagickBooleanType	 status;
	int			 x = -1, y = -1;

	ctx = malloc(sizeof(struct pron_context));
	if (ctx == NULL) {
		fprintf(stderr, "malloc\n");
		goto error;
	}
	ctx->streams = NULL;
	ctx->wand = NULL;

	/* TODO Errorhandling */
	ctx->streams = malloc(panels_x * sizeof(unsigned char **));
	if (ctx->streams == NULL) {
		fprintf(stderr, "malloc\n");
		goto error;
	}
	for (x = 0; x < panels_x; x++) {
		ctx->streams[x] = malloc(panels_y * sizeof(unsigned char *));
		if (ctx->streams[x] == NULL) {
			fprintf(stderr, "malloc\n");
			goto error;
		}
		for (y = 0; y < panels_y; y++) {
			ctx->streams[x][y] = malloc(width*height);
			if (ctx->streams[x][y] == NULL) {
				fprintf(stderr, "malloc\n");
				goto error;
			}
		}
	}

	ctx->wand = NewMagickWand();
	status = MagickReadImage(ctx->wand, filename);
	if (status == MagickFalse) {
		fprintf(stderr, "could not read image: %s\n", filename);
		goto error;
	}

	ctx->width = width;
	ctx->height = height;

	ctx->panels_x = panels_x;
	ctx->panels_y = panels_y;

	MagickSetLastIterator(ctx->wand);
	ctx->frame_count = MagickGetIteratorIndex(ctx->wand);
	MagickSetFirstIterator(ctx->wand);

	return (ctx);

error:
	if (ctx != NULL) {
		if (ctx->wand != NULL)
			DestroyMagickWand(ctx->wand);

		for ( ; x >= 0; x--) {
			for ( ; y >= 0; y--) {
				free(ctx->streams[x][y]);
			}
			free(ctx->streams[x]);
		}

		free(ctx->streams);
		free(ctx);
	}

	return (NULL);
}

void
pron_context_close(struct pron_context *ctx)
{
	int x, y;

	assert(ctx != NULL);
	assert(IsMagickWand(ctx->wand) == MagickTrue);

	DestroyMagickWand(ctx->wand);

	for ( x = 0; x < ctx->panels_x; x++) {
		for ( y = 0; y < ctx->panels_y; y++) {
			free(ctx->streams[x][y]);
		}
		free(ctx->streams[x]);
	}

	free(ctx->streams);
	free(ctx);
}

unsigned char *
pron_get_stream(struct pron_context *ctx, ssize_t px, ssize_t py)
{
	assert(ctx != NULL);
	assert(px < ctx->panels_x);
	assert(py < ctx->panels_y);

	return (ctx->streams[px][py]);
}

size_t
pron_get_width(struct pron_context *ctx)
{
	assert(ctx != NULL);
	return (ctx->width);
}

size_t
pron_get_height(struct pron_context *ctx)
{
	assert(ctx != NULL);
	return (ctx->height);
}

ssize_t
pron_get_frame_count(struct pron_context *ctx)
{
	assert(ctx != NULL);
	return (ctx->frame_count);
}

int
pron_pronisate(struct pron_context *ctx, ssize_t frame)
{
	MagickWand		*image_wand = NULL;
	MagickWand		*crop_wand = NULL;
	MagickBooleanType	 mstatus;
	int			 istatus;
	int			 px, py;

	assert(ctx != NULL);
	assert(IsMagickWand(ctx->wand) == MagickTrue);

	image_wand = CloneMagickWand(ctx->wand);

	mstatus = MagickSetIteratorIndex(image_wand, frame);
	if (mstatus == MagickFalse) {
		ThrowWandException(image_wand);
		goto error;
	}

	mstatus = MagickScaleImage(image_wand, ctx->width * ctx->panels_x, ctx->height * ctx->panels_y);
	if (mstatus == MagickFalse) {
		ThrowWandException(ctx->wand);
		goto error;
	}

	istatus = handle_transparency(&image_wand);
	if (istatus != 0) {
		fprintf(stderr, "handle_transparency");
		goto error;
	}

	for (px = 0; px < ctx->panels_x; px++) {
		for (py = 0; py < ctx->panels_y; py++) {
			crop_wand = CloneMagickWand(image_wand);

			mstatus = MagickCropImage(crop_wand, 
			    ctx->width, ctx->height, ctx->width * px, ctx->height * py);
			if (mstatus == MagickFalse) {
				ThrowWandException(crop_wand);
				goto error;
			}

			istatus = fill_stream(ctx, crop_wand, px, py);
			if (istatus != 0) {
				fprintf(stderr, "fill_stream");
				goto error;
			}

			DestroyMagickWand(crop_wand);
		}
	}

	DestroyMagickWand(image_wand);

	return (0);

error:
	if (image_wand != NULL)
		DestroyMagickWand(image_wand);
	if (crop_wand != NULL)
		DestroyMagickWand(crop_wand);

	return (-1);

}

static int
fill_stream(struct pron_context *ctx, MagickWand *image_wand, int px, int py)
{
	unsigned char		*p = NULL;
	PixelIterator		*iterator = NULL;
	size_t			 y;

	assert(ctx != NULL);
	assert(ctx->streams != NULL);
	assert(IsMagickWand(image_wand) == MagickTrue);
	assert(MagickGetImageWidth(image_wand) == ctx->width);
	assert(MagickGetImageHeight(image_wand) == ctx->height);

	p = ctx->streams[px][py];

	iterator = NewPixelIterator(image_wand);
	if (iterator == NULL) {
		ThrowWandException(image_wand);
		goto error;
	}

	for (y = 0; y < ctx->height; y++) {
		PixelWand	**pixels;
		size_t		  x, width;

		pixels = PixelGetNextIteratorRow(iterator, &width);
		if (pixels == NULL)
			break;
		for (x = 0; x < width; x++) {
			double hue, saturation, lightness;

			PixelGetHSL(pixels[x], &hue, &saturation, &lightness);
			*(p++) = (char)(lightness*255);
		}
	}
	if (y < ctx->height) {
		ThrowWandException(image_wand);
		goto error;
	}

	DestroyPixelIterator(iterator);

	return (0);

error:
	if (iterator != NULL)
		DestroyPixelIterator(iterator);

	return (-1);
}

static int
handle_transparency(MagickWand **image_wand)
{
	PixelWand		*background = NULL;
	MagickWand		*composite_wand = NULL;
	MagickBooleanType	 status;
	size_t			 width, height;

	assert(IsMagickWand(*image_wand) == MagickTrue);

	width = MagickGetImageWidth(*image_wand);
	height = MagickGetImageHeight(*image_wand);

	background = NewPixelWand();
	status = PixelSetColor(background, "white");
	if (status == MagickFalse) {
		fprintf(stderr, "PixelSetColor");
		goto error;
	}

	composite_wand = NewMagickWand();
	status = MagickNewImage(composite_wand, width, height, background);
	if (status == MagickFalse) {
		ThrowWandException(composite_wand);
		goto error;
	}

	status = MagickCompositeImage(composite_wand, *image_wand, OverCompositeOp, 0, 0);
	if (status == MagickFalse) {
		ThrowWandException(*image_wand);
		return (-1);
	}

	DestroyPixelWand(background);
	DestroyMagickWand(*image_wand);

	*image_wand = composite_wand;

	return (0);

error:
	if (background != NULL)
		DestroyPixelWand(background);
	if (composite_wand != NULL)
		DestroyMagickWand(composite_wand);

	return (-1);
}
