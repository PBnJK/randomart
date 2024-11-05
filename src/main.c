/* RandomArt
 * Entry point
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "ast.h"
#include "mempool.h"

#define IMAGE_SIZE 512

/* Saves a given array of RGB values to a .ppm image */
static void _writePPM(const char *FILENAME, int w, int h, byte *data) {
	FILE *f = fopen(FILENAME, "wb");
	if( f == NULL ) {
		fprintf(stderr, "Couldn't open file %s: ", FILENAME);
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	/* PPM header */
	fprintf(f, "P6\n%d\n%d\n255\n", w, h);
	for( int i = 0; i < w * h * 3; ++i ) {
		fputc(data[i], f);
	}

	fclose(f);
}

int main(int argc, char *argv[]) {
	unsigned seed = time(NULL);
	printf("Using seed: %d\n", seed);
	srand(seed);

	byte *image = astGenerateArt(IMAGE_SIZE, IMAGE_SIZE);
	_writePPM("image.ppm", IMAGE_SIZE, IMAGE_SIZE, image);

	free(image);

	return EXIT_SUCCESS;
}
