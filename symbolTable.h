#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#define MAX_BUCKETS 337
#define MAX_CHAIN_LENGTH 10

// Might be required later
struct sym_value {
    int temp;
};

struct symbol {
    char *id;
    int id_len;
    int unique_id;
    struct sym_value v;
    struct symbol* next;
};

typedef struct symbol symbol;

struct symbolTable {
    symbol **symbolArray;
    int size;
    int curr_size;
};

typedef struct symbolTable SymbolTable;

SymbolTable* createSymbolTable(int size);
void destroySymbolTable(SymbolTable *st);
void insertSymbol(SymbolTable* st, char *id, int id_len, struct sym_value v);
symbol* lookupSymbol(SymbolTable* st, char *id, int id_len);

#endif
