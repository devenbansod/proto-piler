#ifndef STACK_INTERFACE
#define STACK_INTERFACE
#include "stackDef.h"

Stack* createStack(Stack *s, int size);
void push(stackElem t, Stack *s);
stackElem pop(Stack *s);
int getSize(Stack *s);
int isEmpty(Stack *s);

#endif