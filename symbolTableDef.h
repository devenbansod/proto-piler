#ifndef SYMBOL_TABLE_DEF
#define SYMBOL_TABLE_DEF

#include "lexerDef.h"

#define MAX_BUCKETS 337
#define MAX_CHAIN_LENGTH 10


typedef enum {
	program = 201,
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
	more_ids,
	eps = 500
} NonTerminal;

struct symbolType {
	int isTerminal;
	NonTerminal NonT;
	Terminal Term;
};

typedef struct symbolType symbolType;


// Might be required later
struct sym_value {
    int temp;
};

struct symbol {
    char *id;
    int id_len;
    symbolType symbol_name;
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
