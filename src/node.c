/* RandomArt
 * AST node
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "mempool.h"

#include "node.h"

static int _maxrec = 0;

/* Generates a random number in the range [0, 1] */
static double _rand(void) {
	double num = (double)rand();
	return (num / RAND_MAX);
}

static unsigned _valueChance, _arithChance, _trigChance, _expChance,
	_commonChance, _condChance;

void nodeSetup(unsigned maxrec, unsigned val, unsigned arith, unsigned trig,
	unsigned exp, unsigned common, unsigned cond) {
	_maxrec = maxrec;

	_valueChance = val;
	_arithChance = arith;
	_trigChance = trig;
	_expChance = exp;
	_commonChance = common;
	_condChance = cond;

	if( _valueChance + _arithChance + _trigChance + _expChance + _commonChance
			+ _condChance
		> 100 ) {
		printf("chances must add up to be at least less than 100%%\n");
		exit(EXIT_FAILURE);
	}
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

/* Gets a random number in range [0, n[ */
static int _randN(int n) {
	return rand() % n;
}

static Node *_getValue(MemPool *pool) {
	switch( _randN(3) ) {
	case 0:
		return NODE_NUM(_rand());
	case 1:
		return NODE_X();
	}

	/* Make random less likely because it kinda sucks */
	if( _randN(16) == 0 ) {
		return NODE_RAND();
	}

	return NODE_Y();
}

static Node *_getArith(MemPool *pool, int *rec) {
	*rec += 2;

	switch( _randN(5) ) {
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

	switch( _randN(2) ) {
	case 0:
		return NODE_SIN(NODE_RANDOM(*rec));
	}

	return NODE_COS(NODE_RANDOM(*rec));
}

static Node *_getExp(MemPool *pool, int *rec) {
	*rec += 1;

	switch( _randN(3) ) {
	case 0:
		return NODE_EXP(NODE_RANDOM(*rec));
	case 1:
		return NODE_LOG(NODE_RANDOM(*rec));
	}

	return NODE_SQRT(NODE_RANDOM(*rec));
}

static Node *_getCommon(MemPool *pool, int *rec) {
	switch( _randN(4) ) {
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
	switch( _randN(6) ) {
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

	switch( _randN(2) ) {
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

	double myRand = rand() / (1.0 + RAND_MAX);
	unsigned int range = 101;
	unsigned int r = myRand * range;

	if( r < _valueChance ) {
		return _getValue(pool);
	}
	r -= _valueChance;

	if( r < _arithChance ) {
		return _getArith(pool, &rec);
	}
	r -= _arithChance;

	if( r < _trigChance ) {
		return _getTrig(pool, &rec);
	}
	r -= _trigChance;

	if( r < _expChance ) {
		return _getExp(pool, &rec);
	}
	r -= _expChance;

	if( r < _commonChance ) {
		return _getCommon(pool, &rec);
	}
	r -= _commonChance;

	if( r < _condChance ) {
		return _getCond(pool, &rec);
	}
	r -= _condChance;

	return _getShader(pool, &rec);
}
