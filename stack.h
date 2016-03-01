#ifndef STACK_INTERFACE
#define STACK_INTERFACE
#include "stackDef.h"

Stack* createStack(Stack *s, int size);
void push(termNonTerm t, Stack *s);
termNonTerm pop(Stack *s);
int getSize(Stack *s);
int isEmpty(Stack *s);
termNonTerm getElem(symbolType, int isTerminal, int error);

#endif