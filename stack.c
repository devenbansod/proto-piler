#include "common.h"
#include "stack.h"

void createStack(Stack *s) {
	s->top = NULL;
	s->size = 0;
}

void push(Symbol t, Stack *s) {
	if (s->size == 0) {
		s->top = (stackElem*)malloc(sizeof(stackElem));
		s->top->t = t;
		s->top->next = NULL;
	} else {
		stackElem *temp = (stackElem*)malloc(sizeof(stackElem));
		temp->next = s->top;
		temp->t = t;
		s->top = temp;
	}
	s->size++;
}


Symbol pop(Stack *s) {
	Symbol ret;

	if (s->size == 0) {
		ret.error = -1;
	} else {
		stackElem *temp = s->top->next;
		ret = s->top->t;
		free(s->top);
		s->top = temp;
		s->size--;
	}
	return ret;
}


int getSize(Stack *s) {
	return s->size;
}
