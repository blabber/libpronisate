#include <assert.h>
#include <wand/MagickWand.h>

#include "pronisate.h"
#include "_pronisate.h"

static const char *errors[5] = {
	"",				/* PRON_SUCCESS */
	"could not allocate memory",	/* PRON_MALLOC_ERROR */
	"could not read image",		/* PRON_READ_IMAGE_ERROR */
	"MagickWand internal error",	/* PRON_WAND_ERROR */
	"could not set pixel color",	/* PRON_PIXELSETCOLOR_ERROR */
};

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

const char *
pron_get_error(PRON_ERROR error)
{
	return (errors[error]);
}

