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
#define INT_WIDTH 4
#define REAL_WIDTH 8
/*
 * Structure for Type table elem
 */
struct typeTableElem {
    char type_name[25];
    int type_len;
    
    int fields_count; // no. of fields in record defn, 0 for INT or REAL
    char **field_names; //array for storing names of fields
    char **field_types; //array for storing types of fields 
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
    char *type; // acts as a key for type-table
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

/*
 * Structure for function table elem
 */
struct functionTableElem {
    char *id;
    int id_len;
    char **input_types;
    char **output_types;
    char **input_ids;
    char **output_ids; // array of
    int input_len;
    int output_len; 
    struct functionTableElem* next;
};

typedef struct functionTableElem functionTableElem;

// hash table with separate chaining
struct functionTable {
    functionTableElem **functionArray;
    int size;
    int curr_size;
};

typedef struct functionTable FunctionTable;


#endif