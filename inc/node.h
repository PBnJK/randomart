#ifndef GUARD_RANDOMART_NODE_H_
#define GUARD_RANDOMART_NODE_H_

#include "mempool.h"

typedef enum _NodeType {
	/* Values */
	NT_NUM, /* Number */
	NT_X, /* X coordinate */
	NT_Y, /* Y coordinate */
	NT_T, /* T member */

	/* Arithmetic */
	NT_ADD, /* A + B */
	NT_SUB, /* A - B */
	NT_MUL, /* A * B */
	NT_DIV, /* A / B */
	NT_MOD, /* A % B */

	/* Trigonometry */
	NT_SIN, /* sin A */
	NT_COS, /* cos A */

	/* Exponential */
	NT_EXP, /* exp A */
	NT_LOG, /* log A */
	NT_SQRT, /* sqrt A */

	/* Common */
	NT_ABS, /* abs A */
	NT_MIN, /* min(A, B) */
	NT_MAX, /* max(A, B) */
	NT_FRACT, /* A - floor(A) */

	/* Conditionals */
	NT_IF, /* A ? B : C */
	NT_LT, /* A < B */
	NT_LTEQ, /* A <= B */
	NT_GT, /* A > B */
	NT_GTEQ, /* A >= B */
	NT_EQ, /* A == B */
	NT_NEQ, /* A != B */

	/* Shader */
	NT_RGB, /* rgb(A, B, C) */
	NT_MIX, /* lerp(A, B, C) */
} NodeType;

typedef struct _Node {
	NodeType type;
	double num;
	struct _Node *a, *b, *c;
} Node;

#define NODE(T) nodeNew((T), pool)
#define NODE_RANDOM(C) nodeCreateRandom(pool, (C))

#define NODE_NUM(N) nodeCreateNumber((N), pool)
#define NODE_X() NODE(NT_X, pool)
#define NODE_Y() NODE(NT_Y, pool)

#define NODE_ADD(A, B) nodeCreateAB(NT_ADD, (A), (B), pool)
#define NODE_SUB(A, B) nodeCreateAB(NT_SUB, (A), (B), pool)
#define NODE_MUL(A, B) nodeCreateAB(NT_MUL, (A), (B), pool)
#define NODE_DIV(A, B) nodeCreateAB(NT_DIV, (A), (B), pool)
#define NODE_MOD(A, B) nodeCreateAB(NT_MOD, (A), (B), pool)

#define NODE_SIN(A) nodeCreateA(NT_SIN, (A), pool)
#define NODE_COS(A) nodeCreateA(NT_COS, (A), pool)

#define NODE_EXP(A) nodeCreateA(NT_EXP, (A), pool)
#define NODE_LOG(A) nodeCreateA(NT_LOG, (A), pool)
#define NODE_SQRT(A) nodeCreateA(NT_SQRT, (A), pool)

#define NODE_ABS(A) nodeCreateA(NT_ABS, (A), pool)
#define NODE_MIN(A, B) nodeCreateAB(NT_MIN, (A), (B), pool)
#define NODE_MAX(A, B) nodeCreateAB(NT_MAX, (A), (B), pool)
#define NODE_FRACT(A) nodeCreateA(NT_FRACT, (A), pool)

#define NODE_IF(A, B, C) nodeCreateABC(NT_IF, (A), (B), (C), pool)
#define NODE_COND(T, A, B) nodeCreateAB((T), (A), (B), pool)
#define NODE_LT(A, B) nodeCreateAB(NT_LT, (A), (B), pool)
#define NODE_LTEQ(A, B) nodeCreateAB(NT_LTEQ, (A), (B), pool)
#define NODE_GT(A, B) nodeCreateAB(NT_GT, (A), (B), pool)
#define NODE_GTEQ(A, B) nodeCreateAB(NT_GTEQ, (A), (B), pool)
#define NODE_EQ(A, B) nodeCreateAB(NT_EQ, (A), (B), pool)
#define NODE_NEQ(A, B) nodeCreateAB(NT_NEQ, (A), (B), pool)

#define NODE_RGB(A, B, C) nodeCreateABC(NT_RGB, (A), (B), (C), pool)
#define NODE_MIX(A, B, C) nodeCreateABC(NT_MIX, (A), (B), (C), pool)

void nodeSetup(unsigned maxrec);

Node *newNode(NodeType type, MemPool *pool);

Node *nodeCreateNumber(double number, MemPool *pool);

Node *nodeCreateA(NodeType type, Node *a, MemPool *pool);
Node *nodeCreateAB(NodeType type, Node *a, Node *b, MemPool *pool);
Node *nodeCreateABC(NodeType type, Node *a, Node *b, Node *c, MemPool *pool);

/* Generates a random node, which recursively creates other random nodes
 *
 * The rec value is used to prevent stack overflows: if the recursion depth is
 * beyond a given treshold, we cap off this branch in the AST
 */
Node *nodeCreateRandom(MemPool *pool, int rec);

#endif // !GUARD_RANDOMART_NODE_H_
