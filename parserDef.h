#include "stackDef.h"
#include "symbolTableDef.h"

#ifndef PARSER_DEF
#define PARSER_DEF

// Row-index is the Non-terminal on top of stack
// Column-index is the look-ahead symbol read
// Value refers to a particular grammar rule.

struct parseTable {
	int pt[100][100];
};

typedef struct parseTable parseTable;

struct treeNode {
	int isTerminal;
	struct treeNode* parent;
	int curr_children;
	struct treeNode* children[10];
	symbolType st;
	tokenInfo t;
};

typedef struct treeNode treeNode;


struct parseTree {
	treeNode *root;
	int size;
};

typedef struct parseTree parseTree;

#endif
