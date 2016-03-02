#ifndef PARSER_DEF
#define PARSER_DEF

/* ONLY FOR TESTS */
// #define START_TERMINAL 27
// #define START_NON_TERMINAL 41
// #define COUNT_TERMINAL 7
// #define COUNT_NON_TERMINAL 5
// #define COUNT_RULE 8
// #define EPS 33
// #define DOLLAR 32

#define START_TERMINAL 1
#define START_NON_TERMINAL 201
#define COUNT_TERMINAL 54
#define COUNT_NON_TERMINAL 54
#define COUNT_RULE 95
#define EPS 500
#define DOLLAR 1000
#define DOLLAR_STORE 54
#define EOS 1000

#include "stackDef.h"
#include "symbolTableDef.h"

struct grammarRule
{
    Symbol lhs;
    Symbol rhs[20];
    int size_rhs;
};

typedef struct grammarRule grammarRule;


// Row-index is the Non-terminal on top of stack
// Column-index is the look-ahead symbol read
// Value refers to a particular grammar rule.
typedef int parseTable;

#endif
