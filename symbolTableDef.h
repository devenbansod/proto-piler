/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef SYMBOL_TABLE_DEF
#define SYMBOL_TABLE_DEF

#include "lexerDef.h"

#define MAX_BUCKETS 337
#define MAX_CHAIN_LENGTH 10

// Might be required later
struct sym_value {
    int temp;
};

/*
 * Structure for Symbol table elem
 */
struct symbolTableElem {
    char *id;
    int id_len;
    Symbol symbol_name;
    int unique_id;
    int isTerminal;
    struct sym_value v;
    struct symbolTableElem* next;
};

typedef struct symbolTableElem symbolTableElem;

// hash table with separate chaining
struct symbolTable {
    symbolTableElem **symbolArray;
    int size;
    int curr_size;
};

typedef struct symbolTable SymbolTable;

#endif
