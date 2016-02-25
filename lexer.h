#include "fileBufferDef.h"

#ifndef LEXER_INTERFACE
#define LEXER_INTERFACE

void initStateNames();

FILE* getStream(FILE *fp, FileBuffer *b, int k);

tokenInfo getNextToken(
    FileBuffer *buf,
    int *line,
    char *lexeme
);

void lexicalAnalysis(FILE *fp, int k);

#endif
