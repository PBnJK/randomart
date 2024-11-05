/* RandomArt
 * AST node
 */

#include <assert.h>
#include <stdlib.h>

#include "mempool.h"

#include "node.h"

/* Generates a random number in the range [-1, 1] */
static double _randNumber(void) {
	double num = (double)rand();
	return 2 * (num / RAND_MAX) - 1;
}

Node *nodeNew(NodeType type, MemPool *pool) {
	Node *node = poolAlloc(pool, sizeof(*node));

	node->type = type;

	node->a = NULL;
	node->b = NULL;
	node->c = NULL;

	node->num = 0.0;

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

#define VALUE_CHANCE 40
#define ARITH_CHANCE 46
#define TRIG_CHANCE 6
#define COND_CHANCE 2
#define MISC_CHANCE 6

static_assert(
	VALUE_CHANCE + ARITH_CHANCE + TRIG_CHANCE + COND_CHANCE + MISC_CHANCE
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

	if( rand() % 2 ) {
		return NODE_SIN(NODE_RANDOM(*rec));
	}

	return NODE_COS(NODE_RANDOM(*rec));
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

Node *nodeCreateRandom(MemPool *pool, int rec) {
	if( rec > 6 ) {
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

	if( r < COND_CHANCE ) {
		return _getCond(pool, &rec);
	}
	r -= COND_CHANCE;

	return NODE_RGB(NODE_RANDOM(rec), NODE_RANDOM(rec), NODE_RANDOM(rec));
}
