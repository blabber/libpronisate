#include <assert.h>
#include <wand/MagickWand.h>

#include "pronisate.h"
#include "_pronisate.h"

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
