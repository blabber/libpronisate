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
};

/* stolen from the MagickWand documentation */
#define ThrowWandException(wand) do {                             \
	char *description;                                        \
	ExceptionType severity;                                   \
	description = MagickGetException(wand, &severity);        \
	fprintf(stderr, "%s %s %lu %s\n",                         \
		GetMagickModule(), description);                  \
	description=(char *) MagickRelinquishMemory(description); \
	return (-1);                                              \
} while (0)

struct pron_context *
pron_context_open(size_t width, size_t height)
{
	struct pron_context *ctx;

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

	return (ctx);
}

void
pron_context_close(struct pron_context *ctx)
{
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

size_t
pron_pronisate(struct pron_context *ctx, char *filename)
{
	PixelIterator *iterator;
	MagickWand *image_wand;
	MagickBooleanType status;
	unsigned char *p;
	size_t y;

	/* read image. */
	image_wand = NewMagickWand();
	status = MagickReadImage(image_wand, filename);
	if (status == MagickFalse)
		ThrowWandException(image_wand);

	/* scale image */
	status = MagickScaleImage(image_wand, ctx->width, ctx->height);
	if (status == MagickFalse)
		ThrowWandException(image_wand);

	/* iterate image and fill stream */
	p = ctx->stream;

	iterator = NewPixelIterator(image_wand);
	if ((iterator == NULL))
		ThrowWandException(image_wand);

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
	if (y < MagickGetImageHeight(image_wand))
		ThrowWandException(image_wand);

	/* clean up */
	DestroyPixelIterator(iterator);
	DestroyMagickWand(image_wand);

	return (0);
}
