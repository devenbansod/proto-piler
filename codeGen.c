/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "codeGen.h"
#include "AST.h"

int curr_label = 0;

void generateNewLabel(char* new_label) {
	strcpy(new_label, "L");
	char temp[10];
	sprintf(temp, "%d", curr_label);
	strcat(new_label, temp);
	curr_label++;
}


symbolTableElem* lookupSymbolHelper(SymbolTable* st, char *id, int id_len) {
	symbolTableElem* looked_up = lookupSymbol(st, id, id_len);

	if (looked_up == NULL) {
		looked_up = lookupSymbol(globalST, id, id_len);
	}

	return looked_up;
}


/*
 * Write start up things
 */
int generateProgram(treeNode* orig, FILE *fp) {
	fprintf(fp, "SECTION\t.data\n\n");
	fprintf(fp, "\tformatin db \"%%d\", 0\n");
    fprintf(fp, "\tformatout db \"%%d\", 10, 0 ; newline, nul terminator\n");
	fprintf(fp, "datastart:\n");

    fprintf(fp, "SECTION .bss\n");
    fprintf(fp, "bufferstart: resb %d\n", curr_global_offset_backup * 2);

	fprintf(fp, "SECTION\t.text\n\n");
	fprintf(fp, "extern\tprintf\n");
	fprintf(fp, "extern\tscanf\n");
	fprintf(fp, "global\tmain\n");
	fprintf(fp, "main:\n");
	fprintf(fp, "\tMOV EBP, bufferstart\n\n");

	// move to StmtsNode
	orig = orig->children[1];

	// move to otherStmtsNode
	orig = orig->children[2];


	// now run a loop over all statements
	int i = 0;
	for (i = 0; i < orig->curr_children; i++) {
		switch (orig->children[i]->symbol_type) {
			case TK_ASSIGNOP:
				generateAssign(orig->children[i], fp);
				fprintf(fp, "\n");
				break;

			case conditionalStmt:
				generateConditional(orig->children[i], fp);
				fprintf(fp, "\n");
				break;

			case iterativeStmt:
				generateIterative(orig->children[i], fp);
				fprintf(fp, "\n");
				break;

			case TK_WRITE:
			case TK_READ:
				generateIO(orig->children[i], fp);
				fprintf(fp, "\n");
				break;

			default:
				continue;
		}
	}

	fprintf(fp, "\tret\n");

	curr_label = 0;
	return 0;
}


/*
 * Write 'db' things may be
 */
int generateDeclarations(treeNode* orig, FILE *fp) {

	return 0;
}

int generateStmt(treeNode* orig, FILE *fp) {
	switch (orig->symbol_type) {
		case TK_ASSIGNOP:
			generateAssign(orig, fp);
			fprintf(fp, "\n");
			break;

		case conditionalStmt:
			generateConditional(orig, fp);
			fprintf(fp, "\n");
			break;

		case iterativeStmt:
			generateIterative(orig, fp);
			fprintf(fp, "\n");
			break;

		case TK_WRITE:
		case TK_READ:
			generateIO(orig, fp);
			fprintf(fp, "\n");
			break;

		default:
			return -1;
	}

	return 0;
}

int generateRecordExpr(treeNode* orig, FILE *fp, int field_no, int reg) {
	if (orig->symbol_type == TK_PLUS) {
		if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_ID
		) {
			char *reg_str = (char*)malloc(sizeof(char) * 3);
			getRegFromInt(reg, reg_str);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			fprintf(fp, "\tMOV %s, [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);
			fprintf(fp, "\tADD %s, [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			return reg;

		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type != TK_ID
		) {
			char *reg_str = (char*)malloc(sizeof(char) * 3);
			char *reg_str_temp = (char*)malloc(sizeof(char) * 3);
			getRegFromInt(reg, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			generateRecordExpr(orig->children[1], fp, field_no, (reg+1)%4);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			fprintf(fp, "\tMOV %s, [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			fprintf(fp, "\tADD %s, %s\n",
				reg_str,
				reg_str_temp
			);

			return reg;

		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type == TK_ID
		) {
			char *reg_str = (char*)malloc(sizeof(char) * 3);
			char *reg_str_temp = (char*)malloc(sizeof(char) * 3);
			getRegFromInt(reg, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			generateRecordExpr(orig->children[0], fp, field_no, reg);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			fprintf(fp, "\tMOV %s, [EBP + %dd]\n",
				reg_str_temp,
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			fprintf(fp, "\tADD %s, %s\n",
				reg_str,
				reg_str_temp
			);

			return reg;

		} else {
			// both children are Expr
			char *reg_str = (char*)malloc(sizeof(char) * 3);
			char *reg_str_temp = (char*)malloc(sizeof(char) * 3);
			getRegFromInt(reg, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			generateRecordExpr(orig->children[1], fp, field_no, (reg+1)%4);
			fprintf(fp, "\tPUSH %s\n", reg_str_temp);

			generateRecordExpr(orig->children[0], fp, field_no, reg);
			fprintf(fp, "\tPOP %s\n", reg_str_temp);

			fprintf(fp, "\tADD %s, %s\n", reg_str, reg_str_temp);

			return reg;
		}
	} else if (orig->symbol_type == TK_MINUS) {
		if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_ID
		) {
			char *reg_str = (char*)malloc(sizeof(char) * 3);
			getRegFromInt(reg, reg_str);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			fprintf(fp, "\tMOV %s, [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);
			fprintf(fp, "\tSUB %s, [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			return reg;

		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type != TK_ID
		) {
			char *reg_str = (char*)malloc(sizeof(char) * 3);
			char *reg_str_temp = (char*)malloc(sizeof(char) * 3);
			getRegFromInt(reg, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			generateRecordExpr(orig->children[1], fp, field_no, (reg+1)%4);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			fprintf(fp, "\tMOV %s, [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			fprintf(fp, "\tSUB %s, %s\n",
				reg_str,
				reg_str_temp
			);

			return reg;

		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type == TK_ID
		) {
			char *reg_str = (char*)malloc(sizeof(char) * 3);
			char *reg_str_temp = (char*)malloc(sizeof(char) * 3);
			getRegFromInt(reg, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			generateRecordExpr(orig->children[0], fp, field_no, reg);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			fprintf(fp, "\tMOV %s, [EBP + %dd]\n",
				reg_str_temp,
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			fprintf(fp, "\tSUB %s, %s\n",
				reg_str,
				reg_str_temp
			);

			return reg;

		} else {
			// both children are Expr
			char *reg_str = (char*)malloc(sizeof(char) * 3);
			char *reg_str_temp = (char*)malloc(sizeof(char) * 3);
			getRegFromInt(reg, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			generateRecordExpr(orig->children[1], fp, field_no, (reg+1)%4);
			fprintf(fp, "\tPUSH %s\n", reg_str_temp);

			generateRecordExpr(orig->children[0], fp, field_no, reg);
			fprintf(fp, "\tPOP %s\n", reg_str_temp);

			fprintf(fp, "\tSUB %s, %s\n", reg_str, reg_str_temp);

			return reg;
		}
	} else if (orig->symbol_type == TK_MUL) {
		if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[0]->symbol_type == TK_ID
		) {
			int record_child = 0;
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			if (strcmp(looked_up->type, "int") != 0
				&& strcmp(looked_up->type, "real") != 0
			) {
				record_child = 0;
			} else {
				looked_up = lookupSymbolHelper(
					orig->children[0]->st, orig->children[0]->tk_info.lexeme,
					strlen(orig->children[0]->tk_info.lexeme)
				);
				record_child = 1;
			}

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			symbolTableElem* non_rec = lookupSymbolHelper(
				orig->children[(record_child + 1) % 2]->st,
				orig->children[(record_child + 1) % 2]->tk_info.lexeme,
				strlen(orig->children[(record_child + 1) % 2]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV EAX, [EBP + %dd]\n",
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			fprintf(fp, "\tMUL dword [EBP + %dd]\n",
				BASE_ADDR + non_rec->offset
			);

			if (reg != 1) {
				char *reg_str = (char*)malloc(sizeof(char) * 3);
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			fprintf(fp, "\tMOV EAX, [EBP + %dd]\n",
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			fprintf(fp, "\tMUL %sd\n",
				orig->children[1]->tk_info.lexeme
			);

			if (reg != 1) {
				char *reg_str = (char*)malloc(sizeof(char) * 3);
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		}

	} else if (orig->symbol_type == TK_DIV) {
		if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[0]->symbol_type == TK_ID
		) {
			int record_child = 0;
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			if (strcmp(looked_up->type, "int") != 0
				&& strcmp(looked_up->type, "real") != 0
			) {
				record_child = 0;
			} else {
				looked_up = lookupSymbolHelper(
					orig->children[0]->st, orig->children[0]->tk_info.lexeme,
					strlen(orig->children[0]->tk_info.lexeme)
				);
				record_child = 1;
			}

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			symbolTableElem* non_rec = lookupSymbolHelper(
				orig->children[(record_child + 1) % 2]->st,
				orig->children[(record_child + 1) % 2]->tk_info.lexeme,
				strlen(orig->children[(record_child + 1) % 2]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV EAX, [EBP + %dd]\n",
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			fprintf(fp, "\tDIV [EBP + %dd]\n",
				BASE_ADDR + non_rec->offset
			);

			if (reg != 1) {
				char *reg_str = (char*)malloc(sizeof(char) * 3);
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);

			fprintf(fp, "\tMOV EAX, [EBP + %dd]\n",
				BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
			);

			fprintf(fp, "\tMUL %sd\n",
				orig->children[1]->tk_info.lexeme
			);

			if (reg != 1) {
				char *reg_str = (char*)malloc(sizeof(char) * 3);
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		}
	} else if (orig->symbol_type == TK_ID) {
		char *reg_str = (char*)malloc(sizeof(char) * 3);
		getRegFromInt(reg, reg_str);

		symbolTableElem* looked_up = lookupSymbolHelper(
			orig->children[0]->st, orig->children[0]->tk_info.lexeme,
			strlen(orig->children[0]->tk_info.lexeme)
		);

		typeTableElem* looked_up_type = lookupType(
			globalTT, looked_up->type, strlen(looked_up->type)
		);

		fprintf(fp, "\tMOV %s, [EBP + %dd]\n",
			reg_str,
			BASE_ADDR + looked_up->offset + looked_up_type->offset[field_no]
		);

		return reg;
	}

	return reg;
}


/*
 * Specially for operations on record types
 */
int generateAssignRecords(treeNode* orig, FILE *fp) {
	// for every field, perform the operation
	symbolTableElem* looked_up = lookupSymbolHelper(
		orig->st, orig->children[0]->tk_info.lexeme,
		strlen(orig->children[0]->tk_info.lexeme)
	);

	typeTableElem* looked_up_type = lookupType(
		globalTT, looked_up->type, strlen(looked_up->type)
	);
	int i = 0, offset = 0;

	for (i = 0; i < looked_up_type->fields_count; i++) {
		offset = looked_up_type->offset[i];
		generateRecordExpr(orig->children[1], fp, i, 1);
		char *reg_str = (char*)malloc(sizeof(char) * 3);
		getRegFromInt(1, reg_str);

		fprintf(fp, "\tMOV dword [EBP + %dd], %s\n",
			BASE_ADDR + looked_up->offset + offset,
			reg_str
		);
	}

	return 0;
}

/*
 * First, evaluate the 2nd child (recursively)
 * Then, append to it,
 * assign instruction with Dst arg as 1st child
 * and Src arg as temporary generated from 2nd child' eval
 */
int generateAssign(treeNode* orig, FILE *fp) {

	int offset = 0;
	symbolTableElem* looked_up = NULL;

	if (orig->children[0]->symbol_type == singleOrRecId) {
		looked_up = lookupSymbolHelper(
			orig->st, orig->children[0]->children[0]->tk_info.lexeme,
			strlen(orig->children[0]->children[0]->tk_info.lexeme)
		);
		typeTableElem* looked_up_type = lookupType(
			globalTT, looked_up->type, strlen(looked_up->type)
		);
		int i = 0;

		for (i = 0; i < looked_up_type->fields_count; i++) {
			if (strcmp(
				looked_up_type->field_names[i], orig->children[0]->children[1]->tk_info.lexeme)
				== 0
			) {
				offset = looked_up_type->offset[i];
				break;
			}
		}
	} else {
		looked_up = lookupSymbolHelper(
			orig->st, orig->children[0]->tk_info.lexeme,
			strlen(orig->children[0]->tk_info.lexeme)
		);

		// operations on Record types
		if (strcmp(looked_up->type, "int") != 0
			&& strcmp(looked_up->type, "real") != 0
		) {
			generateAssignRecords(orig, fp);
			return 0;
		}
	}

	if (orig->children[1]->symbol_type == TK_NUM) {
		// the real assign statement
		fprintf(fp,
			"\tMOV dword [EBP + %dd], %s\n",
			BASE_ADDR + looked_up->offset + offset,
			orig->children[1]->tk_info.lexeme
		);
	} else {
		// since assign will always be from a expr to memory
		// we always generate MOV 1stChild, EAX i.e. reg = 1 as argument
		generateArithmeticExpr(orig->children[1], fp, 1);

		// the real assign statement
		fprintf(fp, "\tMOV dword [EBP + %dd], EAX\n", BASE_ADDR + looked_up->offset + offset);
	}

	return 0;
}

/*
 *
 */
int generateIterative(treeNode* orig, FILE *fp) {

	char* loop_label = (char*)malloc(sizeof(char) * 5);
	char* end_label = (char*)malloc(sizeof(char) * 5);
	char* reg_str = (char*)malloc(sizeof(char) * 5);
	generateNewLabel(loop_label);
	generateNewLabel(end_label);
	getRegFromInt(1, reg_str);

	fprintf(fp, "%s:\n", loop_label);
	generateBooleanExpr(orig->children[0], fp, 1);

	fprintf(fp, "\tCMP %s, 1\n", reg_str);
	fprintf(fp, "\tJNZ %s\n", end_label);

	// first handle first statement inside Then
	generateStmt(orig->children[1], fp);
	// loop over otherStmts Node if it is not null
	if (orig->children[2]) {
		int i;
		for (i = 0; i < orig->children[2]->curr_children; i++) {
			generateStmt(orig->children[2]->children[i], fp);
		}
	}
	fprintf(fp, "\tJMP %s\n", loop_label);

	fprintf(fp, "%s:\n", end_label);

	return 0;
}


/*
 *
 */
int
generateBooleanExpr(treeNode* orig, FILE *fp, int reg) {
	if (orig->symbol_type == TK_AND) {
		// condition1 && condition2
		char* reg_str = (char*)malloc(sizeof(char) * 3);
		char* reg_str_temp = (char*)malloc(sizeof(char) * 3);

		int ret1 = generateBooleanExpr(orig->children[0], fp, reg);
		getRegFromInt(ret1, reg_str);
		fprintf(fp, "\tPUSH %s\n", reg_str);

		int ret2 = generateBooleanExpr(orig->children[1], fp, reg);
		getRegFromInt(ret2, reg_str_temp);

		getRegFromInt((ret2 + 1) % 4, reg_str);
		fprintf(fp, "\tPOP %s\n", reg_str);

		fprintf(fp, "\tAND %s, %s\n", reg_str, reg_str_temp);

		if ((ret2 + 1) % 4 != reg) {
			getRegFromInt(reg, reg_str_temp);
			fprintf(fp, "\tMOV %s, %s\n", reg_str_temp, reg_str);
		}

		return reg;

	} else if (orig->symbol_type == TK_OR) {
		// condition1 || condition2
		char* reg_str = (char*)malloc(sizeof(char) * 3);
		char* reg_str_temp = (char*)malloc(sizeof(char) * 3);

		int ret1 = generateBooleanExpr(orig->children[0], fp, reg);
		getRegFromInt(ret1, reg_str);
		fprintf(fp, "\tPUSH %s\n", reg_str);

		int ret2 = generateBooleanExpr(orig->children[1], fp, reg);
		getRegFromInt(ret2, reg_str_temp);

		getRegFromInt((ret2 + 1) % 4, reg_str);
		fprintf(fp, "\tPOP %s\n", reg_str);

		fprintf(fp, "\tOR %s, %s\n", reg_str, reg_str_temp);

		if ((ret2 + 1) % 4 != reg) {
			getRegFromInt(reg, reg_str_temp);
			fprintf(fp, "\tMOV %s, %s\n", reg_str_temp, reg_str);
		}

		return reg;
	} else {
		// relational operator
		return generateRelational(orig, fp, reg);
	}
}


/*
 *
 */
int generateConditional(treeNode* orig, FILE *fp) {

	// boolean expr
	int ret = generateBooleanExpr(orig->children[0], fp, 1);

	char* true_label = (char*)malloc(sizeof(char) * 5);
	char* false_label = (char*)malloc(sizeof(char) * 5);
	char* end_label = (char*)malloc(sizeof(char) * 5);

	generateNewLabel(true_label);
	generateNewLabel(false_label);
	generateNewLabel(end_label);

	char* reg_str = (char*)malloc(sizeof(char) * 3);
	getRegFromInt(ret, reg_str);

	fprintf(fp, "\tCMP %s, 1\n", reg_str);
	fprintf(fp, "\tJNZ %s\n", false_label);
	// generate for THEN Statements
	fprintf(fp, "%s:\n", true_label);

	// first handle first statement inside Then
	generateStmt(orig->children[1], fp);
	// loop over otherStmts Node if it is not null
	if (orig->children[2]) {
		int i;
		for (i = 0; i < orig->children[2]->curr_children; i++) {
			generateStmt(orig->children[2]->children[i], fp);
		}
	}
	fprintf(fp, "\tJMP %s\n", end_label);

	// generate for ELSE statements
	fprintf(fp, "%s:\n", false_label);

	// check if TK_ELSE Node exists
	if (orig->curr_children > 3
		&& orig->children[3]->symbol_type == TK_ELSE
	) {
		// first handle first statement inside Then
		generateStmt(orig->children[4], fp);
		// loop over otherStmts Node if it is not null
		if (orig->children[5]) {
			int i;
			for (i = 0; i < orig->children[5]->curr_children; i++) {
				generateStmt(orig->children[5]->children[i], fp);
			}
		}
	}

	fprintf(fp, "%s:\n", end_label);
	return 0;
}

/*
 *
 */
int generateRelational(treeNode* orig, FILE *fp, int reg) {

	char* true_label = (char*)malloc(sizeof(char) * 5);
	char* false_label = (char*)malloc(sizeof(char) * 5);
	generateNewLabel(true_label);
	generateNewLabel(false_label);

	char* reg_str = (char*)malloc(sizeof(char) * 3);
	getRegFromInt((reg + 1) % 4, reg_str);
	fprintf(fp, "\tMOV %s, 1\n", reg_str);

	if (orig->children[0]->symbol_type == TK_NUM
		&& orig->children[1]->symbol_type == TK_NUM
	) {
		getRegFromInt(1, reg_str);
		fprintf(fp, "\tMOV %s, %sd\n", reg_str, orig->children[0]->tk_info.lexeme);

		fprintf(fp, "\tCMP %s, %s\n",
			reg_str,
			orig->children[1]->tk_info.lexeme
		);
	} else if (orig->children[0]->symbol_type == TK_NUM
		&& orig->children[1]->symbol_type == TK_ID
	) {
		symbolTableElem* looked_up = lookupSymbolHelper(
			orig->st,
			orig->children[1]->tk_info.lexeme,
			strlen(orig->children[1]->tk_info.lexeme)
		);

		fprintf(fp, "\tCMP %s,  [EBP + %dd]\n",
			orig->children[0]->tk_info.lexeme,
			BASE_ADDR + looked_up->offset
		);
	} else if (orig->children[0]->symbol_type == TK_ID
		&& orig->children[1]->symbol_type == TK_NUM
	) {
		symbolTableElem* looked_up = lookupSymbolHelper(
			orig->st,
			orig->children[0]->tk_info.lexeme,
			strlen(orig->children[0]->tk_info.lexeme)
		);
		char* reg_str = (char*)malloc(sizeof(char) * 3);
		getRegFromInt(1, reg_str);

		fprintf(fp, "\tMOV %s,  [EBP + %dd]\n",
			reg_str,
			BASE_ADDR + looked_up->offset
		);
		fprintf(fp, "\tCMP %s, %s\n",
			reg_str,
			orig->children[1]->tk_info.lexeme
		);
	} else {
		symbolTableElem* looked_up = lookupSymbolHelper(
			orig->st,
			orig->children[0]->tk_info.lexeme,
			strlen(orig->children[0]->tk_info.lexeme)
		);
		char* reg_str = (char*)malloc(sizeof(char) * 3);
		getRegFromInt(1, reg_str);

		fprintf(fp, "\tMOV %s,  [EBP + %dd]\n",
			reg_str,
			BASE_ADDR + looked_up->offset
		);

		looked_up = lookupSymbolHelper(
			orig->st,
			orig->children[1]->tk_info.lexeme,
			strlen(orig->children[1]->tk_info.lexeme)
		);

		fprintf(fp, "\tCMP %s,  [EBP + %dd]\n",
			reg_str,
			BASE_ADDR + looked_up->offset
		);
	}

	if (orig->symbol_type == TK_LT) {
		fprintf(fp, "\tJL %s\n", true_label);
	} else if (orig->symbol_type == TK_LE) {
		fprintf(fp, "\tJLE %s\n", true_label);
	} else if (orig->symbol_type == TK_GT) {
		fprintf(fp, "\tJG %s\n", true_label);
	} else if (orig->symbol_type == TK_GE) {
		fprintf(fp, "\tJGE %s\n", true_label);
	} else if (orig->symbol_type == TK_EQ) {
		fprintf(fp, "\tJE %s\n", true_label);
	} else {
		fprintf(fp, "\tJNE %s\n", true_label);
	}

	fprintf(fp, "%s:\n", false_label);
	getRegFromInt((reg + 1) % 4, reg_str);

	char *reg_str_temp = (char*)malloc(sizeof(char) * 3);
	getRegFromInt(reg, reg_str_temp);

	fprintf(fp, "\tMOV %s, 0\n", reg_str);

	fprintf(fp, "%s:\n", true_label);
	fprintf(fp, "\tMOV %s, %s\n", reg_str_temp, reg_str);

	return reg;
}

/*
 *
 */
int generateIO(treeNode* orig, FILE *fp) {

	fprintf(fp,
		"\tPUSH EAX\n"
		"\tPUSH EBX\n"
		"\tPUSH ECX\n"
		"\tPUSH EDX\n"
	);

	if (orig->symbol_type == TK_WRITE) {
		// get value from offset into reg
		// push it
		// change EBP value from bufferstart to datastart
		// push address of formatout

		int offset = 0;
		symbolTableElem* looked_up = NULL;

		if (orig->children[0]->symbol_type == allVar) {
			looked_up = lookupSymbolHelper(
				orig->st, orig->children[0]->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->children[0]->tk_info.lexeme)
			);

			if (strcmp(looked_up->type, "int") != 0
				&& strcmp(looked_up->type, "real") != 0
			) {
				// print all fields of record
				typeTableElem* looked_up_type = lookupType(
					globalTT, looked_up->type, strlen(looked_up->type)
				);
				int i = 0;

				for (i = 0; i < looked_up_type->fields_count; i++) {
					offset = looked_up_type->offset[i];
					fprintf(fp, "\tMOV EAX, [EBP + %dd]\n", BASE_ADDR + looked_up->offset + offset);

					fprintf(fp,
					  	"\tpush EAX\n"
					    "\tpush formatout\n"
					    "\tcall printf\n"
					    "\tadd esp, 8 ; remove parameters\n"
				    );
				}

				fprintf(fp,
					"\tPOP EAX\n"
					"\tPOP EBX\n"
					"\tPOP ECX\n"
					"\tPOP EDX\n"
				);


				return 0;
			} else {

				typeTableElem* looked_up_type = lookupType(
					globalTT, looked_up->type, strlen(looked_up->type)
				);
				int i = 0;

				for (i = 0; i < looked_up_type->fields_count; i++) {
					if (strcmp(
						looked_up_type->field_names[i], orig->children[0]->children[1]->tk_info.lexeme)
						== 0
					) {
						offset = looked_up_type->offset[i];
						break;
					}
				}
			}
		} else {
			looked_up = lookupSymbolHelper(
				orig->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);
		}

		fprintf(fp, "\tMOV EAX, [EBP + %dd]\n", BASE_ADDR + looked_up->offset + offset);

		fprintf(fp,
		  	"\tpush EAX\n"
		    "\tpush formatout\n"
		    "\tcall printf\n"
		    "\tadd esp, 8 ; remove parameters\n"
	    );
	} else {
		int offset = 0;
		symbolTableElem* looked_up = NULL;

		if (orig->children[0]->symbol_type == singleOrRecId) {
			looked_up = lookupSymbolHelper(
				orig->st, orig->children[0]->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->children[0]->tk_info.lexeme)
			);
			typeTableElem* looked_up_type = lookupType(
				globalTT, looked_up->type, strlen(looked_up->type)
			);
			int i = 0;

			for (i = 0; i < looked_up_type->fields_count; i++) {
				if (strcmp(
					looked_up_type->field_names[i], orig->children[0]->children[1]->tk_info.lexeme)
					== 0
				) {
					offset = looked_up_type->offset[i];
					break;
				}
			}
		} else {
			looked_up = lookupSymbolHelper(
				orig->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);
		}

		fprintf(fp, "\tMOV EAX, EBP\n");
		fprintf(fp, "\tADD EAX, %dd\n", BASE_ADDR + looked_up->offset + offset);

		fprintf(fp,
		  	"\tpush EAX\n"
		    "\tpush formatin\n"
		    "\tcall scanf\n"
		    "\tadd esp, 8 ; remove parameters\n"
	    );
	}

	fprintf(fp,
		"\tPOP EAX\n"
		"\tPOP EBX\n"
		"\tPOP ECX\n"
		"\tPOP EDX\n"
	);


	return 0;
}

int generateArithmeticExpr(treeNode *orig, FILE* fp, int reg) {

	if (orig->symbol_type == TK_PLUS) {
		if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// ID OP ID
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV %s,  [EBP + %dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			fprintf(fp, "\tADD %s,  [EBP + %dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// ID OP NUM
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp,
				"\tMOV %s,  [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset
			);
			fprintf(
				fp, "\tADD %s, %sd\n",
				reg_str,
				orig->children[1]->tk_info.lexeme
			);

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// i.e. the second child is Expr
			// ID OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV %s,  [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tADD %s, %s\n",
				reg_str, reg_str_temp
			);

			return reg;

		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// NUM OP NUM
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tMOV %s, %sd\n", reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tADD %s, %sd\n", reg_str,
				orig->children[1]->tk_info.lexeme
			);

			return reg;
		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// NUM OP ID
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tMOV %s, %sd\n",
				reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tADD %s,  [EBP + %dd]\n", reg_str,
				BASE_ADDR + looked_up->offset
			);

			return reg;
		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// NUM OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV %s, %sd\n",
				reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tADD %s, %s\n",
				reg_str, reg_str_temp
			);

			return reg;

		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// EXPR OP NUM
			int ret = generateArithmeticExpr(orig->children[0], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV %s, %sd\n",
				reg_str,
				orig->children[1]->tk_info.lexeme
			);

			fprintf(fp, "\tADD %s, %s\n",
				reg_str, reg_str_temp
			);

			return reg;
		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// EXPR OP ID
			int ret = generateArithmeticExpr(orig->children[1], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV %s,  [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tADD %s, %s\n",
				reg_str, reg_str_temp
			);

			return reg;
		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// EXPR OP EXPR
			generateArithmeticExpr(orig->children[1], fp, reg);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);
			fprintf(fp, "\tPUSH %s\n", reg_str);

			generateArithmeticExpr(orig->children[0], fp, reg);
			getRegFromInt(reg, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			fprintf(fp, "\tPOP %s\n", reg_str_temp);
			fprintf(fp, "\tADD %s, %s\n", reg_str, reg_str_temp);

			return reg;
		}

	} else if (orig->symbol_type == TK_MINUS) {
		if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// ID OP ID
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV %s,  [EBP + %dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			fprintf(fp, "\tSUB %s,  [EBP + %dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// ID OP NUM
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp,
				"\tMOV %s,  [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset
			);
			fprintf(
				fp, "\tSUB %s, %sd\n",
				reg_str,
				orig->children[1]->tk_info.lexeme
			);

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// ID OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV %s,  [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tSUB %s, %s\n",
				reg_str, reg_str_temp
			);

			return reg;

		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// NUM OP NUM
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tMOV %s, %sd\n", reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tSUB %s, %sd\n", reg_str,
				orig->children[1]->tk_info.lexeme
			);

			return reg;
		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// NUM OP ID
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tMOV %s, %sd\n",
				reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tSUB %s,  [EBP + %dd]\n", reg_str,
				BASE_ADDR + looked_up->offset
			);

			return reg;
		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// NUM OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV %s, %sd\n",
				reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tSUB %s, %s\n",
				reg_str, reg_str_temp
			);

			return reg;

		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// EXPR OP NUM
			int ret = generateArithmeticExpr(orig->children[0], fp, reg);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(ret, reg_str);

			fprintf(fp, "\tSUB %s, %sd\n",
				reg_str, orig->children[1]->tk_info.lexeme
			);

			return reg;
		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// EXPR OP ID
			generateArithmeticExpr(orig->children[1], fp, reg);
			char *reg_str = (char*)malloc(sizeof(char)*3);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tSUB %s,  [EBP + %dd]\n",
				reg_str, BASE_ADDR + looked_up->offset
			);

			return reg;
		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// EXPR OP EXPR
			generateArithmeticExpr(orig->children[1], fp, reg);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);
			fprintf(fp, "\tPUSH %s\n", reg_str);

			generateArithmeticExpr(orig->children[0], fp, reg);
			getRegFromInt(reg, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			fprintf(fp, "\tPOP %s\n", reg_str_temp);
			fprintf(fp, "\tSUB %s, %s\n", reg_str, reg_str_temp);

			return reg;

		}

	} else if (orig->symbol_type == TK_MUL) {
		if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// ID OP ID
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(1, reg_str);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV %s,  [EBP + %dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			fprintf(fp, "\tMUL dword [EBP + %dd]\n", BASE_ADDR + looked_up->offset);

			if (reg != 1) {
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// ID OP NUM
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(1, reg_str);

			fprintf(fp,
				"\tMOV %s,  [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset
			);
			fprintf(
				fp, "\tMUL %sd\n",
				orig->children[1]->tk_info.lexeme
			);

			if (reg != 1) {
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// i.e. the second child is Expr
			// ID OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV EAX,  [EBP + %dd]\n",
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tMUL %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;

		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// NUM OP NUM
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tMOV %s, %sd\n", reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tMUL %sd\n",
				orig->children[1]->tk_info.lexeme
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// NUM OP ID
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tMOV %s, %sd\n",
				reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tMUL dword [EBP + %dd]\n",
				BASE_ADDR + looked_up->offset
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// NUM OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV EAX, %sd\n",
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tMUL %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;

		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// EXPR OP NUM
			int ret = generateArithmeticExpr(orig->children[0], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV EAX, %s\n",
				reg_str_temp
			);

			getRegFromInt((ret+1) % 4, reg_str_temp);
			fprintf(fp, "\tMOV %s, %sd\n",
				reg_str_temp,
				orig->children[1]->tk_info.lexeme
			);

			fprintf(fp, "\tMUL %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// EXPR OP ID
			int ret = generateArithmeticExpr(orig->children[0], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV EAX, %s\n",
				reg_str_temp
			);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			getRegFromInt((ret+1) % 4, reg_str_temp);
			fprintf(fp, "\tMOV %s,  [EBP + %dd]\n",
				reg_str_temp,
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tMUL %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// EXPR OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, reg);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			// Push result of Right child
			getRegFromInt(ret, reg_str);
			fprintf(fp, "\tPUSH %s\n", reg_str);

			ret = generateArithmeticExpr(orig->children[0], fp, reg);
			getRegFromInt(ret, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			if (reg != 1) {
				fprintf(fp, "\tMOV EAX, %s\n", reg_str);
			}

			fprintf(fp, "\tPOP %s\n", reg_str_temp);
			fprintf(fp, "\tMUL %s\n", reg_str_temp);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		}
	} else if (orig->symbol_type == TK_DIV) {
		if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// ID OP ID
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(1, reg_str);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV %s,  [EBP + %dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			fprintf(fp, "\tDIV dword [EBP + %dd]\n", BASE_ADDR + looked_up->offset);

			if (reg != 1) {
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// ID OP NUM
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(1, reg_str);

			fprintf(fp,
				"\tMOV %s,  [EBP + %dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset
			);
			fprintf(
				fp, "\tDIV %sd\n",
				orig->children[1]->tk_info.lexeme
			);

			if (reg != 1) {
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_ID
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// i.e. the second child is Expr
			// ID OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[0]->st, orig->children[0]->tk_info.lexeme,
				strlen(orig->children[0]->tk_info.lexeme)
			);

			fprintf(fp, "\tMOV EAX,  [EBP + %dd]\n",
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tDIV %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;

		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// NUM OP NUM
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tMOV %s, %sd\n", reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tDIV %sd\n",
				orig->children[1]->tk_info.lexeme
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// NUM OP ID
			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tMOV %s, %sd\n",
				reg_str,
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tDIV dword [EBP + %dd]\n",
				BASE_ADDR + looked_up->offset
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type == TK_NUM
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// NUM OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV EAX, %sd\n",
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tDIV %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;

		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type == TK_NUM
		) {
			// EXPR OP NUM
			int ret = generateArithmeticExpr(orig->children[0], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV EAX, %s\n",
				reg_str_temp
			);

			getRegFromInt((ret+1) % 4, reg_str_temp);
			fprintf(fp, "\tMOV %s, %sd\n",
				reg_str_temp,
				orig->children[1]->tk_info.lexeme
			);

			fprintf(fp, "\tDIV %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type == TK_ID
		) {
			// EXPR OP ID
			int ret = generateArithmeticExpr(orig->children[0], fp, (reg+1) % 4);

			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			getRegFromInt(reg, reg_str);
			getRegFromInt(ret, reg_str_temp);

			fprintf(fp, "\tMOV EAX, %s\n",
				reg_str_temp
			);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			getRegFromInt((ret+1) % 4, reg_str_temp);
			fprintf(fp, "\tMOV %s,  [EBP + %dd]\n",
				reg_str_temp,
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tDIV %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		} else if (orig->children[0]->symbol_type != TK_ID
			&& orig->children[0]->symbol_type != TK_NUM
			&& orig->children[1]->symbol_type != TK_ID
			&& orig->children[1]->symbol_type != TK_NUM
		) {
			// EXPR OP EXPR
			int ret = generateArithmeticExpr(orig->children[1], fp, reg);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);

			// Push result of Right child
			getRegFromInt(ret, reg_str);
			fprintf(fp, "\tPUSH %s\n", reg_str);

			ret = generateArithmeticExpr(orig->children[0], fp, reg);
			getRegFromInt(ret, reg_str);
			getRegFromInt((reg + 1) % 4, reg_str_temp);

			if (reg != 1) {
				fprintf(fp, "\tMOV EAX, %s\n", reg_str);
			}

			fprintf(fp, "\tPOP %s\n", reg_str_temp);
			fprintf(fp, "\tDIV %s\n", reg_str_temp);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, EAX\n", reg_str);
			}

			return reg;
		}
	}

	return 0;
}

int getRegFromInt(int ret, char *reg) {
	switch (ret) {
		case 1:
			strcpy(reg, "EAX");
			break;

		case 2:
			strcpy(reg, "EBX");
			break;

		case 3:
			strcpy(reg, "ECX");
			break;

		case 4:
			strcpy(reg, "EDX");
			break;

		default:
			printf("%d\n", ret);
			strcpy(reg, "INVALID");
	}

	return strlen(reg);
}



/*
	Assign statement
	================
	Calculate the results from left side (recursively) in EAX
	then
		MOV OFFSET_OF_RHS, EAX
 */
