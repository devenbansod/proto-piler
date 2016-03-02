#ifndef TREE_DEF
#define TREE_DEF

#include "lexerDef.h"

struct treeNode {
	struct treeNode* parent;
	struct treeNode **children;
	Symbol symbol_type;
	tokenInfo tk_info;
	int processed_children;
	int curr_children;
};

typedef struct treeNode treeNode;


struct parseTree {
	treeNode *root;
	int size;
};

typedef struct parseTree parseTree;

#endif