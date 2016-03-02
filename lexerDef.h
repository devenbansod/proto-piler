#ifndef LEXER_DEF
#define LEXER_DEF

struct DFA_state {
	int state_id;
	int final;
	int error;
};

typedef struct DFA_state DFA_state;

struct tokenInfo {
    Symbol symbol_type;
    int line_no;
    char lexeme[100];
    int error;
    int valueIfNum;
};

typedef struct tokenInfo tokenInfo;

#endif
