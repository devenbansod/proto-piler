/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "codeGen.h"

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
	fprintf(fp, "SECTION\t.text\n\n");
	fprintf(fp, "global\t_start\n");
	fprintf(fp, "_start:\n");

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
				break;

			case conditionalStmt:
				generateConditional(orig->children[i], fp);
				break;

			case iterativeStmt:
				generateConditional(orig->children[i], fp);
				break;

			case TK_WRITE:
			case TK_READ:
				generateIO(orig->children[i], fp);
				break;

			default:
				continue;
		}
	}
}


/*
 * Write 'db' things may be
 */
int generateDeclarations(treeNode* orig, FILE *fp) {

}

/*
 * First, evaluate the 2nd child (recursively)
 * Then, append to it,
 * assign instruction with Dst arg as 1st child
 * and Src arg as temporary generated from 2nd child' eval
 */
int generateAssign(treeNode* orig, FILE *fp) {

	symbolTableElem* looked_up = lookupSymbolHelper(
		orig->st, orig->children[0]->tk_info.lexeme,
		strlen(orig->children[0]->tk_info.lexeme)
	);

	if (orig->children[1]->symbol_type == TK_NUM) {
		// the real assign statement
		fprintf(fp,
			"\tMOV [%dd], %s\n",
			BASE_ADDR + looked_up->offset,
			orig->children[1]->tk_info.lexeme
		);
	} else {
		// since assign will always be from a expr to memory
		// we always generate MOV 1stChild, AX i.e. reg = 1 as argument
		int ret = generateArithmeticExpr(orig->children[1], fp, 1);

		// the real assign statement
		fprintf(fp, "\tMOV [%dd], AX\n", BASE_ADDR + looked_up->offset);
	}
}

/*
 *
 */
int generateIterative(treeNode* orig, FILE *fp) {
	fprintf(stderr, "Iterative on line %d\n", orig->children[0]->tk_info.line_no);
}

/*
 *
 */
int generateConditional(treeNode* orig, FILE *fp) {
	fprintf(stderr, "Conditional on line %d\n", orig->children[0]->tk_info.line_no);

	if (orig->symbol_type == TK_AND) {
		// condition1 && condition2
		char* true_label = (char*)malloc(sizeof(char) * 5);
		char* false_label = (char*)malloc(sizeof(char) * 5);

	} else if (orig->symbol_type == TK_AND) {
		// condition1 || condition2
	} else {
		// relational operator
	}
}

/*
 *
 */
int generateRelational(treeNode* orig, FILE *fp, char* true_label, char* false_label) {

	if (orig->children[0]->symbol_type == TK_NUM
		&& orig->children[1]->symbol_type == TK_NUM
	) {
		char* reg_str = (char*)malloc(sizeof(char) * 3);
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

		fprintf(fp, "\tCMP %s, [%dd]\n",
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

		fprintf(fp, "\tMOV %s, [%dd]\n",
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

		fprintf(fp, "\tMOV %s, [%dd]\n",
			reg_str,
			BASE_ADDR + looked_up->offset
		);

		looked_up = lookupSymbolHelper(
			orig->st,
			orig->children[1]->tk_info.lexeme,
			strlen(orig->children[1]->tk_info.lexeme)
		);

		fprintf(fp, "\tCMP %s, [%dd]\n",
			reg_str,
			BASE_ADDR + looked_up->offset
		);
	}



	if (orig->symbol_type == TK_LT) {
		fprintf(fp, "JL %s\n", true_label);
		fprintf(fp, "JMP %s\n", false_label);
	} else if (orig->symbol_type == TK_LE) {
		fprintf(fp, "JLE %s\n", true_label);
		fprintf(fp, "JMP %s\n", false_label);
	} else if (orig->symbol_type == TK_GT) {
		fprintf(fp, "JG %s\n", true_label);
		fprintf(fp, "JMP %s\n", false_label);
	} else if (orig->symbol_type == TK_GE) {
		fprintf(fp, "JGE %s\n", true_label);
		fprintf(fp, "JMP %s\n", false_label);
	} else if (orig->symbol_type == TK_EQ) {
		fprintf(fp, "JE %s\n", true_label);
		fprintf(fp, "JMP %s\n", false_label);
	} else {
		fprintf(fp, "JNE %s\n", true_label);
		fprintf(fp, "JMP %s\n", false_label);
	}

	return 1;
}

/*
 *
 */
int generateIO(treeNode* orig, FILE *fp) {
	fprintf(stderr, "IO on line %d\n", orig->tk_info.line_no);
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

			fprintf(fp, "\tMOV %s, [%dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			fprintf(fp, "\tADD %s, [%dd]\n", reg_str, BASE_ADDR + looked_up->offset);

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
				"\tMOV %s, [%dd]\n",
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

			fprintf(fp, "\tMOV %s, [%dd]\n",
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

			fprintf(fp, "\tADD %s, [%dd]\n", reg_str,
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

			fprintf(fp, "\tMOV %s, [%dd]\n",
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
			int ret = generateArithmeticExpr(orig->children[1], fp, reg);
			char *reg_str = (char*)malloc(sizeof(char)*3);
			char *reg_str_temp = (char*)malloc(sizeof(char)*3);
			getRegFromInt(reg, reg_str);
			fprintf(fp, "\tPUSH %s\n", reg_str);

			ret = generateArithmeticExpr(orig->children[0], fp, reg);
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

			fprintf(fp, "\tMOV %s, [%dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			fprintf(fp, "\tSUB %s, [%dd]\n", reg_str, BASE_ADDR + looked_up->offset);

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
				"\tMOV %s, [%dd]\n",
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

			fprintf(fp, "\tMOV %s, [%dd]\n",
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

			fprintf(fp, "\tSUB %s, [%dd]\n", reg_str,
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
			int ret = generateArithmeticExpr(orig->children[1], fp, reg);
			char *reg_str = (char*)malloc(sizeof(char)*3);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			getRegFromInt(reg, reg_str);

			fprintf(fp, "\tSUB %s, [%dd]\n",
				reg_str, BASE_ADDR + looked_up->offset
			);

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
			getRegFromInt(reg, reg_str);
			fprintf(fp, "\tPUSH %s\n", reg_str);

			ret = generateArithmeticExpr(orig->children[0], fp, reg);
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

			fprintf(fp, "\tMOV %s, [%dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			fprintf(fp, "\tMUL [%dd]\n", BASE_ADDR + looked_up->offset);

			if (reg != 1) {
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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
				"\tMOV %s, [%dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset
			);
			fprintf(
				fp, "\tMUL %sd\n",
				orig->children[1]->tk_info.lexeme
			);

			if (reg != 1) {
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMOV AX, [%dd]\n",
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tMUL %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMUL [%dd]\n",
				BASE_ADDR + looked_up->offset
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMOV AX, %sd\n",
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tMUL %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMOV AX, %s\n",
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
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMOV AX, %s\n",
				reg_str_temp
			);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			getRegFromInt((ret+1) % 4, reg_str_temp);
			fprintf(fp, "\tMOV %s, [%dd]\n",
				reg_str_temp,
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tMUL %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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
				fprintf(fp, "\tMOV AX, %s\n", reg_str);
			}

			fprintf(fp, "POP %s\n", reg_str_temp);
			fprintf(fp, "\tMUL %s\n", reg_str_temp);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMOV %s, [%dd]\n", reg_str, BASE_ADDR + looked_up->offset);

			looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);

			fprintf(fp, "\tDIV [%dd]\n", BASE_ADDR + looked_up->offset);

			if (reg != 1) {
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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
				"\tMOV %s, [%dd]\n",
				reg_str,
				BASE_ADDR + looked_up->offset
			);
			fprintf(
				fp, "\tDIV %sd\n",
				orig->children[1]->tk_info.lexeme
			);

			if (reg != 1) {
				getRegFromInt(reg, reg_str);
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMOV AX, [%dd]\n",
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tDIV %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tDIV [%dd]\n",
				BASE_ADDR + looked_up->offset
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMOV AX, %sd\n",
				orig->children[0]->tk_info.lexeme
			);

			fprintf(fp, "\tDIV %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMOV AX, %s\n",
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
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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

			fprintf(fp, "\tMOV AX, %s\n",
				reg_str_temp
			);

			symbolTableElem* looked_up = lookupSymbolHelper(
				orig->children[1]->st, orig->children[1]->tk_info.lexeme,
				strlen(orig->children[1]->tk_info.lexeme)
			);
			getRegFromInt((ret+1) % 4, reg_str_temp);
			fprintf(fp, "\tMOV %s, [%dd]\n",
				reg_str_temp,
				BASE_ADDR + looked_up->offset
			);

			fprintf(fp, "\tDIV %s\n",
				reg_str_temp
			);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
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
				fprintf(fp, "\tMOV AX, %s\n", reg_str);
			}

			fprintf(fp, "POP %s\n", reg_str_temp);
			fprintf(fp, "\tDIV %s\n", reg_str_temp);

			if (reg != 1) {
				fprintf(fp, "\tMOV %s, AX\n", reg_str);
			}

			return reg;
		}
	}

}

int getRegFromInt(int ret, char *reg) {
	switch (ret) {
		case 1:
			strcpy(reg, "AX");
			break;

		case 2:
			strcpy(reg, "BX");
			break;

		case 3:
			strcpy(reg, "CX");
			break;

		case 4:
			strcpy(reg, "DX");
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
	Calculate the results from left side (recursively) in AX
	then
		MOV OFFSET_OF_RHS, AX
 */
