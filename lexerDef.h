/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef LEXER_DEF
#define LEXER_DEF

/*
 * Structure for DFA state
 */
struct DFA_state {
	int state_id;
	int final;
	int error;
};

typedef struct DFA_state DFA_state;

/*
 * Structure for collecting all information in lexer
 */
struct tokenInfo {
    Symbol symbol_type;
    int line_no;
    char lexeme[100];
    int error;
    int valueIfNum;
};

typedef struct tokenInfo tokenInfo;

#endif
