#ifndef PARSER_INTERFACE
#define PARSER_INTERFACE

parseTable** createParseTable(grammarRule *G, parseTable *T);
parseTree* createParseTree(parseTree *t);

// parseTree parseInputSourceCode(char *testcaseFile, parseTable** T);
parseTree* parseInputSourceCode(char *testcaseFile);

void printParseTree(parseTree *PT, char *outfile);

#endif