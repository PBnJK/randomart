#ifndef GUARD_RANDOMART_AST_H_
#define GUARD_RANDOMART_AST_H_

#include <stdbool.h>

#include "mempool.h"
#include "node.h"

/* Runs a created AST, returning an array of RGB triples (the image) */
byte *astDoFrame(Node *ast, int w, int h, double t);

/* Prints the AST as a lisp-like line */
void astPrint(Node *ast);

/* Randomly creates an AST */
Node *astCreate(MemPool *pool);

#endif // !GUARD_RANDOMART_AST_H_
