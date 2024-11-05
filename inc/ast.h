#ifndef GUARD_RANDOMART_AST_H_
#define GUARD_RANDOMART_AST_H_

#include <stdbool.h>

#include "mempool.h"
#include "node.h"

/* Runs a created AST, returning an array of RGB triples (the image) */
byte *astRun(Node *ast, int w, int h);

/* Prints the AST as a lisp-like line */
void astPrint(Node *ast);

/* Randomly creates a list */
Node *astCreate(MemPool *pool);

/* Draws the list as a nice ASCII art image :-O */
void astDraw(Node *ast);

/* Does all of the above stuff (create; print; run;) returning the image */
byte *astGenerateArt(int w, int h, bool quiet);

#endif // !GUARD_RANDOMART_AST_H_
