/* RandomArt
 * Entry point
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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

static void _usage(void) {
	printf("RandomArt generator\n\n");
	printf("usage: randomart [OPTIONS]\n");
	printf("       -h, --help..... Display this help text\n");
	printf("       -o, --output... Output filename (default: 'image.ppm')\n");
	printf("       -s, --seed..... Seed to use (default: random)\n");
	printf("       -S, --size..... Size of the image (default: 512)\n");
	printf("       -q, --quiet.... Don't print the generator function\n");
}

#define NEXT()                                                                 \
	--argc;                                                                    \
	++argv

static bool _isOpt(int argc, char *argv[], char argS, char *argL) {
	char *cmd = *argv;
	if( *cmd != '-' ) {
		fprintf(stderr, "unexpected %s\n", cmd);
		exit(EXIT_FAILURE);
	}

	if( cmd[1] != argS ) {
		return false;
	}

	if( cmd[2] == '\0' ) {
		return true;
	}

	return strcmp(cmd, argL) == 0;
}

#define CHECK(S, L) if( _isOpt(argc, argv, (S), (L)) )
#define EXPECT(B)                                                              \
	do {                                                                       \
		if( argc == 1 ) {                                                      \
			fprintf(stderr, "expected " B "\n");                               \
			exit(EXIT_FAILURE);                                                \
		}                                                                      \
		NEXT();                                                                \
	} while( false )

int main(int argc, char *argv[]) {
	unsigned long seed = time(NULL);
	char *file = "image.ppm";
	int size = IMAGE_SIZE;

	bool quiet = false;

	NEXT();
	while( argc > 0 ) {
		CHECK('h', "help") {
			_usage();
			exit(EXIT_SUCCESS);
		}
		else CHECK('o', "output") {
			EXPECT("filename");
			file = *argv;
		}
		else CHECK('s', "seed") {
			EXPECT("seed");
			seed = strtoul(*argv, NULL, 10);
		}
		else CHECK('S', "size") {
			EXPECT("size");
			size = strtoul(*argv, NULL, 10);
		}
		else CHECK('q', "quiet") {
			quiet = true;
		}

		NEXT();
	}

	srand(seed);

	byte *image = astGenerateArt(size, size, quiet);
	_writePPM(file, size, size, image);

	free(image);

	return EXIT_SUCCESS;
}
