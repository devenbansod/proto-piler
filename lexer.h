#include "fileBuffer.h"
#ifndef LEXER
#define LEXER

struct state {
    int state_id;
    int final;
    int error;
};

typedef struct state State;

void initStateNames();

State getNextToken(
    FileBuffer *buf,
    int *line,
    char *lexeme
);

void lexicalAnalysis(char *filepath);

#endif
