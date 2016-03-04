/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef SYMBOL_TABLE_INTERFACE
#define SYMBOL_TABLE_INTERFACE

/*
 * Creates and returns a new symbol table
 */
SymbolTable* createSymbolTable(int size);

/*
 * Destroy and free a symbol table
 */
void destroySymbolTable(SymbolTable *st);

/*
 * Insert element into the Symbol table
 */
void insertSymbol(SymbolTable* st, char *id, int id_len, struct sym_value v);

/*
 * Lookup for a particular key
 */
SymbolTableElem* lookupSymbol(SymbolTable* st, char *id, int id_len);

#endif
