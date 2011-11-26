#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <wand/MagickWand.h>

#include "pronisate.h"

struct pron_context {
	size_t		 width;
	size_t		 height;
	unsigned char	*stream;
	MagickWand	*wand;
};

/* stolen from the MagickWand documentation */
#define ThrowWandException(wand) do {                             \
	char *description;                                        \
	ExceptionType severity;                                   \
	description = MagickGetException(wand, &severity);        \
	fprintf(stderr, "%s %s %lu %s\n",                         \
		GetMagickModule(), description);                  \
	description=(char *) MagickRelinquishMemory(description); \
} while (0)

struct pron_context *
pron_context_open(char *filename, size_t width, size_t height)
{
	struct pron_context *ctx;
	MagickBooleanType status;

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
	ctx->width = width;
	ctx->height = height;

	MagickWandGenesis();

	ctx->wand = NewMagickWand();
	status = MagickReadImage(ctx->wand, filename);
	if (status == MagickFalse) {
		ThrowWandException(ctx->wand);
		free(ctx->stream);
		free(ctx);
		return (NULL);
	}

	return (ctx);
}

void
pron_context_close(struct pron_context *ctx)
{
	DestroyMagickWand(ctx->wand);
	free(ctx->stream);
	free(ctx);

	MagickWandTerminus();
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

int
pron_pronisate(struct pron_context *ctx, ssize_t frame)
{
	PixelIterator *iterator;
	PixelWand *background;
	MagickWand *composite_wand;
	MagickBooleanType status;
	unsigned char *p;
	size_t y;

	/* get frame */
	status = MagickSetIteratorIndex(ctx->wand, frame);
	if (status == MagickFalse) {
		ThrowWandException(ctx->wand);
		return (-1);
	}

	/* scale image */
	status = MagickScaleImage(ctx->wand, ctx->width, ctx->height);
	if (status == MagickFalse) {
		ThrowWandException(ctx->wand);
		return (-1);
	}

	/* handle transparency */
	background = NewPixelWand();
	PixelSetColor(background, "white");

	composite_wand = NewMagickWand();
	status = MagickNewImage(composite_wand, ctx->width, ctx->height, background);
	if (status == MagickFalse) {
		ThrowWandException(composite_wand);
		return (-1);
	}

	status = MagickCompositeImage(composite_wand, ctx->wand, OverCompositeOp, 0, 0);
	if (status == MagickFalse) {
		ThrowWandException(composite_wand);
		return (-1);
	}

	/* iterate image and fill stream */
	p = ctx->stream;

	iterator = NewPixelIterator(composite_wand);
	if ((iterator == NULL))
		ThrowWandException(composite_wand);

	for (y = 0; y < ctx->height; y++) {
		PixelWand **pixels;
		size_t x, width;

		pixels = PixelGetNextIteratorRow(iterator, &width);
		if ((pixels == NULL))
			break;
		for (x = 0; x < width; x++) {
			double hue, saturation, lightness;

			PixelGetHSL(pixels[x], &hue, &saturation, &lightness);
			*(p++) = (char)(lightness*255);
		}
	}
	if (y < ctx->height)
		ThrowWandException(composite_wand);

	/* clean up */
	DestroyPixelIterator(iterator);
	DestroyPixelWand(background);
	DestroyMagickWand(composite_wand);

	return (0);
}
