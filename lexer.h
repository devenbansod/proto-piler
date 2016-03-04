/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

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

void printCommentFreeSource(char *filename);

void lexicalAnalysis(FILE *fp, int k);

void reportError (FILE *fp, int error_code, tokenInfo err_tok);

#endif
