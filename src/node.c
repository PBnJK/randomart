/* RandomArt
 * AST node
 */

#include <assert.h>
#include <stdlib.h>

#include "mempool.h"

#include "node.h"

static unsigned _maxrec = 0;

/* Generates a random number in the range [-1, 1] */
static double _randNumber(void) {
	double num = (double)rand();
	return 2.0 * (num / RAND_MAX) - 1.0;
}

void nodeSetup(unsigned maxrec) {
	_maxrec = maxrec;
}

Node *nodeNew(NodeType type, MemPool *pool) {
	Node *node = poolAlloc(pool, sizeof(*node));
	node->type = type;

	return node;
}

Node *nodeCreateNumber(double number, MemPool *pool) {
	Node *num = NODE(NT_NUM);
	num->num = number;

	return num;
}

Node *nodeCreateA(NodeType type, Node *a, MemPool *pool) {
	Node *node = NODE(type);
	node->a = a;

	return node;
}

Node *nodeCreateAB(NodeType type, Node *a, Node *b, MemPool *pool) {
	Node *node = NODE(type);
	node->a = a;
	node->b = b;

	return node;
}

Node *nodeCreateABC(NodeType type, Node *a, Node *b, Node *c, MemPool *pool) {
	Node *node = NODE(type);
	node->a = a;
	node->b = b;
	node->c = c;

	return node;
}

#define VALUE_CHANCE 20
#define ARITH_CHANCE 20
#define TRIG_CHANCE 20
#define EXP_CHANCE 10
#define COMMON_CHANCE 10
#define COND_CHANCE 10
#define SHADER_CHANCE 10

static_assert(VALUE_CHANCE + ARITH_CHANCE + TRIG_CHANCE + EXP_CHANCE
			+ COMMON_CHANCE + COND_CHANCE + SHADER_CHANCE
		== 100,
	"chances must add up to 100%");

static Node *_getValue(MemPool *pool) {
	switch( rand() % 5 ) {
	case 0:
		return NODE_NUM(_randNumber());
	case 1:
		return NODE(NT_X);
	}

	return NODE(NT_Y);
}

static Node *_getArith(MemPool *pool, int *rec) {
	*rec += 2;

	switch( rand() % 5 ) {
	case 0:
		return NODE_ADD(NODE_RANDOM(*rec), NODE_RANDOM(*rec));
	case 1:
		return NODE_SUB(NODE_RANDOM(*rec), NODE_RANDOM(*rec));
	case 2:
		return NODE_MUL(NODE_RANDOM(*rec), NODE_RANDOM(*rec));
	case 3:
		return NODE_DIV(NODE_RANDOM(*rec), NODE_RANDOM(*rec));
	}

	return NODE_MOD(NODE_RANDOM(*rec), NODE_RANDOM(*rec));
}

static Node *_getTrig(MemPool *pool, int *rec) {
	*rec += 1;

	switch( rand() % 2 ) {
	case 0:
		return NODE_SIN(NODE_RANDOM(*rec));
	}

	return NODE_COS(NODE_RANDOM(*rec));
}

static Node *_getExp(MemPool *pool, int *rec) {
	*rec += 1;

	switch( rand() % 3 ) {
	case 0:
		return NODE_EXP(NODE_RANDOM(*rec));
	case 1:
		return NODE_LOG(NODE_RANDOM(*rec));
	}

	return NODE_SQRT(NODE_RANDOM(*rec));
}

static Node *_getCommon(MemPool *pool, int *rec) {
	switch( rand() % 4 ) {
	case 0:
		*rec -= 1;
		return NODE_ABS(NODE_RANDOM(*rec));
	case 1:
		*rec += 2;
		return NODE_MIN(NODE_RANDOM(*rec), NODE_RANDOM(*rec));
	case 2:
		*rec += 2;
		return NODE_MAX(NODE_RANDOM(*rec), NODE_RANDOM(*rec));
	}

	*rec += 2;
	return NODE_FRACT(NODE_RANDOM(*rec));
}

static Node *_getCond(MemPool *pool, int *rec) {
	*rec += 4;

	NodeType cond = NT_LT;
	switch( rand() % 6 ) {
	case 0:
		cond = NT_LT;
		break;
	case 1:
		cond = NT_LTEQ;
		break;
	case 2:
		cond = NT_GT;
		break;
	case 3:
		cond = NT_GTEQ;
		break;
	case 4:
		cond = NT_EQ;
		break;
	case 5:
		cond = NT_NEQ;
		break;
	}

	return NODE_IF( //
		NODE_COND(cond, NODE_RANDOM(*rec), NODE_RANDOM(*rec)), //
		NODE_RANDOM(*rec), //
		NODE_RANDOM(*rec) //
	);
}

static Node *_getShader(MemPool *pool, int *rec) {
	*rec += 3;

	switch( rand() % 2 ) {
	case 0:
		return NODE_RGB(
			NODE_RANDOM(*rec), NODE_RANDOM(*rec), NODE_RANDOM(*rec));
	}

	return NODE_MIX(NODE_RANDOM(*rec), NODE_RANDOM(*rec), NODE_RANDOM(*rec));
}

Node *nodeCreateRandom(MemPool *pool, int rec) {
	if( rec > _maxrec ) {
		return _getValue(pool);
	}

	unsigned r = rand() % 100;
	if( r < VALUE_CHANCE ) {
		return _getValue(pool);
	}
	r -= VALUE_CHANCE;

	if( r < ARITH_CHANCE ) {
		return _getArith(pool, &rec);
	}
	r -= ARITH_CHANCE;

	if( r < TRIG_CHANCE ) {
		return _getTrig(pool, &rec);
	}
	r -= TRIG_CHANCE;

	if( r < EXP_CHANCE ) {
		return _getExp(pool, &rec);
	}
	r -= EXP_CHANCE;

	if( r < COMMON_CHANCE ) {
		return _getCommon(pool, &rec);
	}
	r -= COMMON_CHANCE;

	if( r < COND_CHANCE ) {
		return _getCond(pool, &rec);
	}
	r -= COND_CHANCE;

	return _getShader(pool, &rec);
}
