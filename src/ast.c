/* RandomArt
 * AST generator
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "mempool.h"
#include "node.h"

#include "ast.h"

#define RUN(S) (_runAST((S), x, y, pool))

#define COND(S) ((RUN(ast->a) S RUN(ast->b)) ? NODE_NUM(1) : NODE_NUM(-1))
#define ARITH(O) (NODE_NUM(RUN(ast->a)->num O RUN(ast->b)->num))

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
	case NT_SIN:
		return NODE_NUM(sin(RUN(ast->a)->num));
	case NT_COS:
		return NODE_NUM(sin(RUN(ast->a)->num));
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
	}

	return ast;
}

/* XY as intensity ([0, DIMENSION] -> [-1, 1]) */
#define X_INT (2 * ((double)(x) / w) - 1)
#define Y_INT (2 * ((double)(y) / h) - 1)

/* RGB from intensity */
#define INT_C(C) ((int)((((C) + 1) / 2) * 255))

byte *astRun(Node *ast, int w, int h) {
	byte *image = malloc(w * h * 3);
	MemPool vmPool = poolNew();

	int i = 0;
	for( int y = 0; y < h; ++y ) {
		for( int x = 0; x < w; ++x ) {
			Node *node = _runAST(ast, X_INT, Y_INT, &vmPool);
			image[i++] = INT_C(node->a->num);
			image[i++] = INT_C(node->b->num);
			image[i++] = INT_C(node->c->num);

			poolFree(&vmPool);
			vmPool = poolNew();
		}
	}

	poolFree(&vmPool);

	return image;
}

#define F(N) printf("%s(", N)
#define P(A) astPrint((A))
#define COMMA printf(", ")
#define ENDF printf(")")

#define UNARY()                                                                \
	F(UNARY_NAME_TABLE[ast->type]);                                            \
	P(ast->a);                                                                 \
	ENDF

#define BINARY()                                                               \
	F(BINARY_NAME_TABLE[ast->type]);                                           \
	P(ast->a);                                                                 \
	COMMA;                                                                     \
	P(ast->b);                                                                 \
	ENDF

#define TERNARY()                                                              \
	F(TERNARY_NAME_TABLE[ast->type]);                                          \
	P(ast->a);                                                                 \
	COMMA;                                                                     \
	P(ast->b);                                                                 \
	COMMA;                                                                     \
	P(ast->c);                                                                 \
	ENDF

static const char *UNARY_NAME_TABLE[] = {
	[NT_SIN] = "sin",
	[NT_COS] = "cos",
};

static const char *BINARY_NAME_TABLE[] = {
	[NT_ADD] = "add",
	[NT_SUB] = "sub",
	[NT_MUL] = "mul",
	[NT_DIV] = "div",
	[NT_MOD] = "mod",
	[NT_LT] = "<",
	[NT_LTEQ] = "<=",
	[NT_GT] = ">",
	[NT_GTEQ] = ">=",
	[NT_EQ] = "==",
	[NT_NEQ] = "!=",
};

static const char *TERNARY_NAME_TABLE[] = {
	[NT_IF] = "if",
	[NT_RGB] = "rgb",
};

void astPrint(Node *ast) {
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
	case NT_SIN:
	case NT_COS:
		UNARY();
		break;
	case NT_ADD:
	case NT_SUB:
	case NT_MUL:
	case NT_DIV:
	case NT_MOD:
	case NT_LT:
	case NT_LTEQ:
	case NT_GT:
	case NT_GTEQ:
	case NT_EQ:
	case NT_NEQ:
		BINARY();
		break;
	case NT_IF:
	case NT_RGB:
		TERNARY();
		break;
	}
}

Node *astCreate(MemPool *pool) {
	return NODE_RGB(NODE_RANDOM(0), NODE_RANDOM(0), NODE_RANDOM(0));
}

byte *astGenerateArt(int w, int h) {
	MemPool pool = poolNew();

	Node *ast = astCreate(&pool);
	astPrint(ast);
	byte *image = astRun(ast, w, h);

	poolFree(&pool);

	return image;
}
