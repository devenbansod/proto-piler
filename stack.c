/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "stackDef.h"

/*
 * Create a new stack and return
 */
Stack* createStack(Stack *s, int size) {
    s = (Stack *)malloc(sizeof(Stack));
    s->top = -1;
    s->size = size;
    s->arr = (stackElem*)malloc(sizeof(stackElem) * size);
    return s;
}

/*
 * Push an element onto stack
 */
void push(stackElem t, Stack *s) {
    if (s->size > s->top + 1) {
        s->top++;
        s->arr[s->top] = t;
    } else {
        s->arr = (stackElem*)realloc(s->arr, sizeof(stackElem) * s->size * 2);
    }
}

/*
 * Pop an element from top of stack
 */
stackElem pop(Stack *s) {
    stackElem ret;

    if (s->size == 0) {
        ret.error = -1;
    } else {
        ret = s->arr[s->top--];
    }

    return ret;
}


/*
 * Returns the current number of elements in stack
 */
int getSize(Stack *s) {
    return s->top;
}

/*
 * Checks if the stack is empty
 */
int isEmpty(Stack *s) {
    return s->top == -1;
}
