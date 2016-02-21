#ifndef SYMBOL_TABLE_INTERFACE
#define SYMBOL_TABLE_INTERFACE

SymbolTable* createSymbolTable(int size);
void destroySymbolTable(SymbolTable *st);
void insertSymbol(SymbolTable* st, char *id, int id_len, struct sym_value v);
symbol* lookupSymbol(SymbolTable* st, char *id, int id_len);

#endif
