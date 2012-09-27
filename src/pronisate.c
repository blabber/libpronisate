#include <assert.h>

#include "pronisate.h"
#include "_pronisate.h"

static int handle_transparency(MagickWand **_image_wand);
static int fill_stream(struct pron_context *_ctx, MagickWand *_image_wand, int _panel_x, int _panel_y);

PRON_ERROR
pron_pronisate(struct pron_context *ctx, ssize_t frame)
{
	MagickWand		*image_wand = NULL;
	MagickWand		*crop_wand = NULL;
	MagickBooleanType	 mstatus;
	int			 istatus;
	int			 px, py;
	PRON_ERROR		 error;

	assert(ctx != NULL);
	assert(IsMagickWand(ctx->wand) == MagickTrue);

	image_wand = CloneMagickWand(ctx->wand);

	mstatus = MagickSetIteratorIndex(image_wand, frame);
	if (mstatus == MagickFalse) {
		error = PRON_WAND_ERROR;
		goto error;
	}

	mstatus = MagickScaleImage(image_wand, ctx->width * ctx->panels_x, ctx->height * ctx->panels_y);
	if (mstatus == MagickFalse) {
		error = PRON_WAND_ERROR;
		goto error;
	}

	istatus = handle_transparency(&image_wand);
	if (istatus != 0)
		goto error;

	for (px = 0; px < ctx->panels_x; px++) {
		for (py = 0; py < ctx->panels_y; py++) {
			crop_wand = CloneMagickWand(image_wand);

			mstatus = MagickCropImage(crop_wand, 
			    ctx->width, ctx->height, ctx->width * px, ctx->height * py);
			if (mstatus == MagickFalse) {
				error = PRON_WAND_ERROR;
				goto error;
			}

			error = fill_stream(ctx, crop_wand, px, py);
			if (error != PRON_SUCCESS)
				goto error;

			DestroyMagickWand(crop_wand);
		}
	}

	DestroyMagickWand(image_wand);

	return (PRON_SUCCESS);

error:
	if (image_wand != NULL)
		DestroyMagickWand(image_wand);
	if (crop_wand != NULL)
		DestroyMagickWand(crop_wand);

	return (error);

}

static int
fill_stream(struct pron_context *ctx, MagickWand *image_wand, int px, int py)
{
	unsigned char	*p = NULL;
	PixelIterator	*iterator = NULL;
	size_t		 y;
	PRON_ERROR	 error;

	assert(ctx != NULL);
	assert(ctx->streams != NULL);
	assert(IsMagickWand(image_wand) == MagickTrue);
	assert(MagickGetImageWidth(image_wand) == ctx->width);
	assert(MagickGetImageHeight(image_wand) == ctx->height);

	p = ctx->streams[px][py];

	iterator = NewPixelIterator(image_wand);
	if (iterator == NULL) {
		error = PRON_WAND_ERROR;
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
		error = PRON_WAND_ERROR;
		goto error;
	}

	DestroyPixelIterator(iterator);

	return (PRON_SUCCESS);

error:
	if (iterator != NULL)
		DestroyPixelIterator(iterator);

	return (error);
}

static int
handle_transparency(MagickWand **image_wand)
{
	PixelWand		*background = NULL;
	MagickWand		*composite_wand = NULL;
	MagickBooleanType	 status;
	size_t			 width, height;
	PRON_ERROR		 error;

	assert(IsMagickWand(*image_wand) == MagickTrue);

	width = MagickGetImageWidth(*image_wand);
	height = MagickGetImageHeight(*image_wand);

	background = NewPixelWand();
	status = PixelSetColor(background, "white");
	if (status == MagickFalse) {
		error = PRON_PIXELSETCOLOR_ERROR;
		goto error;
	}

	composite_wand = NewMagickWand();
	status = MagickNewImage(composite_wand, width, height, background);
	if (status == MagickFalse) {
		error = PRON_WAND_ERROR;
		goto error;
	}

	status = MagickCompositeImage(composite_wand, *image_wand, OverCompositeOp, 0, 0);
	if (status == MagickFalse) {
		error = PRON_WAND_ERROR;
		goto error;
	}

	DestroyPixelWand(background);
	DestroyMagickWand(*image_wand);

	*image_wand = composite_wand;

	return (PRON_SUCCESS);

error:
	if (background != NULL)
		DestroyPixelWand(background);
	if (composite_wand != NULL)
		DestroyMagickWand(composite_wand);

	return (error);
}
