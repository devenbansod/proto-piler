#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#define MAX_BUCKETS 337

struct symbol {
	char *id;
	int id_len;
	int unique_id;
	struct symbol* next;
};

typedef struct symbol symbol;

struct symbolTable {
	symbol* symbolArray[MAX_BUCKETS];
	int size;
};

typedef struct symbolTable SymbolTable;

void createSymbolTable(SymbolTable* st);
void destroySymbolTable(SymbolTable *st);
void insertSymbol(SymbolTable* st, char *id, int id_len);
symbol* lookupSymbol(SymbolTable* st, char *id, int id_len);

#endif