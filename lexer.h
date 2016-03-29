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


/*
 * Interface to keyword_trie
 *
 */

// if present returns state_id, else returns -1
int checkIfKeyword(Trie* t, char *str, int len);

// For initiliasation from file I/O
void insertKeyword(Trie* t, char *str, int len, int value);

void initKeywordTrie();

#endif
