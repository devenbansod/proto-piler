/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef PARSER_INTERFACE
#define PARSER_INTERFACE

parseTable** createParseTable(grammarRule *G, parseTable *T);
parseTree* createParseTree(parseTree *t);
parseTree* parseInputSourceCode(char *testcaseFile, int *error);
void printParseTree(parseTree *PT, char *outfile);

#endif