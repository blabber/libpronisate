#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pronisate.h>

#define IMAGE	"OWNAGE.gif"
#define WIDTH	100
#define HEIGHT	50

int
main(int argc, char **argv)
{
	struct pron_context *ctx;
	unsigned char *p;
	size_t x, y;
	ssize_t frame;

	ctx = pron_context_open(IMAGE, WIDTH, HEIGHT);
	if (ctx == NULL) {
		fprintf(stderr, "pron_context_open failed\n");
		exit(EXIT_FAILURE);
	}

	for (frame = 0; frame <= pron_get_frame_count(ctx); frame++){
		if (pron_pronisate(ctx, frame) != 0) {
			fprintf(stderr, "pron_pronisate failed\n");
			exit(EXIT_FAILURE);
		}

		p = pron_get_stream(ctx);
		for (y = 0; y < pron_get_height(ctx); y++) {
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

	return (0);
}
