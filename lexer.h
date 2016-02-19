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
    char *buf, int buf_len, int *start, int *line, int *check_error,
    char *lexeme
);

#endif