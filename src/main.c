/* RandomArt
 */

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef uint8_t byte;

#define IMAGE_SIZE 512

typedef enum _NodeType {
	/* Values */
	NT_NUM, /* Number */
	NT_X, /* X coordinate */
	NT_Y, /* Y coordinate */

	/* Arithmetic */
	NT_ADD, /* A + B */
	NT_SUB, /* A - B */
	NT_MUL, /* A * B */
	NT_DIV, /* A / B */
	NT_MOD, /* A % B */

	/* Trigonometry */
	NT_SIN, /* sin A */
	NT_COS, /* cos A */

	/* Conditionals */
	NT_IF, /* A ? B : C */
	NT_LT, /* A < B */
	NT_LTEQ, /* A <= B */
	NT_GT, /* A > B */
	NT_GTEQ, /* A >= B */
	NT_EQ, /* A == B */
	NT_NEQ, /* A != B */

	/* Misc. */
	NT_RGB, /* rgb(A, B, C) */
} NodeType;

typedef struct _Node {
	NodeType type;
	double num;
	struct _Node *a, *b, *c;
} Node;

#define POOL_START_SIZE (256 * sizeof(Node))
typedef struct _MemPool {
	byte *start, *next, *end;
} MemPool;

static MemPool *_poolNew(void) {
	MemPool *pool = malloc(POOL_START_SIZE);
	pool->next = (byte *)&pool[1];
	pool->end = pool->next + POOL_START_SIZE;

	return pool;
}

static void *_poolAlloc(MemPool *pool, int size) {
	void *mem = (void *)pool->next;
	pool->next += size;
	return mem;
}

static Node *_newNode(NodeType type, MemPool *pool) {
	Node *node = _poolAlloc(pool, sizeof(*node));

	node->type = type;

	node->a = NULL;
	node->b = NULL;
	node->c = NULL;

	node->num = 0.0;

	return node;
}

#define NODE(T) _newNode((T), pool)
#define NODE_RANDOM() _createRandomNode(pool)

#define NODE_NUM(N) _createNodeNumber((N), pool)
#define NODE_X() NODE(NT_X, pool)
#define NODE_Y() NODE(NT_Y, pool)

#define NODE_ADD(A, B) _createNodeBinary(NT_ADD, (A), (B), pool)
#define NODE_SUB(A, B) _createNodeBinary(NT_SUB, (A), (B), pool)
#define NODE_MUL(A, B) _createNodeBinary(NT_MUL, (A), (B), pool)
#define NODE_DIV(A, B) _createNodeBinary(NT_DIV, (A), (B), pool)
#define NODE_MOD(A, B) _createNodeBinary(NT_MOD, (A), (B), pool)

#define NODE_SIN(A) _createNodeUnary(NT_SIN, (A), pool)
#define NODE_COS(A) _createNodeUnary(NT_COS, (A), pool)

#define NODE_IF(A, B, C) _createNodeTernary(NT_IF, (A), (B), (C), pool)
#define NODE_LT(A, B) _createNodeBinary(NT_LT, (A), (B), pool)
#define NODE_LTEQ(A, B) _createNodeBinary(NT_LTEQ, (A), (B), pool)
#define NODE_GT(A, B) _createNodeBinary(NT_GT, (A), (B), pool)
#define NODE_GTEQ(A, B) _createNodeBinary(NT_GTEQ, (A), (B), pool)
#define NODE_EQ(A, B) _createNodeBinary(NT_EQ, (A), (B), pool)
#define NODE_NEQ(A, B) _createNodeBinary(NT_NEQ, (A), (B), pool)

#define NODE_RGB(A, B, C) _createNodeTernary(NT_RGB, (A), (B), (C), pool)

static Node *_createNodeNumber(double number, MemPool *pool) {
	Node *num = NODE(NT_NUM);
	num->num = number;

	return num;
}

static Node *_createNodeUnary(NodeType type, Node *a, MemPool *pool) {
	Node *node = NODE(type);
	node->a = a;

	return node;
}

static Node *_createNodeBinary(NodeType type, Node *a, Node *b, MemPool *pool) {
	Node *node = NODE(type);
	node->a = a;
	node->b = b;

	return node;
}

static Node *_createNodeTernary(
	NodeType type, Node *a, Node *b, Node *c, MemPool *pool) {
	Node *node = NODE(type);
	node->a = a;
	node->b = b;
	node->c = c;

	return node;
}

/* Generates a random number in the range [-1, 1] */
static double _randNumber(void) {
	double num = (double)rand();
	return 2 * (num / RAND_MAX) - 1;
}

static Node *_createRandomNode(MemPool *pool) {
	switch( rand() % 5 ) {
	case 0:
		return NODE_ADD(NODE_RANDOM(), NODE_RANDOM());
	case 1:
		return NODE_MUL(NODE_RANDOM(), NODE_RANDOM());
	case 2:
		return NODE_NUM(_randNumber());
	case 3:
		return NODE(NT_X);
	case 4:
		return NODE(NT_Y);
	}

	return NULL;
}

static Node *_createAST(MemPool *pool) {
	return NODE_RGB(NODE_RANDOM(), NODE_RANDOM(), NODE_RANDOM());
}

#define RUN(S) (_runAST((S), x, y, pool))

#define COND(S) ((RUN(ast->a) S RUN(ast->b)) ? NODE_NUM(1) : NODE_NUM(-1))
#define ARITH(O) (NODE_NUM(RUN(ast->a)->num O RUN(ast->b)->num))

Node *rnum, *rrgb;

static Node *_runAST(Node *ast, double x, double y, MemPool *pool) {
	if( ast == NULL ) {
		return NULL;
	}

	switch( ast->type ) {
	case NT_NUM:
		return ast;
	case NT_X:
		return NODE_NUM(x);
	case NT_Y:
		return NODE_NUM(y);
	case NT_ADD:
		return ARITH(+);
	case NT_SUB:
		return ARITH(-);
	case NT_MUL:
		return ARITH(*);
	case NT_DIV:
		return ARITH(/);
	case NT_MOD:
		return NODE_NUM(fmod(RUN(ast->a)->num, RUN(ast->b)->num));
	case NT_IF:
		return (RUN(ast->a) > 0) ? RUN(ast->b) : RUN(ast->c);
	case NT_LT:
		return COND(<);
	case NT_LTEQ:
		return COND(<=);
	case NT_GT:
		return COND(>);
	case NT_GTEQ:
		return COND(>=);
	case NT_EQ:
		return COND(==);
	case NT_NEQ:
		return COND(!=);
	case NT_RGB:
		return NODE_RGB(RUN(ast->a), RUN(ast->b), RUN(ast->c));
	default:
		fprintf(stderr, "not handled yet (Node[%d]), skipping\n", ast->type);
	}

	return ast;
}

/* XY as intensity ([0, IMAGE_SIZE] -> [-1, 1]) */
#define XY_INT(V) (2 * ((double)(V) / IMAGE_SIZE) - 1)
#define INT_C(C) ((int)((((C) + 1) / 2) * 255))

static byte *_interpretAST(Node *ast, int w, int h) {
	byte *image = malloc(w * h * 3);
	MemPool *vmPool = _poolNew();

	int i = 0;
	for( int y = 0; y < h; ++y ) {
		for( int x = 0; x < w; ++x ) {
			Node *node = _runAST(ast, XY_INT(x), XY_INT(y), vmPool);
			image[i++] = INT_C(node->a->num);
			image[i++] = INT_C(node->b->num);
			image[i++] = INT_C(node->c->num);

			free(vmPool);
			vmPool = _poolNew();
		}
	}

	free(vmPool);

	return image;
}

#define F(N) printf(N "(")
#define P(A) _printAST((A))
#define COMMA printf(", ")
#define ENDF printf(")")

#define UNARY(N)                                                               \
	F(N);                                                                      \
	P(ast->a);                                                                 \
	ENDF

#define BINARY(N)                                                              \
	F(N);                                                                      \
	P(ast->a);                                                                 \
	COMMA;                                                                     \
	P(ast->b);                                                                 \
	ENDF

#define TERNARY(N)                                                             \
	F(N);                                                                      \
	P(ast->a);                                                                 \
	COMMA;                                                                     \
	P(ast->b);                                                                 \
	COMMA;                                                                     \
	P(ast->c);                                                                 \
	ENDF

static void _printAST(Node *ast) {
	if( ast == NULL ) {
		return;
	}

	switch( ast->type ) {
	case NT_NUM:
		printf("%g", ast->num);
		break;
	case NT_X:
		printf("x");
		break;
	case NT_Y:
		printf("y");
		break;
	case NT_ADD:
		BINARY("add");
		break;
	case NT_SUB:
		BINARY("sub");
		break;
	case NT_MUL:
		BINARY("mul");
		break;
	case NT_DIV:
		BINARY("div");
		break;
	case NT_MOD:
		BINARY("mod");
		break;
	case NT_IF:
		TERNARY("if");
		break;
	case NT_GT:
		BINARY("gt");
		break;
	case NT_RGB:
		TERNARY("rgb");
		printf("\n");
		break;
	default:
		printf("?");
		break;
	}
}

#undef F
#undef COMMA
#undef ENDF

static byte *_generateRandomArt(int w, int h, MemPool *pool) {
	Node *ast = _createAST(pool);
	_printAST(ast);

	return _interpretAST(ast, w, h);
}

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
	srand(time(NULL));

	MemPool *astPool = _poolNew();
	byte *image = _generateRandomArt(IMAGE_SIZE, IMAGE_SIZE, astPool);

	_writePPM("image.ppm", IMAGE_SIZE, IMAGE_SIZE, image);

	free(astPool);
	free(image);

	return EXIT_SUCCESS;
}
