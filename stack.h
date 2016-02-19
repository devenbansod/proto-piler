#ifndef STACK
#define STACK

struct symbol {
	int state_id;
	int isTerminal;
	int error;
};

typedef struct symbol Symbol;

struct stackElem {
	struct symbol t;
	struct stackElem *next;
};

typedef struct stackElem stackElem;

struct stack {
	struct stackElem* top;
	int size;
};

typedef struct stack Stack;

#endif