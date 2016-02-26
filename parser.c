#include "common.h"
#include "parserDef.h"



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
	Stack *s = (Stack)malloc(sizeof(Stack));
	createStack(s);

	// push the EOS
	// push the start symbol

	// create the Root Tree node with Start symbol


	// ALGO on pg. 227
	// OR
	// https://courses.cs.washington.edu/courses/cse401/04sp/slides/03b-LL1-example.pdf

}