#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <wand/MagickWand.h>

#include "pronisate.h"

struct pron_context {
	size_t		 width;
	size_t		 height;
	ssize_t		 frame_count;
	unsigned char	*stream;
	MagickWand	*wand;
};

static int handle_transparency(MagickWand **_image_wand);
static int fill_stream(struct pron_context *_ctx, MagickWand *_image_wand);

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
pron_context_open(char *filename, size_t width, size_t height)
{
	struct pron_context	*ctx;
	MagickBooleanType	 status;

	ctx = malloc(sizeof(struct pron_context));
	if (ctx == NULL) {
		fprintf(stderr, "malloc");
		return (NULL);
	}
	ctx->stream = malloc(width*height);
	if (ctx->stream == NULL) {
		fprintf(stderr, "malloc");
		free(ctx);
		return (NULL);
	}

	ctx->wand = NewMagickWand();
	status = MagickReadImage(ctx->wand, filename);
	if (status == MagickFalse) {
		ThrowWandException(ctx->wand);
		free(ctx->stream);
		free(ctx);
		return (NULL);
	}

	ctx->width = width;
	ctx->height = height;

	MagickSetLastIterator(ctx->wand);
	ctx->frame_count = MagickGetIteratorIndex(ctx->wand);
	MagickSetFirstIterator(ctx->wand);

	return (ctx);
}

void
pron_context_close(struct pron_context *ctx)
{
	DestroyMagickWand(ctx->wand);
	free(ctx->stream);
	free(ctx);
}

unsigned char *
pron_get_stream(struct pron_context *ctx)
{
	assert(ctx != NULL);
	return (ctx->stream);
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
	MagickWand		*image_wand;
	MagickBooleanType	 mstatus;
	int			 istatus;

	image_wand = CloneMagickWand(ctx->wand);

	/* get frame */
	mstatus = MagickSetIteratorIndex(image_wand, frame);
	if (mstatus == MagickFalse) {
		ThrowWandException(image_wand);
		return (-1);
	}

	/* scale image */
	mstatus = MagickScaleImage(image_wand, ctx->width, ctx->height);
	if (mstatus == MagickFalse) {
		ThrowWandException(image_wand);
		return (-1);
	}

	/* handle transparency */
	istatus = handle_transparency(&image_wand);
	if (istatus != 0) {
		fprintf(stderr, "handle_transparency");
		return (-1);
	}

	/* iterate image and fill stream */
	istatus = fill_stream(ctx, image_wand);
	if (istatus != 0) {
		fprintf(stderr, "fill_stream");
		return (-1);
	}

	/* clean up */
	DestroyMagickWand(image_wand);

	return (0);
}

static int
fill_stream(struct pron_context *ctx, MagickWand *image_wand)
{
	unsigned char		*p;
	PixelIterator		*iterator;
	size_t			 y;

	p = ctx->stream;

	iterator = NewPixelIterator(image_wand);
	if ((iterator == NULL)) {
		ThrowWandException(image_wand);
		return (-1);
	}

	for (y = 0; y < ctx->height; y++) {
		PixelWand	**pixels;
		size_t		  x, width;

		pixels = PixelGetNextIteratorRow(iterator, &width);
		if ((pixels == NULL))
			break;
		for (x = 0; x < width; x++) {
			double hue, saturation, lightness;

			PixelGetHSL(pixels[x], &hue, &saturation, &lightness);
			*(p++) = (char)(lightness*255);
		}
	}
	if (y < ctx->height) {
		ThrowWandException(image_wand);
		return (-1);
	}

	DestroyPixelIterator(iterator);

	return (0);
}

static int
handle_transparency(MagickWand **image_wand)
{
	PixelWand		*background;
	MagickWand		*composite_wand;
	MagickBooleanType	 status;
	size_t			 width, height;

	width = MagickGetImageWidth(*image_wand);
	height = MagickGetImageHeight(*image_wand);

	background = NewPixelWand();
	status = PixelSetColor(background, "white");
	if (status == MagickFalse) {
		fprintf(stderr, "PixelSetColor");
		return (-1);
	}

	composite_wand = NewMagickWand();
	status = MagickNewImage(composite_wand, width, height, background);
	if (status == MagickFalse) {
		ThrowWandException(composite_wand);
		return (-1);
	}

	DestroyPixelWand(background);

	status = MagickCompositeImage(composite_wand, *image_wand, OverCompositeOp, 0, 0);
	if (status == MagickFalse) {
		ThrowWandException(*image_wand);
		return (-1);
	}

	DestroyMagickWand(*image_wand);

	*image_wand = composite_wand;

	return (0);
}
