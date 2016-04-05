/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef TREE_DEF
#define TREE_DEF

#include "lexerDef.h"
#include "symbolTableDef.h"

/*
 * Structure for Parse Tree Node
 */
struct treeNode {
	struct treeNode* parent;
	struct treeNode* next;
	struct treeNode **children;
	Symbol symbol_type;
	tokenInfo tk_info;
	int processed_children;
	int curr_children;

	// Points to its correct symbol table
	// tk_info.lexeme is used to index
	SymbolTable* st;
};

typedef struct treeNode treeNode;

/*
 * Structure for Parse Tree
 */
struct parseTree {
	treeNode *root;
	int size;

	TypeTable *typeTbl;
};

typedef struct parseTree parseTree;

#endif