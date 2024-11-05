#ifndef GUARD_RANDOMART_AST_H_
#define GUARD_RANDOMART_AST_H_

#include "mempool.h"
#include "node.h"

byte *astRun(Node *ast, int w, int h);
void astPrint(Node *ast);

Node *astCreate(MemPool *pool);

byte *astGenerateArt(int w, int h);

#endif // !GUARD_RANDOMART_AST_H_
