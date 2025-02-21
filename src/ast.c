/* RandomArt
 * AST generator
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mempool.h"
#include "node.h"

#include "ast.h"

static double _mix(double x, double y, double a) {
	return x * (1.0 - a) + y * a;
}

/* Gets a random number between [-1, 1] */
static double _rand(void) {
	const double NUM = (double)rand();
	const double MAX = (double)RAND_MAX;

	return (NUM / MAX) * 2.0 - 1.0;
}

#define RUN(S) (_runAST((S), x, y, t, pool))
#define RUNN(S) (_runAST((S), x, y, t, pool)->num)

#define COND(S) ((RUN(ast->a) S RUN(ast->b)) ? NODE_NUM(1) : NODE_NUM(0))
#define ARITH(O) (NODE_NUM(RUN(ast->a)->num O RUN(ast->b)->num))

static Node *_runAST(Node *ast, double x, double y, double t, MemPool *pool) {
	switch( ast->type ) {
	case NT_NUM:
		return ast;
	case NT_X:
		return NODE_NUM(x);
	case NT_Y:
		return NODE_NUM(y);
	case NT_T:
		return NODE_NUM(t);
	case NT_RAND: {
		return NODE_NUM(_rand());
	}
	case NT_ADD:
		return ARITH(+);
	case NT_SUB:
		return ARITH(-);
	case NT_MUL:
		return ARITH(*);
	case NT_DIV:
		return ARITH(/);
	case NT_MOD:
		return NODE_NUM(fmod(RUNN(ast->a), RUNN(ast->b)));
	case NT_SIN:
		return NODE_NUM(sin(RUNN(ast->a)));
	case NT_COS:
		return NODE_NUM(sin(RUNN(ast->a)));
	case NT_EXP:
		return NODE_NUM(exp(RUNN(ast->a)));
	case NT_LOG:
		return NODE_NUM(log(RUNN(ast->a)));
	case NT_SQRT:
		return NODE_NUM(sqrt(RUNN(ast->a)));
	case NT_ABS:
		return NODE_NUM(fabs(RUNN(ast->a)));
	case NT_MIN:
		return NODE_NUM(fmin(RUNN(ast->a), RUNN(ast->b)));
	case NT_MAX:
		return NODE_NUM(fmax(RUNN(ast->a), RUNN(ast->b)));
	case NT_FRACT: {
		double n = RUNN(ast->a);
		return NODE_NUM(n - floor(n));
	}
	case NT_IF:
		return (RUNN(ast->a) > 0) ? RUN(ast->b) : RUN(ast->c);
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
	case NT_MIX:
		return NODE_NUM(_mix(RUNN(ast->a), RUNN(ast->b), RUNN(ast->c)));
	}

	return ast;
}

/* XY as intensity ([0, DIMENSION] -> [-1, 1]) */
#define X_INT (2 * ((double)(x) / w) - 1)
#define Y_INT (2 * ((double)(y) / h) - 1)

/* RGB from intensity ([-1, 1] -> [0, 255]) */
#define INT_C(C) ((int)((((C) + 1) / 2) * 255))

byte *astDoFrame(Node *ast, int w, int h, double t) {
	byte *image = malloc(w * h * 3);
	MemPool vmPool;

	int i = 0;
	for( int y = 0; y < h; ++y ) {
		vmPool = poolNew();

		for( int x = 0; x < w; ++x ) {
			Node *node = _runAST(ast, X_INT, Y_INT, t, &vmPool);
			image[i++] = INT_C(node->a->num);
			image[i++] = INT_C(node->b->num);
			image[i++] = INT_C(node->c->num);
		}

		poolFree(&vmPool);
	}

	return image;
}

#define F(N) printf("(%s ", N)
#define P(A) _astPrint((A))
#define SPACE printf(" ")
#define ENDF printf(")")

#define UNARY()                                                                \
	F(UNARY_NAME_TABLE[ast->type]);                                            \
	P(ast->a);                                                                 \
	ENDF

#define BINARY()                                                               \
	F(BINARY_NAME_TABLE[ast->type]);                                           \
	P(ast->a);                                                                 \
	SPACE;                                                                     \
	P(ast->b);                                                                 \
	ENDF

#define TERNARY()                                                              \
	F(TERNARY_NAME_TABLE[ast->type]);                                          \
	P(ast->a);                                                                 \
	SPACE;                                                                     \
	P(ast->b);                                                                 \
	SPACE;                                                                     \
	P(ast->c);                                                                 \
	ENDF

static const char *UNARY_NAME_TABLE[] = {
	[NT_SIN] = "sin",
	[NT_COS] = "cos",
	[NT_EXP] = "exp",
	[NT_LOG] = "log",
	[NT_SQRT] = "sqrt",
	[NT_ABS] = "abs",
	[NT_FRACT] = "fract",
};

static const char *BINARY_NAME_TABLE[] = {
	[NT_ADD] = "add",
	[NT_SUB] = "sub",
	[NT_MUL] = "mul",
	[NT_DIV] = "div",
	[NT_MOD] = "mod",
	[NT_MIN] = "min",
	[NT_MAX] = "max",
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
	[NT_MIX] = "mix",
};

static void _astPrint(Node *ast) {
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
	case NT_T:
		printf("t");
		break;
	case NT_RAND:
		printf("rnd");
		break;
	case NT_SIN:
	case NT_COS:
	case NT_EXP:
	case NT_LOG:
	case NT_ABS:
	case NT_SQRT:
	case NT_FRACT:
		UNARY();
		break;
	case NT_ADD:
	case NT_SUB:
	case NT_MUL:
	case NT_DIV:
	case NT_MOD:
	case NT_MIN:
	case NT_MAX:
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
	case NT_MIX:
		TERNARY();
		break;
	}
}

void astPrint(Node *ast) {
	_astPrint(ast);
	printf("\n");
}

static bool _areEqual(Node *l, Node *r) {
	if( l->type != r->type ) {
		return false;
	}

	switch( l->type ) {
	case NT_NUM:
		return l->num == r->num;
	case NT_X:
	case NT_Y:
		return true;
	default:
		return false;
	}
}

static void _optimize(MemPool *pool, Node *parent, Node *ast) {
	switch( ast->type ) {
	case NT_NUM:
	case NT_X:
	case NT_Y:
	case NT_T:
	case NT_RAND:
		break;
	case NT_MIX:
		_optimize(pool, ast, ast->c);
		/* fallthrough */
	case NT_MIN:
	case NT_MAX:
		_optimize(pool, ast, ast->b);
		_optimize(pool, ast, ast->a);
		if( _areEqual(ast->a, ast->b) ) {
			*ast = *ast->a;
		}
		break;
	case NT_ADD:
	case NT_SUB:
	case NT_MUL:
	case NT_DIV:
	case NT_MOD:
		_optimize(pool, ast, ast->b);
		/* fallthrough */
	case NT_SIN:
	case NT_COS:
	case NT_EXP:
	case NT_LOG:
	case NT_SQRT:
	case NT_ABS:
	case NT_FRACT:
		_optimize(pool, ast, ast->a);
		break;
	case NT_LT:
	case NT_LTEQ:
	case NT_GT:
	case NT_GTEQ:
	case NT_EQ:
	case NT_NEQ:
		_optimize(pool, ast, ast->b);
		_optimize(pool, ast, ast->a);
		if( _areEqual(ast->a, ast->b) ) {
			*parent = *parent->b;
		}
		break;
	case NT_IF:
		if( _areEqual(ast->b, ast->c) ) {
			*ast = *ast->b;
			break;
		}

		_optimize(pool, ast, ast->c);
		_optimize(pool, ast, ast->b);
		_optimize(pool, ast, ast->a);
		break;
	case NT_RGB:
		_optimize(pool, ast, ast->c);
		_optimize(pool, ast, ast->b);
		_optimize(pool, ast, ast->a);
		break;
	}
}

Node *astCreate(MemPool *pool) {
	Node *ast = NODE_RGB(NODE_RANDOM(0), NODE_RANDOM(0), NODE_RANDOM(0));
	_optimize(pool, NULL, ast);

	return ast;
}

bool astInjectT(Node *ast) {
	switch( ast->type ) {
	case NT_NUM:
	case NT_X:
	case NT_Y:
	case NT_RAND:
		ast->type = NT_T;
		/* fallthrough */
	case NT_T:
		return true;
	case NT_IF:
	case NT_RGB:
	case NT_MIX:
		if( astInjectT(ast->c) ) {
			return true;
		}
		/* fallthrough */
	case NT_MIN:
	case NT_MAX:
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
		if( astInjectT(ast->b) ) {
			return true;
		}
		/* fallthrough */
	case NT_SIN:
	case NT_COS:
	case NT_EXP:
	case NT_LOG:
	case NT_SQRT:
	case NT_ABS:
	case NT_FRACT:
		return astInjectT(ast->a);
	}

	return false;
}
