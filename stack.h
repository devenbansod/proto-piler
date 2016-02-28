#ifndef STACK_INTERFACE
#define STACK_INTERFACE
#include "stackDef.h"

void createStack(Stack *s);
void push(termNonTerm t, Stack *s);
termNonTerm pop(Stack *s);
int getSize(Stack *s);
int isEmpty(Stack *s);
stackElem getElem(symbolType, int isTerminal, int error);

#endif