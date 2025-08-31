/* RandomArt
 * Entry point for the terminal application
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "ast.h"
#include "lang.h"
#include "mempool.h"
#include "node.h"

#define IMAGE_SIZE 512

static int _randRange(int minValue, int maxValue) {
	return rand() % (maxValue + 1 - minValue) + minValue;
}

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
	printf("                         possible values:\n");
	printf("                         b/branch.... your CPU will hate you\n");
	printf("                         c/complex... more complex maths\n");
	printf("                         C/chaos..... totally chaotic images\n");
	printf("                         f/fair...... equal chances\n");
	printf("                         r/random.... random chances\n");
	printf("                         s/simple.... simple maths\n");
	printf("                         t/trig...... for trigonometry lovers\n");
	printf("                         n/normal.... normal chances (default)\n");
	printf("       -f, --frames..... Specifies frames in GIF (default: 8)\n");
	printf("       -g, --gif........ Generate a gif instead of an image\n");
	printf("       -h, --help....... Display this help text\n");
	printf("       -i, --input...... Takes a script file as input\n");
	printf("       -o, --output..... Output filename (default: 'image')\n");
	printf("       -q, --quiet...... Don't print the generator function\n");
	printf("       -r, --recdepth... Sets the maximum recursion depth\n");
	printf("                         Note: this will probably segfault if\n");
	printf("                         set too high (default: 6)\n");
	printf("       -R, --run........ Runs a script passed as an argument\n");
	printf("       -s, --seed....... Seed to use (default: random)\n");
	printf("       -S, --size....... Size of the image (default: 512)\n");
	printf("       -v, --verbose.... Output some more information\n");
}

#define NEXT()                                                                 \
	--argc;                                                                    \
	++argv

static bool _isOpt(char *argv[], char argS, char *argL) {
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

#define CHECK(S, L) if( _isOpt(argv, (S), (L)) )
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

	uint64_t seed = 0;
	unsigned long maxrec = 6;

	unsigned _valueChance = 25, _arithChance = 75, _trigChance = 0,
			 _expChance = 0, _commonChance = 0, _condChance = 0;

	bool gif = false;
	bool quiet = false;
	bool run = false;
	bool verbose = false;
	bool random_chances = false;

	NEXT();
	while( argc > 0 ) {
		CHECK('c', "chance") {
			EXPECT("chance preset");

			CHANCE('b', "branch") {
				_valueChance = 10;
				_arithChance = 25;
				_trigChance = 5;
				_expChance = 5;
				_commonChance = 5;
				_condChance = 40;
			}
			else CHANCE('c', "complex") {
				_valueChance = 10;
				_arithChance = 20;
				_trigChance = 20;
				_expChance = 20;
				_commonChance = 20;
				_condChance = 0;
			}
			else CHANCE('C', "chaos") {
				_valueChance = 20;
				_arithChance = 5;
				_trigChance = 30;
				_expChance = 30;
				_commonChance = 5;
				_condChance = 10;
			}
			else CHANCE('f', "fair") {
				_valueChance = 20;
				_arithChance = 16;
				_trigChance = 16;
				_expChance = 16;
				_commonChance = 16;
				_condChance = 16;
			}
			else CHANCE('r', "random") {
				random_chances = true;
			}
			else CHANCE('s', "simple") {
				_valueChance = 20;
				_arithChance = 60;
				_trigChance = 5;
				_expChance = 5;
				_commonChance = 5;
				_condChance = 5;
			}
			else CHANCE('t', "trig") {
				_valueChance = 10;
				_arithChance = 10;
				_trigChance = 70;
				_expChance = 5;
				_commonChance = 3;
				_condChance = 1;
			}
			else CHANCE('n', "normal") {
			}
			else {
				printf("unknown chance preset '%s'\n\n", *argv);
				_usage();
				exit(EXIT_FAILURE);
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
		else CHECK('i', "input") {
			EXPECT("script");
			script = *argv;
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

			const uint64_t FNV_OFFSET_BASIS = 0x00000100000001b3U;
			const uint64_t FNV_PRIME = 0xcbf29ce484222325U;

			const char *SEED = *argv;
			seed = FNV_OFFSET_BASIS;
			for( size_t i = 0; i < strlen(SEED); ++i ) {
				seed = seed ^ SEED[i];
				seed = seed * FNV_PRIME;
			}
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

	if( seed == 0 ) {
		seed = time(NULL);
	}

	srand(seed);

	if( random_chances ) {
		int qt = 99, r = 0;

		r = qt - _randRange(0, qt);
		_valueChance = 1 + r;
		qt -= r;

		r = qt - _randRange(0, qt);
		_arithChance = r;
		qt -= r;

		r = qt - _randRange(0, qt);
		_trigChance = r;
		qt -= r;

		r = qt - _randRange(0, qt);
		_expChance = r;
		qt -= r;

		r = qt - _randRange(0, qt);
		_commonChance = r;
		qt -= r;

		_condChance = qt;
	}

	Node *ast;
	MemPool pool = poolNew();

	if( script ) {
		if( !run ) {
			script = _loadFile(script);
			if( !script ) {
				fprintf(stderr, "error loading script at '%s'\n", script);
			}
		}

		ast = langCompile(script);
	} else {
		if( verbose ) {
			printf("chances:\n");
			printf("- value..........%d%%\n", _valueChance);
			printf("- arithmetic.....%d%%\n", _arithChance);
			printf("- trigonometry...%d%%\n", _trigChance);
			printf("- exponent.......%d%%\n", _expChance);
			printf("- shader.........%d%%\n", _commonChance);
			printf("- conditional....%d%%\n", _condChance);
			printf("\nparameters:\n");
			printf("- seed.............. %" PRIu64 "\n", seed);
			printf("- recursion depth... %lu\n", maxrec);
			printf("- outputting to..... %s\n", file);
			printf("- image size........ %dx%d px\n\n", size, size);
		}

		nodeSetup(maxrec, _valueChance, _arithChance, _trigChance, _expChance,
			_commonChance, _condChance);

		ast = astCreate(&pool);
	}

	if( ast == NULL ) {
		fprintf(stderr, "error creating AST\n");

		poolFree(&pool);
		return EXIT_FAILURE;
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
