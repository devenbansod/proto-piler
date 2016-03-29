/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef PARSER_INTERFACE
#define PARSER_INTERFACE


/*
 * Interface to Parser
 *
 */
parseTable** createParseTable(grammarRule *G, parseTable *T);
parseTree* createParseTree(parseTree *t);
parseTree* parseInputSourceCode(char *testcaseFile, int *error);
void printParseTree(parseTree *PT, char *outfile);


/*
 * Interface to First and follow set creation
 *
 */
void createFirstSet(grammarRule* G, grammarRule* firstset, int *fcount, int count);
void createFollowSet(grammarRule* G, grammarRule* followset, int fcount, int count);

#endif