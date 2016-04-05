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

/*
 * Structure for Type table elem
 */
struct typeTableElem {
    char type_name[25];
    int type_len;

    int fields_count; // no. of fields in record defn, 0 for INT or REAL
    int* width; // array for storing widths of fields
    int* offset; // array for storing offsets of fields
    struct typeTableElem* next;
};

typedef struct typeTableElem typeTableElem;

// hash table with separate chaining
struct typeTable {
    typeTableElem **typeArray;
    int size;
    int curr_size;
};

typedef struct typeTable TypeTable;

/*
 * Structure for Symbol table elem
 */
struct symbolTableElem {
    char *lexeme;
    int lex_len;
    char type[25]; // acts as a key for type-table
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
