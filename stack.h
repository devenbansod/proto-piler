/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef STACK_INTERFACE
#define STACK_INTERFACE
#include "stackDef.h"

Stack* createStack(Stack *s, int size);
void push(stackElem t, Stack *s);
stackElem pop(Stack *s);
int getSize(Stack *s);
int isEmpty(Stack *s);

#endif