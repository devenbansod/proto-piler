/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef STACK_DEF
#define STACK_DEF
#include "treeDef.h"
#include "parserDef.h"


/*
 * Structure for Stack element
 */
struct stackElem {
    Symbol symbol_type;
    treeNode* t_node;
    int isTerminal;
    int error;
};

typedef struct stackElem stackElem;

/*
 * Structure for Stack - uses dynamic array
 *
 */
struct stack {
    int top;
    stackElem *arr;
    int size;
};

typedef struct stack Stack;

#endif