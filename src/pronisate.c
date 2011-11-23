#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wand/MagickWand.h>

#include "pronisate.h"

/* stolen from the MagickWand documentation */
#define ThrowWandException(wand) do {                             \
	char *description;                                        \
	ExceptionType severity;                                   \
	description = MagickGetException(wand, &severity);        \
	fprintf(stderr, "%s %s %lu %s\n",                         \
		GetMagickModule(), description);                  \
	description=(char *) MagickRelinquishMemory(description); \
	return (NULL);                                            \
} while (0)

unsigned char *
pronisate(char *filename, size_t width, size_t height)
{
	PixelIterator *iterator;
	MagickWand *image_wand;
	MagickBooleanType status;
	unsigned char *stream, *p;
	size_t y;

	/* init MagickWand */
	MagickWandGenesis();

	/* read image. */
	image_wand = NewMagickWand();
	status = MagickReadImage(image_wand, filename);
	if (status == MagickFalse)
		ThrowWandException(image_wand);

	/* scale image */
	status = MagickScaleImage(image_wand, width, height);
	if (status == MagickFalse)
		ThrowWandException(image_wand);

	if ((stream = malloc(width*height)) == NULL) {
		fprintf(stderr, "libpronisate: could not allocate out buffer (%d bytes)\n", width*height);
		return (NULL);
	}

	/* iterate image and fill stream */
	iterator = NewPixelIterator(image_wand);
	if ((iterator == (PixelIterator *) NULL))
		ThrowWandException(image_wand);

	p = stream;
	for (y = 0; y < height; y++) {
		PixelWand **pixels;
		size_t x;

		pixels = PixelGetNextIteratorRow(iterator, &width);
		if ((pixels == (PixelWand **) NULL))
			break;
		for (x = 0; x < width; x++)
		{
			double hue, saturation, lightness;

			PixelGetHSL(pixels[x], &hue, &saturation, &lightness);
			*(p++) = (char)(lightness*255);
		}
	}
	if (y < MagickGetImageHeight(image_wand))
		ThrowWandException(image_wand);

	/* clean up */
	iterator=DestroyPixelIterator(iterator);
	image_wand=DestroyMagickWand(image_wand);

	/* deinit MagickWand */
	MagickWandTerminus();

	return (stream);
}
