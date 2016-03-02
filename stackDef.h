#ifndef STACK_DEF
#define STACK_DEF
#include "treeDef.h"
#include "symbolTableDef.h"
#include "parserDef.h"


struct stackElem {
    Symbol symbol_type;
    treeNode* t_node;
    int isTerminal;
    int error;
};

typedef struct stackElem stackElem;

struct stack {
    int top;
    stackElem *arr;
    int size;
};

typedef struct stack Stack;

#endif