#include "fileBufferDef.h"

#ifndef LEXER_INTERFACE
#define LEXER_INTERFACE

void initStateNames();

State getNextToken(
    FileBuffer *buf,
    int *line,
    char *lexeme
);

void lexicalAnalysis(char *filepath);

#endif
