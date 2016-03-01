#ifndef LEXER_DEF
#define LEXER_DEF

typedef enum {
	TK_SQL = 1,
	TK_SQR,
	TK_SEM,
	TK_COLON,
	TK_DOT,
	TK_OP,
	TK_CL,
	TK_PLUS,
	TK_MINUS,
	TK_MUL,
	TK_DIV,
	TK_NOT,
	TK_NE,
	TK_EQ,
	TK_GE,
	TK_GT,
	TK_LE,
	TK_ASSIGNOP,
	TK_LT,
	TK_AND,
	TK_RECORDID,
	TK_OR,
	TK_COMMENT,
	TK_RNUM,
	TK_NUM,
	TK_FUNID,
	TK_ID,
	TK_FIELDID,
	TK_COMMA,
	TK_WITH,
	TK_PARAMETERS,
	TK_END,
	TK_WHILE,
	TK_INT,
	TK_REAL,
	TK_TYPE,
	TK_MAIN,
	TK_GLOBAL,
	TK_PARAMETER,
	TK_LIST,
	TK_INPUT,
	TK_OUTPUT,
	TK_ENDWHILE,
	TK_IF,
	TK_THEN,
	TK_ENDIF,
	TK_READ,
	TK_WRITE,
	TK_RETURN,
	TK_RECORD,
	TK_ENDRECORD,
	TK_ELSE,
	TK_CALL,
	ERROR = -1,
	EOI = 100
} Terminal;

struct DFA_state {
	int state_id;
	int final;
	int error;
};

typedef struct DFA_state DFA_state;

struct tokenInfo {
    Terminal term_type;
    int line_no;
    char lexeme[100];
    int error;
};

typedef struct tokenInfo tokenInfo;

#endif
