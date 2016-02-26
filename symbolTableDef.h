#include "lexerDef.h"
#ifndef SYMBOL_TABLE_DEF
#define SYMBOL_TABLE_DEF

#define MAX_BUCKETS 337
#define MAX_CHAIN_LENGTH 10

typedef enum {
	program = 200,
	mainFunction,
	otherFunctions,
	function,
	input_par,
	output_par,
	parameter_list,
	dataType,
	primitiveDatatype,
	constructedDatatype,
	remaining_list,
	stmts,
	typeDefinitions,
	typeDefinition,
	fieldDefinitions,
	fieldDefinition,
	moreFields,
	declarations,
	declaration,
	global_or_not,
	otherStmts,
	stmt,
	assignmentStmt,
	singleOrRecId,
	new_24,
	funCallStmt,
	outputParameters,
	inputParameters,
	iterativeStmt,
	conditionalStmt,
	else_part,
	ioStmt,
	allVar,
	arithmeticExpression,
	term,
	factor,
	highPrecedenceOperator,
	lowPrecedenceOperators,
	all,
	temp,
	operator,
	booleanExpression,
	var,
	logicalOp,
	relationalOp,
	returnStmt,
	optionalReturn,
	idList,
	more_ids
} NT;


// Might be required later
struct sym_value {
    int temp;
};

union symbol_type {
	NT a;
	Terminal b;
};

struct symbol {
    char *id;
    int id_len;
    union symbol_type symbol_name;
    int unique_id;
    int isTerminal;
    struct sym_value v;
    struct symbol* next;
};

typedef struct symbol symbol;

// hash table with separate chaining
struct symbolTable {
    symbol **symbolArray;
    int size;
    int curr_size;
};

typedef struct symbolTable SymbolTable;

#endif
