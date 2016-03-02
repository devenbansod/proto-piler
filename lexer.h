#ifndef LEXER_INTERFACE
#define LEXER_INTERFACE
#include "fileBufferDef.h"
#include "keywordTrieDef.h"

void initStateNames();

FILE* getStream(FILE *fp, FileBuffer *b, int k);

tokenInfo getNextToken(
    FileBuffer *buf,
    int *line,
    char *lexeme
);

void lexicalAnalysis(FILE *fp, int k);

void reportError (FILE *fp, int error_code, tokenInfo err_tok);

#endif
