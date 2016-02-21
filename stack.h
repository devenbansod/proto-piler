#ifndef STACK_INTERFACE
#define STACK_INTERFACE

Stack* createStack();
void push(termNonTerm t, Stack *s);
termNonTerm pop(Stack *s);
int getSize(Stack *s);

#endif