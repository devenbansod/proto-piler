/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

/*
 * Common header file with
 * all standard libraries included
 * and the enum 'Symbol' defined at one place.
 *
 * The file is included insdie every file in the project
 *
 */

#ifndef COMMON_HEADER
#define COMMON_HEADER

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"


/*
 * 'Symbol' enum for TK_* and the Non-terminals
 *
 */
typedef enum {
	TK_SQL = 1,
	TK_SQR,
	TK_SEM,
	TK_COLON,
	TK_DOT,
	TK_OP,
	TK_CL,
	TK_PLUS,
	TK_MINUS,
	TK_MUL,
	TK_DIV,
	TK_NOT,
	TK_NE,
	TK_EQ,
	TK_GE,
	TK_GT,
	TK_LE,
	TK_ASSIGNOP,
	TK_LT,
	TK_AND,
	TK_RECORDID,
	TK_OR,
	TK_COMMENT,
	TK_RNUM,
	TK_NUM,
	TK_FUNID,
	TK_ID,
	TK_FIELDID,
	TK_COMMA,
	TK_WITH,
	TK_PARAMETERS,
	TK_END,
	TK_WHILE,
	TK_INT,
	TK_REAL,
	TK_TYPE,
	TK_MAIN,
	TK_GLOBAL,
	TK_PARAMETER,
	TK_LIST,
	TK_INPUT,
	TK_OUTPUT,
	TK_ENDWHILE,
	TK_IF,
	TK_THEN,
	TK_ENDIF,
	TK_READ,
	TK_WRITE,
	TK_RETURN,
	TK_RECORD,
	TK_ENDRECORD,
	TK_ELSE,
	TK_CALL,
	ERROR = -1,
	EOI = 100,

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
	expPrime,
	term,
	termPrime,
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
	some_types,
	other_types,
	for_ll1,
	eps = 500

} Symbol;


#endif

