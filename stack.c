#include "common.h"
#include "stackDef.h"

Stack* createStack(Stack *s, int size) {
    s = (Stack *)malloc(sizeof(Stack));
    s->top = -1;
    s->size = size;
    s->arr = (stackElem*)malloc(sizeof(stackElem) * size);
    return s;
}

void push(stackElem t, Stack *s) {
    if (s->size > s->top + 1) {
        s->top++;
        s->arr[s->top] = t;
    } else {
        s->arr = (stackElem*)realloc(s->arr, sizeof(stackElem) * s->size * 2);
    }
}

stackElem pop(Stack *s) {
    stackElem ret;

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
