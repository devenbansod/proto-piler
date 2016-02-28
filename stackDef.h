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

struct stackElem {
    struct termNonTerm t;
    struct stackElem *next;
};

typedef struct stackElem stackElem;

struct stack {
    struct stackElem* top;
    int size;
};

typedef struct stack Stack;

#endif