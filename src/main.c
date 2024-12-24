/* RandomArt
 * Entry point for the terminal application
 */

#include "lang.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "ast.h"
#include "mempool.h"
#include "node.h"

#define IMAGE_SIZE 512

static char *_loadFile(const char *FILEPATH) {
	FILE *file = fopen(FILEPATH, "rb");
	if( file == NULL ) {
		return NULL;
	}

	/* Get file size */
	fseek(file, 0L, SEEK_END);
	const long FILE_SIZE = ftell(file);
	rewind(file);

	/* Prepare buffer */
	char *buffer = malloc(FILE_SIZE + 1);
	if( buffer == NULL ) {
		fprintf(stderr, "malloc(%ld) failed\n", FILE_SIZE + 1);
		exit(EXIT_FAILURE);
	}

	/* Read file contents into buffer */
	const long BYTES_READ = fread(buffer, 1, FILE_SIZE, file);
	if( BYTES_READ < FILE_SIZE ) {
		fprintf(stderr, "couldn't read file into buffer\n");
		exit(EXIT_FAILURE);
	}

	buffer[FILE_SIZE] = '\0';

	/* Close file; we're done here */
	fclose(file);

	return buffer;
}

static void _usage(void) {
	printf("usage: randomart [OPTIONS]\n");
	printf("       -c, --chance..... Specifies the node chances\n");
	printf("       -f, --frames..... Specifies frames in GIF (default: 8)\n");
	printf("       -g, --gif........ Generate a gif instead of an image\n");
	printf("       -h, --help....... Display this help text\n");
	printf("       -o, --output..... Output filename (default: 'image')\n");
	printf("       -q, --quiet...... Don't print the generator function\n");
	printf("       -r, --recdepth... Sets the maximum recursion depth\n");
	printf("                         Note: this will probably segfault if\n");
	printf("                         set too high (default: 6)\n");
	printf("       -R, --run........ Runs the given file\n");
	printf("       -s, --seed....... Seed to use (default: random)\n");
	printf("       -S, --size....... Size of the image (default: 512)\n");
	printf("       -v, --verbose.... Output some more information\n");
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

	if( cmd[1] == argS && cmd[2] == '\0' ) {
		return true;
	}

	if( cmd[1] == '-' && cmd[2] != '\0' ) {
		return strcmp(cmd + 2, argL) == 0;
	}

	return false;
}

#define CHECK(S, L) if( _isOpt(argc, argv, (S), (L)) )
#define CHANCE(C, A) if( **argv == (C) || strcmp(*argv, (A)) == 0 )
#define EXPECT(B)                                                              \
	do {                                                                       \
		if( argc == 1 ) {                                                      \
			fprintf(stderr, "expected " B "\n");                               \
			exit(EXIT_FAILURE);                                                \
		}                                                                      \
		NEXT();                                                                \
	} while( false )
#define LIMIT(m, M, B, S)                                                      \
	do {                                                                       \
		if( (B) <= (m) ) {                                                     \
			printf("invalid " S " : must be larger than %d!\n", (m));          \
			exit(EXIT_FAILURE);                                                \
		}                                                                      \
		if( (B) >= (M) ) {                                                     \
			printf("invalid " S " : must be smaller than %d!\n", (M));         \
			exit(EXIT_FAILURE);                                                \
		}                                                                      \
	} while( false )

int main(int argc, char *argv[]) {
	char *file = "image";
	char *script = NULL;

	int size = IMAGE_SIZE;
	int frames = 8;

	unsigned long seed = time(NULL);
	unsigned long maxrec = 6;

	unsigned _valueChance = 25, _arithChance = 75, _trigChance = 0,
			 _expChance = 0, _commonChance = 0, _condChance = 0;

	bool gif = false;
	bool quiet = false;
	bool run = false;
	bool verbose = false;

	NEXT();
	while( argc > 0 ) {
		CHECK('c', "chance") {
			EXPECT("chance preset");

			CHANCE('n', "normal") {
			}
			else CHANCE('c', "complex") {
				_valueChance = 10;
				_arithChance = 20;
				_trigChance = 25;
				_expChance = 25;
				_commonChance = 20;
				_condChance = 0;
			}
		}
		else CHECK('f', "frames") {
			EXPECT("frame count");
			frames = strtoul(*argv, NULL, 10);
			LIMIT(1, 128, frames, "frame count");
			gif = true;
		}
		else CHECK('g', "gif") {
			gif = true;
		}
		else CHECK('h', "help") {
			printf("RandomArt generator\n\n");
			_usage();
			exit(EXIT_SUCCESS);
		}
		else CHECK('o', "output") {
			EXPECT("filename");
			file = *argv;
		}
		else CHECK('q', "quiet") {
			quiet = true;
		}
		else CHECK('r', "recdepth") {
			EXPECT("recursion depth");
			maxrec = strtoul(*argv, NULL, 10);
			LIMIT(0, 128, maxrec, "recursion depth");
		}
		else CHECK('R', "run") {
			EXPECT("script path");

			script = *argv;
			run = true;
		}
		else CHECK('s', "seed") {
			EXPECT("seed");
			seed = strtoul(*argv, NULL, 10);
		}
		else CHECK('S', "size") {
			EXPECT("size");
			size = strtoul(*argv, NULL, 10);
			LIMIT(1, 16384, size, "size");
		}
		else CHECK('v', "verbose") {
			verbose = true;
		}
		else {
			printf("unknown option '%s'\n\n", *argv);
			_usage();
			exit(EXIT_FAILURE);
		}

		NEXT();
	}

	Node *ast;
	MemPool pool = poolNew();

	if( run ) {
		const char *SCRIPT = _loadFile(script);
		if( !SCRIPT ) {
			fprintf(stderr, "error loading script at '%s'\n", script);
		}

		ast = langCompile(SCRIPT);
	} else {
		if( verbose ) {
			printf("parameters:\n");
			printf("- seed.............. %lu\n", seed);
			printf("- recursion depth... %lu\n", maxrec);
			printf("- outputting to..... %s\n", file);
			printf("- image size........ %dx%d px\n\n", size, size);
		}

		srand(seed);
		nodeSetup(maxrec, _valueChance, _arithChance, _trigChance, _expChance,
			_commonChance, _condChance);

		ast = astCreate(&pool);
	}

	if( gif ) {
		printf("1. generating GIF...\n");

		astInjectT(ast);
		if( !quiet ) {
			printf("f(x, y) = ");
			astPrint(ast);
		}

		double t;
		char filepath[256];
		for( int i = 0; i < frames; ++i ) {
			t = 2 * (((double)i) / frames) - 1;

			printf("2. running AST [%03d/%03d]...", i + 1, frames);
			fflush(stdout);
			byte *image = astDoFrame(ast, size, size, t);

			snprintf(filepath, 256, "%s-%02d.png", file, i);
			stbi_write_png(filepath, size, size, 3, image, size * 3);

			free(image);
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
		}

		printf("\n");
	} else {
		printf("1. generating image...\n");
		if( !quiet ) {
			printf("f(x, y) = ");
			astPrint(ast);
		}

		printf("\n2. running AST...\n");
		byte *image = astDoFrame(ast, size, size, 1);

		printf("\n3. writing image...\n");
		char filepath[256];
		snprintf(filepath, 256, "%s.png", file);

		stbi_write_png(filepath, size, size, 3, image, size * 3);
		free(image);
	}

	poolFree(&pool);
	printf("done!\n");

	return EXIT_SUCCESS;
}
