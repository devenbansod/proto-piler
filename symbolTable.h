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
int insertSymbol(SymbolTable* st, char *id, int id_len, char* type, int* offset);

/*
 * Lookup for a particular key
 */
symbolTableElem* lookupSymbol(SymbolTable* st, char *id, int id_len);

/*
 * print the symbol table
 */

void printSymbolTable(SymbolTable* st, char *scope);

/*
 * TYPE TABLE
 */

/*
 * Creates and returns a new type table
 */
TypeTable* createTypeTable(int size);

/*
 * Destroy and free memory for a type table
 */
void destroyTypeTable(TypeTable *typeTbl);

/*
 * Insert element into the type table
 */
int insertType(TypeTable* typeTbl, char* type, int id_len, int* width, int* offset, 
	char** field_names, char **field_types, int fields_count);

/*
 * Lookup for a particular key
 */
typeTableElem* lookupType(TypeTable* typeTbl, char *id, int id_len);

/*
 * Print type table on terminal
 */
void printTypeTable(TypeTable *typeTbl);

/*
 * FUNCTION TABLE
 */

/*
 * Creates and returns a new functions table
 */
FunctionTable* createFunctionTable(int size);

/*
 * Destroy and free a function table
 */
void destroyFunctionTable(FunctionTable *fxTable);

/*
 * Insert element into the function table
 */
int insertFunction(FunctionTable *fxTable, char *id, int id_len, 
    char **input_types, char **output_types, char **input_ids, char **output_ids, int input_len, int output_len);

/*
 * Lookup for a particular key
 */
functionTableElem* lookupFunction(FunctionTable* fxTable, char *id, int id_len);

/*
 * Print function table on terminal
 */
void printFunctionTable(FunctionTable *fxTable);

#endif