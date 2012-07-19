#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pronisate.h>

#define IMAGE	"OWNAGE.gif"
#define WIDTH	100
#define HEIGHT	50
#define PANEL_X	0
#define PANEL_Y	0
#define PANELS_X	1
#define PANELS_Y	1

int
main(int argc, char **argv)
{
	struct pron_context *ctx;
	ssize_t frame;
	PRON_ERROR error;

	pron_init();

	error = pron_context_open(IMAGE, WIDTH, HEIGHT, PANELS_X, PANELS_Y, &ctx);
	if (error != PRON_SUCCESS) {
		fprintf(stderr, "pron_context_open faileid: %s\n", pron_get_error(error));
		exit(EXIT_FAILURE);
	}

	for (frame = 0; frame <= pron_get_frame_count(ctx); frame++){
		unsigned char	*p;
		size_t		 y;

		error = pron_pronisate(ctx, frame);
		if (error != PRON_SUCCESS) {
			fprintf(stderr, "pron_pronisate failed: %s\n", pron_get_error(error));
			exit(EXIT_FAILURE);
		}

		p = pron_get_stream(ctx, PANEL_X, PANEL_Y);

		for (y = 0; y < pron_get_height(ctx); y++) {
			size_t x;

			for (x = 0; x < pron_get_width(ctx); x++) {
				if (x == 0)
					printf("\n");

				if (*p < 51)
					printf(" ");
				else if (*p < 102)
					printf(".");
				else if (*p < 153)
					printf(":");
				else if (*p < 204)
					printf("0");
				else
					printf("8");

				p++;
			}
		}

		usleep(125000);
	}

	pron_context_close(ctx);

	pron_deinit();

	return (0);
}
