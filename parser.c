#include "common.h"
#include "parserDef.h"

termNonTerm getTermNonTerm(int isTerminal, symbolType st, int error) {
	termNonTerm t;
	t.isTerminal = isTerminal;
	if (isTerminal) {
		t.st.b = st.a;
	} else {
		t.st.a = st.b;
	}

	t.error = error;

	return t;
}

treeNode* getTreeNode(
	int isTerminal,
	treeNode *parent, symbolType st
) {
	treeNode* new_node = (treeNode*)malloc(sizeof(treeNode));
	new_node->isTerminal = isTerminal;
	new_node->parent = parent;
	new_node->st = st;
	new_node->curr_children = 0;

	return new_node;
}

void setTokenInfo(treeNode *t_node, tokenInfo tk_info) {
	t_node->t = tk_info;
}

void createParseTree(parseTree *new_tree) {
	new_tree = (parseTree*)malloc(sizeof(parseTree));
	new_tree->root = NULL;
	new_tree->size = 0;
}


parseTree parseInputSourceCode(char *testcaseFile, parseTable T) {

	// initialize new parse tree
	parseTree *new_tree;
	createParseTree(new_tree);

	// initialize the new stack
	Stack *s;
	createStack(s);

	// push the EOS
	symbolType st; st.isTerminal = 0; st.a = EOS;
	termNonTerm bottom = getTermNonTerm(0, st, 0);
	push(bottom, s);

	// push the start symbol
	st.isTerminal = 0; st.a = program;
	termNonTerm start_sym = getTermNonTerm(0, st, 0);
	push(start_sym, s);

	// create the Root Tree node with Start symbol


	// ALGO on pg. 227
	// OR
	// https://courses.cs.washington.edu/courses/cse401/04sp/slides/03b-LL1-example.pdf

}

