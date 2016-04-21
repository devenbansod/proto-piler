/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "treeDef.h"
#include "symbolTableDef.h"
#include "symbolTable.h"
#include "AST.h"

SymbolTable *globalST;
TypeTable *globalTT;
FunctionTable *globalFT;

int checkAndReturnType(treeNode* orig, char* type_name) {
	if (orig == NULL || orig->symbol_type == eps) {
		printf("EPS/NULL so returning\n");
		return -1;
	}
	symbolTableElem* looked_up = NULL;

	if (orig->symbol_type == TK_ID
		|| orig->symbol_type == singleOrRecId
	) {
		if (orig->symbol_type == TK_ID) {
			looked_up = lookupSymbol(
				orig->st, orig->tk_info.lexeme,
				strlen(orig->tk_info.lexeme)
			);

			// if not found in local ST, lookup in Global
			if (looked_up == NULL) {
				looked_up = lookupSymbol(
					globalST, orig->tk_info.lexeme,
					strlen(orig->tk_info.lexeme)
				);
			}

			// if still not found, its undeclared
			if (looked_up == NULL) {
				fprintf(
					stderr,
					"*** ERROR: The symbol %s is not declared before it use on %d\n",
					orig->tk_info.lexeme,
					orig->tk_info.line_no
				);
				return -1;
			}

		} else {

			looked_up = lookupSymbol(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			// if not found in local ST, lookup in Global
			if (looked_up == NULL) {
				looked_up = lookupSymbol(
					globalST, orig->children[0]->tk_info.lexeme,
					strlen(orig->children[0]->tk_info.lexeme)
				);
			}

			// if still not found, its undeclared
			if (looked_up == NULL) {
				fprintf(
					stderr,
					"*** ERROR: The symbol %s is not declared before it use on %d\n",
					orig->children[0]->tk_info.lexeme,
					orig->children[0]->tk_info.line_no
				);
				return -1;
			}
		}

		// if Declared, get its type
		if (looked_up->type
			&& orig->curr_children <= 1
		) {
			strcpy(type_name, looked_up->type);
			return strlen(type_name);
		} else {
			// if ID DOT FIELDID
			char type[25];
			memset(type, '\0', 25);
			strcpy(type, looked_up->type);

			typeTableElem* typeT_lookup = lookupType(
				globalTT, type,
				strlen(type)
			);
			// i.e. this is not actually a record
			if (typeT_lookup->fields_count == 0) {
				fprintf(
					stderr,
					"*** ERROR: The variable %s is not a record!\n",
					looked_up->lexeme
				);
				return -1;
			}

			int k = 0;
			for (k = 0; k < typeT_lookup->fields_count; k++) {
				if (strcmp(typeT_lookup->field_names[k], orig->children[1]->tk_info.lexeme) == 0) {

					// reuse the pointer
					typeT_lookup = lookupType(
						globalTT,
						typeT_lookup->field_types[k],
						strlen(typeT_lookup->field_types[k])
					);

					strcpy(type_name, typeT_lookup->type_name);
					return strlen(type_name);
				}
			}

			fprintf(stderr,
				"*** ERROR: The field %s is not declared as part of Record type %s\n",
				orig->children[1]->tk_info.lexeme, typeT_lookup->type_name
			);
			sem_error++;

			return -1;

		}


		return strlen(type_name);
	} else if (orig->symbol_type == TK_NUM) {
		strcpy(type_name, "int");

		return strlen(type_name);
	} else if (orig->symbol_type == TK_RNUM) {
		strcpy(type_name, "real");

		return strlen(type_name);
	}

	else if (orig->symbol_type == idList) {
		// check if 'Lexeme' and Type' Matches
		// as expected in Output Parameter List in
		// Function Defn

	}

	// Arithmetic Expr
	// OR Assignment Statement
	// OR Boolean Expr
	// OR Relational Op
	else if (orig->symbol_type == TK_PLUS
		|| orig->symbol_type == TK_MINUS
		|| orig->symbol_type == TK_MUL
		|| orig->symbol_type == TK_DIV
		|| orig->symbol_type == TK_AND
		|| orig->symbol_type == TK_OR
		|| orig->symbol_type == TK_ASSIGNOP
	) {
		int i = 0;
		char type_name_next[25];
		char type_name_already[25];

		memset(type_name_already, '\0', 25);
		memset(type_name_next, '\0', 25);

		int ret = checkAndReturnType(orig->children[i], type_name_already);

		if (ret == -1)
			return -1;

		int scalar = 0, copy_already = 1;

		// check for scalar multiplications to records
		if (strcmp(type_name_already, "int") != 0
			&& strcmp(type_name_already, "real") != 0
		) {
			scalar = 1;
		}

		for (i = 1; i < orig->curr_children; i++) {
			memset(type_name_next, '\0', 25);

			if (orig->children[i]->symbol_type == eps) {
				return 1;
			}

			if (checkAndReturnType(orig->children[i], type_name_next) == -1)
				return -1;

			// if first operand is a record,
			// and operation is Assign, Add, Sub
			// all others should be of same record type
			if (scalar == 1
				&& (orig->symbol_type == TK_ASSIGNOP
				|| orig->symbol_type == TK_PLUS
				|| orig->symbol_type == TK_MINUS)
			) {
				if (strcmp(type_name_next, type_name_already) != 0) {
					fprintf(
						stderr,
						"*** ERROR: The types of operands do not match on line - %d\n",
						orig->children[i]->tk_info.line_no
					);
					return -1;
				}
			} else if (scalar == 1
				&& (orig->symbol_type == TK_MUL
				|| orig->symbol_type == TK_DIV)
			) {
				if (strcmp(type_name_next, "int") != 0
					&& strcmp(type_name_next, "real") != 0
				) {
					fprintf(
						stderr,
						"*** ERROR:  Invalid operation on line - %d!"
						" Only scalar multiplications to records is allowed\n",
						orig->children[i]->tk_info.line_no
					);
					return -1;
				}
			} else if (scalar == 0 && i == 1
				&& strcmp(type_name_next, "int") != 0
				&& strcmp(type_name_next, "real") != 0
			) {
				// first operand is int or real, but 2nd is record
				// only operation allowed in that case is Multiplication
				if (orig->symbol_type == TK_MUL)
					copy_already = 0;
				else
					fprintf(
						stderr,
						"*** ERROR: The types of operands do not match on line - %d\n",
						orig->children[i]->tk_info.line_no
					);
					i = 1;
			} else {
				if (strcmp(type_name_next, type_name_already) != 0) {
					fprintf(
						stderr,
						"*** ERROR: The types of operands do not match on line - %d\n",
						orig->children[i]->tk_info.line_no
					);
					return -1;
				}
			}

		}

		if (copy_already == 1)
			strcpy(type_name, type_name_already);
		else
			strcpy(type_name, type_name_next);

		return 1;
	} else if (orig->symbol_type == TK_WRITE) {
		int i = 0;
		char type_name_already[25];
		char type_name_next[25];
		memset(type_name_already, '\0', 25);
		memset(type_name_next, '\0', 25);

		int ret = checkAndReturnType(orig->children[0]->children[i], type_name_already);

		typeTableElem* typeT_lookup = lookupType(
			globalTT, type_name_already,
			strlen(type_name_already)
		);

		if (orig->children[0]->curr_children > 1
			&& (strcmp(type_name_already, "real") == 0
			|| strcmp(type_name_already, "int") == 0)
		) {
			fprintf(stderr,
				"*** ERROR: The ID %s on line %d does not have a record type\n",
				orig->children[0]->children[0]->tk_info.lexeme,
				orig->children[0]->children[0]->tk_info.line_no
			);
			sem_error++;
			return -1;
		} else if (orig->children[0]->curr_children > 1
			&& (strcmp(type_name_already, "real") != 0
			&& strcmp(type_name_already, "int") != 0)
		) {
			// is a valid record. need to confirm if field exists
			int k = 0;
			for (k = 0; k < typeT_lookup->fields_count; k++) {
				if (
					strcmp(typeT_lookup->field_names[k],
						orig->children[0]->children[1]->tk_info.lexeme) == 0
				) {

					// reuse the pointer
					typeT_lookup = lookupType(
						globalTT,
						typeT_lookup->field_types[k],
						strlen(typeT_lookup->field_types[k])
					);

					strcpy(type_name, typeT_lookup->type_name);
					return strlen(type_name);
				}
			}

			fprintf(stderr,
				"*** ERROR: The field %s is not declared as part of Record type %s\n",
				orig->children[0]->children[1]->tk_info.lexeme, orig->children[0]->children[0]->tk_info.lexeme
			);
			sem_error++;
			return -1;
		} else if (orig->children[0]->curr_children <= 1
			&& (strcmp(type_name_already, "real") != 0
			&& strcmp(type_name_already, "int") != 0)
		) {
			fprintf(stderr,
				"*** ERROR: The ID %s on line %d has a record type and can not be directly printed\n",
				orig->children[0]->children[0]->tk_info.lexeme,
				orig->children[0]->children[0]->tk_info.line_no
			);
			sem_error++;
			return -1;
		} else {
			// do nothing
		}

		strcpy(type_name, type_name_already);
		return strlen(type_name);

	} else if (orig->symbol_type == TK_READ) {
		int i = 0;
		char type_name_already[25];
		char type_name_next[25];
		memset(type_name_already, '\0', 25);
		memset(type_name_next, '\0', 25);
		int ret = 0;

		typeTableElem* typeT_lookup = NULL;

		// directly ID
		if (orig->children[0]->curr_children == 0) {
			ret = checkAndReturnType(orig->children[0], type_name_already);

			if (orig->children[0]->curr_children > 1
				&& (strcmp(type_name_already, "real") == 0
				|| strcmp(type_name_already, "int") == 0)
			) {
				fprintf(stderr,
					"*** ERROR: The ID %s on line %d does not have a record type\n",
					orig->children[0]->tk_info.lexeme,
					orig->children[0]->tk_info.line_no
				);
				sem_error++;
				return -1;
			} else if (orig->children[0]->curr_children <= 1
				&& (strcmp(type_name_already, "real") != 0
				&& strcmp(type_name_already, "int") != 0)
			) {
				fprintf(stderr,
					"*** ERROR: The ID %s on line %d has a record type and can not be directly read\n",
					orig->children[0]->tk_info.lexeme,
					orig->children[0]->tk_info.line_no
				);
				sem_error++;
				return -1;
			}
		} else {
			// if singleOrRecId

			ret = checkAndReturnType(orig->children[0]->children[i], type_name_already);

			if (orig->children[0]->curr_children > 1
				&& (strcmp(type_name_already, "real") == 0
				|| strcmp(type_name_already, "int") == 0)
			) {
				fprintf(stderr,
					"*** ERROR: The ID %s on line %d does not have a record type\n",
					orig->children[0]->children[0]->tk_info.lexeme,
					orig->children[0]->children[0]->tk_info.line_no
				);
				sem_error++;
				return -1;
			} else if (orig->children[0]->curr_children <= 1
				&& (strcmp(type_name_already, "real") != 0
				&& strcmp(type_name_already, "int") != 0)
			) {
				fprintf(stderr,
					"*** ERROR: The ID %s on line %d has a record type and can not be directly read\n",
					orig->children[0]->children[0]->tk_info.lexeme,
					orig->children[0]->children[0]->tk_info.line_no
				);
				sem_error++;
				return -1;
			} else {

				typeT_lookup = lookupType(
					globalTT, type_name_already,
					strlen(type_name_already)
				);
				// is a valid record. need to confirm if field exists
				int k = 0;
				for (k = 0; k < typeT_lookup->fields_count; k++) {
					if (
						strcmp(typeT_lookup->field_names[k],
							orig->children[0]->children[1]->tk_info.lexeme) == 0
					) {

						// reuse the pointer
						typeT_lookup = lookupType(
							globalTT,
							typeT_lookup->field_types[k],
							strlen(typeT_lookup->field_types[k])
						);

						strcpy(type_name, typeT_lookup->type_name);
						return strlen(type_name);
					}
				}

				fprintf(stderr,
					"*** ERROR: The field %s is not declared as part of Record type %s\n",
					orig->children[0]->children[1]->tk_info.lexeme, typeT_lookup->type_name
				);
				sem_error++;
				return -1;
			}
			return ret;
		}

	} else if (orig->symbol_type == TK_NOT
		|| orig->symbol_type == TK_NE
		|| orig->symbol_type == TK_GE
		|| orig->symbol_type == TK_EQ
		|| orig->symbol_type == TK_LE
		|| orig->symbol_type == TK_LT
	) {
		int i = 0;
		char type_name_next[25];
		char type_name_already[25];

		memset(type_name_already, '\0', 25);
		memset(type_name_next, '\0', 25);

		int ret = checkAndReturnType(orig->children[i], type_name_already);

		if (ret == -1)
			return -1;

		if (strcmp(type_name_already, "boolean") == 0) {
			for (i = 1; i < orig->curr_children; i++) {
				memset(type_name_next, '\0', 25);

				if (orig->children[i]->symbol_type == eps) {
					continue;
				}

				if (checkAndReturnType(orig->children[i], type_name_next) == -1)
					return -1;

				if (strcmp(type_name_already, type_name_next) != 0) {
					fprintf(
						stderr,
						"*** ERROR: Type sem_error in Boolean expression on line 1 - %d\n",
						orig->tk_info.line_no
					);
					return -1;
				}
			}
		} else if (strcmp(type_name_already, "int") == 0) {
			for (i = 1; i < orig->curr_children; i++) {
				memset(type_name_next, '\0', 25);

				if (orig->children[i]->symbol_type == eps) {
					continue;
				}

				if (checkAndReturnType(orig->children[i], type_name_next) == -1)
					return -1;

				if (strcmp(type_name_already, type_name_next) != 0) {
					fprintf(
						stderr,
						"*** ERROR: Type sem_error in Boolean expression on line 2 - %d\n",
						orig->tk_info.line_no
					);
					return -1;
				}
			}
		} else if (strcmp(type_name_already, "real") == 0) {
			for (i = 1; i < orig->curr_children; i++) {
				memset(type_name_next, '\0', 25);

				if (orig->children[i]->symbol_type == eps) {
					continue;
				}

				if (checkAndReturnType(orig->children[i], type_name_next) == -1)
					return -1;

				if (strcmp(type_name_already, type_name_next) != 0) {
					fprintf(
						stderr,
						"*** ERROR: Type sem_error in Boolean expression on line 3 - %d\n",
						orig->tk_info.line_no
					);
					return -1;
				}
			}
		} else {
			fprintf(stderr,
				"*** ERROR: Boolean expression must have only boolean, int or real"
				" data types! Records are not allowed!\n"
			);
			sem_error++;
		}

		strcpy(type_name, "boolean");
		return 1;

	}

	return 1;
}

int performTypeChecking (treeNode* orig) {
	char type_name[25];

	if (orig == NULL
		|| orig->symbol_type == eps
	) {
		return 0;
	}

	// for assignmentStmt
	if (orig->symbol_type == TK_ASSIGNOP
		|| orig->symbol_type == TK_PLUS
		|| orig->symbol_type == TK_MINUS
		|| orig->symbol_type == TK_MUL
		|| orig->symbol_type == TK_DIV
		|| orig->symbol_type == TK_WRITE
		|| orig->symbol_type == TK_READ
	) {
		memset(type_name, '\0', 25);
		if (checkAndReturnType(orig, type_name)) {
			return 0;
		} else {
			fprintf(
				stderr,
				"*** ERROR: The types of operands do not match on line - %d\n",
				orig->tk_info.line_no
			);

			return 0;
		}
	}

	// for iterativeStmt/Boolean Expr
	else if (orig->symbol_type == TK_LE
		|| orig->symbol_type == TK_LT
		|| orig->symbol_type == TK_GE
		|| orig->symbol_type == TK_GT
		|| orig->symbol_type == TK_EQ
	) {
		memset(type_name, '\0', 25);
		if (checkAndReturnType(orig, type_name)) {
			return 0;
		} else {
			fprintf(
				stderr,
				"*** ERROR: The types of operands do not match on line - %d\n",
				orig->tk_info.line_no
			);

			return 0;
		}
	}

	else {

		int i = 0;
		for(i = 0; i < orig->curr_children; i++) {
			if (performTypeChecking(orig->children[i]) == -1) {
				fprintf(stderr, "*** ERROR: Type checking failed here %d\n", orig->children[i]->symbol_type);
			}
		}
	}

	return 0;
}
