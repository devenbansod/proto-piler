/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef PARSER_DEF
#define PARSER_DEF

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

/*
 * Structure for Grammar rule in memory
 */
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
