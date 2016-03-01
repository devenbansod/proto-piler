#include "symbolTableDef.h"
#include "parserDef.h"

#ifndef STACK_DEF
#define STACK_DEF

struct termNonTerm {
    symbolType symbol_type;
    int isTerminal;
    int error;
};

typedef struct termNonTerm termNonTerm;

struct stack {
    int top;
    termNonTerm *arr;
    int size;
};

typedef struct stack Stack;

#endif