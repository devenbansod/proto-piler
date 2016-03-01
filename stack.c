#include "common.h"
#include "stackDef.h"

Stack* createStack(Stack *s, int size) {
    s = (Stack *)malloc(sizeof(Stack));
    s->top = -1;
    s->size = size;
    s->arr = (termNonTerm*)malloc(sizeof(termNonTerm) * size);
    return s;
}

void push(termNonTerm t, Stack *s) {
    if (s->size > s->top + 1) {
        s->top++;
        s->arr[s->top] = t;
    } else {
        s->arr = (termNonTerm*)realloc(s->arr, sizeof(termNonTerm) * s->size * 2);
    }
}

termNonTerm pop(Stack *s) {
    termNonTerm ret;

    if (s->size == 0) {
        ret.error = -1;
    } else {
        ret = s->arr[s->top--];
    }

    return ret;
}


int getSize(Stack *s) {
    return s->top;
}

int isEmpty(Stack *s) {
    return s->top == -1;
}
