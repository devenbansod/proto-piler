#ifndef LEXER_DEF
#define LEXER_DEF

struct state {
    int state_id;
    int final;
    int error;
    int line_no;
    char lexeme[100];
};

typedef struct state State;

#endif
