#ifndef LEXER_DEF
#define LEXER_DEF

struct state {
    int state_id;
    int final;
    int error;
};

typedef struct state State;

#endif
