#ifndef PARSER_INTERFACE
#define PARSER_INTERFACE

void createParseTable(grammarRules *G, parseTable *T);
void createParseTree(parseTree *t);

parseTree parseInputSourceCode(char *testcaseFile, parseTable T);

void printParseTree(parseTree *PT, char *outfile);

#endif