#ifndef PARSER_INTERFACE
#define PARSER_INTERFACE

parseTable** createParseTable(grammarRules *G, parseTable *T);
parseTree* createParseTree(parseTree *t);

parseTree parseInputSourceCode(char *testcaseFile, parseTable T);

void printParseTree(parseTree *PT, char *outfile);

#endif