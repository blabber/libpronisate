#include <stdio.h>
#include <stdlib.h>

#include <pronisate.h>

#define IMAGE	"happy-smiley.gif"
#define WIDTH	70
#define HEIGHT	32

int
main(int argc, char **argv)
{
	unsigned char *stream, *p;
	size_t x, y;

	if ((stream = pronisate(IMAGE, WIDTH, HEIGHT)) == NULL) {
		fprintf(stderr, "fail");
		exit(-1);
	}

	p = stream;
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
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

	return (0);
}
