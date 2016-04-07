/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef SYMBOL_TABLE_INTERFACE
#define SYMBOL_TABLE_INTERFACE

#include "symbolTableDef.h"

/*
 * SYMBOL TABLE
 */

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
int insertSymbol(SymbolTable* st, char *id, int id_len, char* type);

/*
 * Lookup for a particular key
 */
symbolTableElem* lookupSymbol(SymbolTable* st, char *id, int id_len);


/*
 * TYPE TABLE
 */

/*
 * Creates and returns a new table table
 */
TypeTable* createTypeTable(int size);

/*
 * Destroy and free a symbol table
 */
void destroyTypeTable(TypeTable *typeTbl);

/*
 * Insert element into the Symbol table
 */
int insertType(TypeTable* typeTbl, char* type, int id_len, int* width, int* offset, 
	char** field_names, char **field_types, int fields_count);

/*
 * Lookup for a particular key
 */
typeTableElem* lookupType(TypeTable* typeTbl, char *id, int id_len);

void printTypeTable(TypeTable *typeTbl);

#endif
