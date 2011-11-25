#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pronisate.h>

#define IMAGE	"OWNAGE.gif"
#define WIDTH	100
#define HEIGHT	50
#define FRAMES	298

int
main(int argc, char **argv)
{
	struct pron_context *ctx;
	unsigned char *p;
	size_t x, y;
	ssize_t frame;

	ctx = pron_context_open(WIDTH, HEIGHT);
	if (ctx == NULL) {
		fprintf(stderr, "pron_context_open failed\n");
		exit(EXIT_FAILURE);
	}

	for (frame = 0; frame <= FRAMES; frame++){
		if (pron_pronisate(ctx, IMAGE, frame) != 0) {
			fprintf(stderr, "pron_pronisate failed\n");
			exit(EXIT_FAILURE);
		}

		p = pron_get_stream(ctx);
		for (y = 0; y < pron_get_height(ctx); y++) {
			for (x = 0; x < pron_get_width(ctx); x++) {
				if (x == 0)
					printf("\n");

				if (*p < 64)
					printf(" ");
				else if (*p < 128)
					printf(".");
				else if (*p < 192)
					printf(":");
				else
					printf("8");

				p++;
			}
		}

		usleep(1);
	}

	pron_context_close(ctx);

	return (0);
}
