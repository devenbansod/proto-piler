/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "treeDef.h"
#include "AST.h"
#include "symbolTable.h"

/*
 * Main Function call
 */
treeNode* createAST(treeNode *parseTreeRoot) {
	if (parseTreeRoot == NULL
		&& parseTreeRoot->symbol_type == eps
	) {
		return NULL;
	} else {
		return reduceProgram(parseTreeRoot);
	}
}

TypeTable *globalTT;
SymbolTable *globalST;
/*
 * Helper function
 */
void copySymbolTableToChildren(treeNode* orig) {
	if (orig == NULL) {
		return;
	} else {
		int i;
		for (i = 0; i < orig->curr_children; i++) {
			orig->children[i]->st = orig->st;
		}
	}
}

treeNode* reduceProgram(treeNode* root) {
	globalTT = createTypeTable(10);

	int real = REAL_WIDTH;
	int integer = INT_WIDTH;
	insertType(globalTT, "int", 3, &integer, 0, NULL, NULL,  0);
	insertType(globalTT, "real", 4, &real, 0, NULL, NULL, 0);

	globalST = createSymbolTable(10);

	root->children[0] = reduceOtherFunctions(root->children[0]);
	root->children[1] = reduceMainFunction(root->children[1]);

	if (root->children[0])
		root->children[0]->parent = root;

	root->children[1]->parent = root;
	root->curr_children = 2;

	return root;
}

treeNode* reduceMainFunction(treeNode* mainFuncNode) {

	// No error checking as Main function always present
	treeNode* mainFuncNode_backup = mainFuncNode;
	mainFuncNode->st = createSymbolTable(20);
	copySymbolTableToChildren(mainFuncNode);

	mainFuncNode = reduceStmtsNode(mainFuncNode->children[1]);

	free(mainFuncNode_backup->children[0]);
	free(mainFuncNode_backup->children[2]);
	free(mainFuncNode_backup);

	return mainFuncNode;
}

treeNode* reduceOtherFunctions(treeNode* orig) {

	// Might reduce to EPS, so check
	if (orig->children[0]->symbol_type == eps) {
		free(orig->children[0]);
		free(orig);
		return NULL;
	} else {
		int i = 1;
		int size = 10;
		orig->children[0] = reduceFunction(orig->children[0]);
		orig->children[0]->parent = orig;

		orig->children = (treeNode**)realloc(orig->children, 10 * sizeof(treeNode*));
		treeNode* remainList = orig->children[1];
		treeNode* remainList_backup;

		if (remainList->children[0]->symbol_type != eps) {
			while (remainList->symbol_type != eps
				&& remainList->children[0]->symbol_type != eps
			) {
				remainList_backup = remainList;
				if (i == size - 1) {
					size *= 2;
					orig->children = (treeNode**)realloc(orig->children, size * sizeof(treeNode*));
				}

				orig->children[i++] = reduceFunction(remainList->children[0]);
				orig->children[i-1]->parent = orig;

				remainList = remainList->children[1];
				free(remainList_backup);
			}

		}

		orig->curr_children = i;
		return orig;
	}
}

treeNode* reduceFunction(treeNode* funcNode) {
	funcNode->symbol_type = funcNode->children[0]->symbol_type;
	funcNode->tk_info = funcNode->children[0]->tk_info;
	funcNode->st = createSymbolTable(20);
	copySymbolTableToChildren(funcNode);

	// now make the first child as input paramenter list
	funcNode->children[0] = reduceInputPar(funcNode->children[1]);

	// insert() into Function Table OPTIONAL for now

	// now make the 2nd child as output paramenter list
	funcNode->children[1] = reduceOutputPar(funcNode->children[2]);

	// now make the 3nd child as stmts list
	funcNode->children[2] = reduceStmtsNode(funcNode->children[4]);

	funcNode->children[0]->parent = funcNode;
	funcNode->children[1]->parent = funcNode;

	// if returned value is not NULL
	if (funcNode->children[1])
		funcNode->children[1]->parent = funcNode;

	// I think this is redundant check
	if (funcNode->children[2])
		funcNode->children[2]->parent = funcNode;

	funcNode->curr_children = 3;
	return funcNode;
}


treeNode* reduceInputPar(treeNode* inputParNode) {
	treeNode* back_up = inputParNode;

	// remove unnecesarry nodes
	// free(inputParNode->children[0]);
	// free(inputParNode->children[1]);
	// free(inputParNode->children[2]);
	// free(inputParNode->children[3]);
	// free(inputParNode->children[5]);

	inputParNode = reduceParameterList(inputParNode->children[4]);
	// free(back_up);

	return inputParNode;
}

treeNode* reduceOutputPar(treeNode* outputParNode) {
	treeNode* back_up = outputParNode;

	// remove unnecesarry nodes
	// free(outputParNode->children[0]);
	// free(outputParNode->children[1]);
	// free(outputParNode->children[2]);
	// free(outputParNode->children[3]);
	// free(outputParNode->children[5]);

	if (outputParNode->children[4]->symbol_type == eps) {
		// free(outputParNode->children[4]);
		// free(back_up);
		return NULL;
	} else {
		outputParNode = reduceParameterList(outputParNode->children[4]);
	}

	return outputParNode;
}


treeNode* reduceParameterList(treeNode* paramListNode) {
	treeNode* orig = paramListNode;

	orig->children[0] = reduceDatatype(orig->children[0]);
	orig->children[0]->parent = orig;
	orig->children[1]->parent = orig;

	orig->children = (treeNode**)realloc(orig->children, 10 * sizeof(treeNode*));
	int size = 10;

	int i = 2, j = 1;
	// free(orig->children[1]);

	treeNode* remainList = orig->children[2];
	treeNode* remainList_backup;
	while (remainList->children[0]->symbol_type != eps) {
		remainList_backup = remainList;
		if (i == size - 1) {
			size *= 2;
			orig->children = (treeNode**)realloc(orig->children, size * sizeof(treeNode*));
		}

		if (j % 2 != 0) {
			// remainList node is correct
			remainList = remainList->children[1];
		} else {
			// remainList node is actually paramList node
			orig->children[i++] = reduceDatatype(remainList->children[0]);
			orig->children[i++] = remainList->children[1];

			orig->children[i-1]->parent = orig;
			orig->children[i-2]->parent = orig;

			// free(remainList->children[1]);
			remainList = remainList->children[2];
		}
		j++;
		// free(remainList_backup);
	}

	orig->curr_children = i;
	return orig;
}

treeNode* reduceDatatype(treeNode* datatypeNode) {
	treeNode* datatypeNode_backup = datatypeNode;

if (datatypeNode->children[0]->symbol_type == primitiveDatatype)
		datatypeNode = datatypeNode->children[0]->children[0];
	else
		datatypeNode = datatypeNode->children[0]->children[1];

	datatypeNode->parent = datatypeNode_backup->parent;
	datatypeNode->curr_children = 0;

	// free(datatypeNode_backup->children[0]);
	// free(datatypeNode_backup);

	return datatypeNode;
}

treeNode* reduceStmtsNode(treeNode* stmtsNode) {

	copySymbolTableToChildren(stmtsNode);

	stmtsNode->children[0] = reduceTypeDefns(
		stmtsNode->children[0]
	);

	stmtsNode->children[1] = reduceDeclarations(
		stmtsNode->children[1]
	);

	stmtsNode->children[2] = reduceOtherStmts(
		stmtsNode->children[2]
	);

	stmtsNode->children[3] = reduceReturnStmt(
		stmtsNode->children[3]
	);

	int i = 0;
	for(i = 0; i < 4; i++) {
		if (stmtsNode->children[i])
			stmtsNode->children[i]->parent = stmtsNode;
	}


	stmtsNode->curr_children = 4;

	return stmtsNode;
}

treeNode* reduceTypeDefns(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		return NULL;
	}

	copySymbolTableToChildren(orig);
	orig->children[0] = reduceTypeDefn(orig->children[0]);

	treeNode* defns = orig->children[1];
	orig->children = (treeNode**)realloc(orig->children, 10 * sizeof(treeNode*));
	int size = 10;

	int i = 1;
	treeNode* defns_backup;
	while(defns->children[0]->symbol_type != eps) {
		defns_backup = defns;
		if (i == size - 1) {
			size *= 2;
			orig->children = (treeNode**)realloc(
				orig->children, size * sizeof(treeNode*)
			);
		}

		copySymbolTableToChildren(defns);
		orig->children[i++] = reduceTypeDefn(defns->children[0]);
		defns = defns->children[1];
		// free(defns_backup);
	}

	orig->curr_children = i;
	return orig;
}

treeNode* reduceTypeDefn(treeNode* orig) {
	copySymbolTableToChildren(orig);

	// free(orig->children[0]);
	orig->children[0] = orig->children[1];

	orig->children[1] = reduceFieldDefns(orig->children[2]);

	// free(orig->children[3]);
	// free(orig->children[4]);

	orig->curr_children = 2;

	return orig;
}

treeNode* reduceFieldDefns(treeNode* orig) {
	copySymbolTableToChildren(orig);

	char **field_names = (char**)malloc(10* sizeof(char*));
	char **field_types = (char**)malloc(10* sizeof(char*));
	int *width = (int*)malloc(10* sizeof(int));
	int *offset = (int*)malloc(10* sizeof(int));

	orig->children[0] = reduceFieldDefn(orig->children[0]);
	orig->children[1] = reduceFieldDefn(orig->children[1]);

	field_names[0] = (char*) malloc(25*sizeof(char));
    memset(field_names[0], '\0', 25);

    field_types[0] = (char*) malloc(25*sizeof(char));
    memset(field_types[0], '\0', 25);

	strcpy(field_names[0], orig->children[0]->children[0]->tk_info.lexeme);
	strcpy(field_types[0], orig->children[0]->children[1]->tk_info.lexeme);

	if (orig->children[0]->children[0]->symbol_type==TK_REAL){
		width[0] = REAL_WIDTH;
	} else {
		width[0] = INT_WIDTH;
	}
	offset[0] = 0;

	field_names[1] = (char*) malloc(25*sizeof(char));
    memset(field_names[1], '\0', 25);

    field_types[1] = (char*) malloc(25*sizeof(char));
    memset(field_types[1], '\0', 25);

	strcpy(field_names[1], orig->children[1]->children[0]->tk_info.lexeme);
	strcpy(field_types[1], orig->children[1]->children[1]->tk_info.lexeme);

	if (orig->children[1]->children[0]->symbol_type == TK_REAL) {
		width[1] = REAL_WIDTH;
	} else{
		width[1] = INT_WIDTH;
	}
	offset[1] = offset[0] + width[0];

	treeNode* defns = orig->children[2];
	orig->children = (treeNode**)realloc(orig->children, 10 * sizeof(treeNode*));
	int size = 10;
	int i = 2;
	treeNode* defns_backup;

	while(defns->symbol_type != eps
		&& defns->children[0]->symbol_type != eps
	) {
		defns_backup = defns;
		if (i == size - 1) {
			size *= 2;
			orig->children = (treeNode**)realloc(
				orig->children, size * sizeof(treeNode*)
			);
			field_names = (char**) realloc(field_names, size*sizeof(char*));
			field_types = (char**) realloc(field_types, size*sizeof(char*));
			width = (int*) realloc(width, size*sizeof(int));
			offset = (int*) realloc(offset, size*sizeof(int));
		}
		copySymbolTableToChildren(defns);

		orig->children[i] = reduceFieldDefn(defns->children[0]);
		field_names[i] = (char* )malloc(25*sizeof(char));
        memset(field_names[i], '\0', 25);
		strcpy(field_names[i], orig->children[i]->children[0]->tk_info.lexeme);

		field_types[i] = (char* )malloc(25*sizeof(char));
        memset(field_types[i], '\0', 25);
		strcpy(field_types[i], orig->children[i]->children[1]->tk_info.lexeme);

		if (orig->children[i]->children[0]->symbol_type == TK_REAL) {
			width[i] = REAL_WIDTH;
		} else{
			width[i] = INT_WIDTH;
		}
		offset[i] = offset[i-1] + width[i-1];

		defns = defns->children[1];
		// free(defns_backup);
		i++;
	}
	orig->curr_children = i;
	char* type = (char*) malloc(25*sizeof(char));
	strcpy(type, orig->parent->children[0]->tk_info.lexeme);

	insertType(
		globalTT, type, strlen(type), width, offset, field_names,
		field_types, orig->curr_children
	);
	free(type);
	free(width);
	free(offset);
	free(field_names);

	return orig;
}

treeNode* reduceFieldDefn(treeNode* orig) {
	copySymbolTableToChildren(orig);

	// free(orig->children[0]);
	orig->children[0] = orig->children[1]->children[0];

	orig->children[1] = orig->children[3];

	// free(orig->children[2]);
	// free(orig->children[4]);

	orig->curr_children = 2;

	return orig;
}

treeNode* reduceDeclarations(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		// free(orig->children[0]);
		// free(orig);
		return NULL;
	}

	copySymbolTableToChildren(orig);
	treeNode* decns = orig;
	orig->children = (treeNode**)realloc(orig->children, 10 * sizeof(treeNode*));
	int size = 10;
	orig->children[0] = reduceDeclaration(orig->children[0]);

	int i = 1;
	treeNode *decns_backup, *temp;

	decns = orig->children[1];
	while(decns->children[0]->symbol_type != eps) {
		decns_backup = decns;
		if (i == size - 1) {
			size *= 2;
			orig->children = (treeNode**)realloc(
				orig->children, size * sizeof(treeNode*)
			);
		}

		copySymbolTableToChildren(decns);
		temp = reduceDeclaration(decns->children[0]);

		if (temp == NULL) {
			i++;
			break;
		}

		orig->children[i++] = temp;
		temp->parent = orig;

		decns = decns->children[1];
		// free(decns_backup);
	}

	orig->curr_children = i;
	orig->st = createSymbolTable(10);
	int k;
	for (k = 0; k < i; ++k){
		treeNode* declarationNode = orig->children[k];
		if(declarationNode->curr_children > 2){
			// insert in global

			int type_len = strlen(declarationNode->children[0]->tk_info.lexeme);
			char *type = (char*)malloc(type_len * sizeof(char));

			strcpy(type, declarationNode->children[0]->tk_info.lexeme);
			insertSymbol(globalST, declarationNode->children[1]->tk_info.lexeme,
				strlen(declarationNode->children[1]->tk_info.lexeme), type
			);

			free(type);
		}
		else{
			// insert in local

			// check if exists in global
			int id_len = strlen(declarationNode->children[1]->tk_info.lexeme);
			char *id = (char*) malloc(id_len * sizeof(char));

			strcpy(id, declarationNode->children[1]->tk_info.lexeme);

			if (lookupSymbol(globalST, id, id_len) == NULL) {
				int type_len = strlen(declarationNode->children[0]->tk_info.lexeme);
				char *type = (char*)malloc(type_len * sizeof(char));

				strcpy(type, declarationNode->children[0]->tk_info.lexeme);

				insertSymbol(orig->st, id, id_len, type);
				free(type);
			}
			else {
				fprintf(stderr, "%s exists in global symbol table\n", id);
				exit(1);
			}
			free(id);
		}
	}

	printf("Local SymbolTable:\n"); printSymbolTable(orig->st);
	printf("Global SymbolTable:\n"); printSymbolTable(globalST);

	return orig;
}

treeNode* reduceDeclaration(treeNode* orig) {
	// free(orig->children[2]);
	if (orig->symbol_type == eps) {
		return NULL;
	}

	copySymbolTableToChildren(orig);
	orig->children[0] = reduceDatatype(orig->children[1]);
	orig->children[1] = orig->children[3];
	orig->children[0]->parent = orig;
	orig->children[1]->parent = orig;

	if (orig->children[4]->children[0]->symbol_type == eps) {
		orig->children[2] = NULL;
		orig->curr_children = 2;
		// free(orig->children[4]->children[0]);
		// free(orig->children[4]);
	} else {
		orig->children[2] = orig->children[4]->children[1];
		orig->children[2]->parent = orig;
		orig->curr_children = 3;
	}

	return orig;
}

treeNode* reduceOtherStmts(treeNode* orig) {
	treeNode* stmts = orig;
	if (orig->children[0]->symbol_type == eps) {
		// free(orig->children[0]);
		// free(orig);
		return NULL;
	}

	copySymbolTableToChildren(orig);
	orig->children = (treeNode**)realloc(orig->children, 10 * sizeof(treeNode*));
	int size = 10;

	int i = 0;
	treeNode* stmts_backup;

	orig->children[i++] = reduceStatement(stmts->children[0]);
	stmts = orig->children[1];

	while(stmts->children[0]->symbol_type != eps) {
		stmts_backup = stmts;
		if (i == size - 1) {
			size *= 2;
			orig->children = (treeNode**)realloc(
				orig->children, size * sizeof(treeNode*)
			);
		}

		copySymbolTableToChildren(stmts);
		orig->children[i++] = reduceStatement(stmts->children[0]);

		if (orig->children[i-1])
			orig->children[i-1]->parent = orig;

		stmts = stmts->children[1];
		// free(stmts_backup);
	}

	orig->curr_children = i;
	return orig;
}

treeNode* reduceStatement(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);

	switch (orig->children[0]->symbol_type) {
		case assignmentStmt:
			orig = reduceAssignStmt(
				orig->children[0]
			);
			break;

		case iterativeStmt:
			orig = reduceIterativeStmt(
				orig->children[0]
			);
			break;

		case conditionalStmt:
			orig = reduceConditionalStmt(
				orig->children[0]
			);
			break;

		case ioStmt:
			orig = reduceIoStmt(
				orig->children[0]
			);
			break;

		case funCallStmt:
			orig = reduceFunCallStmt(
				orig->children[0]
			);
			break;
		default:
			return NULL;
	}

	orig->parent = backup->parent;
	// free(backup);
	return orig;
}

treeNode* reduceReturnStmt(treeNode* orig) {
	copySymbolTableToChildren(orig);

	free(orig->children[0]);
	free(orig->children[2]);

	if (orig->children[1]->symbol_type == eps) {
		return NULL;
	} else {
		treeNode* backup = reduceIdList(orig->children[1]->children[1]);
		if (backup)
			backup->curr_children = 1;
		return backup;
	}
}

treeNode* reduceAssignStmt(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);

	orig = backup->children[1];
	orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

	orig->children[0] = reduceSingleOrRecId(
		backup->children[0]
	);

	orig->children[1] = reduceArithmeticExpr(
		backup->children[2]
	);

	if (orig->children[0])
		orig->children[0]->parent = orig;
	if (orig->children[1])
		orig->children[1]->parent = orig;

	orig->parent = backup->parent;

	orig->curr_children = 2;
	// free(backup);

	return orig;
}

treeNode* reduceIterativeStmt(treeNode* orig) {
	copySymbolTableToChildren(orig);

	// free(orig->children[0]);
	// free(orig->children[1]);
	// free(orig->children[3]);
	// free(orig->children[6]);
	orig->children[0] = reduceBooleanExpr(orig->children[2]);
	orig->children[1] = reduceStatement(orig->children[4]);
	orig->children[2] = reduceOtherStmts(orig->children[5]);

	if (orig->children[0])
		orig->children[0]->parent = orig;
	if (orig->children[1])
		orig->children[1]->parent = orig;
	if (orig->children[2])
		orig->children[2]->parent = orig;


	orig->curr_children = 3;
	return orig;
}

treeNode* reduceConditionalStmt(treeNode* orig) {
	copySymbolTableToChildren(orig);

	// free(orig->children[0]);
	// free(orig->children[1]);
	orig->children[0] = reduceBooleanExpr(orig->children[2]);
	orig->children[1] = reduceStatement(orig->children[5]);
	orig->children[2] = reduceOtherStmts(orig->children[6]);

	if (orig->children[0])
		orig->children[0]->parent = orig;
	if (orig->children[1])
		orig->children[1]->parent = orig;
	if (orig->children[2])
		orig->children[2]->parent = orig;


	if (orig->children[7]->symbol_type == TK_ENDIF) {
		orig->curr_children = 3;
	} else {
		orig->children[3] = reduceStatement(orig->children[7]->children[1]);
		orig->children[4] = reduceOtherStmts(orig->children[7]->children[2]);

		if (orig->children[3])
			orig->children[3]->parent = orig;
		if (orig->children[4])
			orig->children[5]->parent = orig;
		orig->curr_children = 5;
	}

	return orig;
}

treeNode* reduceIoStmt(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);

	if (orig->children[0]->symbol_type == TK_READ) {
		// free(orig->children[1]);
		orig = orig->children[0];
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		orig->children[0] = reduceSingleOrRecId(backup->children[2]);
		orig->children[0]->parent = orig;
		orig->curr_children = 1;
	} else {
		// free(orig->children[1]);

		orig = orig->children[0];
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));
		orig->children[0] = reduceAllVar(backup->children[2]);

		orig->curr_children = 1;
	}
	if (orig->children[0])
		orig->children[0]->parent = orig;

	orig->parent = backup->parent;
	orig->curr_children = 1;

	return orig;
}

treeNode* reduceFunCallStmt(treeNode* orig) {
	copySymbolTableToChildren(orig);
	treeNode* backup = orig;

	orig = orig->children[2];
	orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

	orig->children[0] = reduceOutputParameters(backup->children[0]);
	orig->children[1] = reduceInputParameters(backup->children[5]);

	// free(backup->children[1]);
	// free(backup->children[3]);
	// free(backup->children[4]);


	if (orig->children[0])
		orig->children[0]->parent = orig;
	if (orig->children[1])
		orig->children[1]->parent = orig;

	orig->parent = backup->parent;
	orig->curr_children = 2;
	return orig;
}

treeNode* reduceOutputParameters(treeNode* orig) {
	treeNode* backup = orig;
	if (orig->symbol_type == eps) {
		// free(orig);
		return NULL;
	} else {
		copySymbolTableToChildren(orig);
		// free(orig->children[0]);
		// free(orig->children[2]);
		// free(orig->children[3]);

		orig = reduceIdList(orig->children[1]);
		orig->parent = backup->parent;
		// free(backup);
	}
	return orig;
}

treeNode* reduceInputParameters(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);

	// free(orig->children[0]);
	// free(orig->children[2]);

	orig = reduceIdList(orig->children[1]);
	orig->parent = backup->parent;
	// free(backup);

	return orig;
}

treeNode* reduceIdList(treeNode* orig) {

	if (orig == NULL || orig->symbol_type == eps) {
		return NULL;
	}
	copySymbolTableToChildren(orig);

	int size = 10;
	orig->children = (treeNode**)realloc(orig->children, 10 * sizeof(treeNode*));

	int i = 1, j = 1;
	treeNode* remainList = orig->children[1];
	treeNode* remainList_backup;

	while (remainList->symbol_type != eps
		&& remainList->children[0]->symbol_type != eps
	) {
		remainList_backup = remainList;
		if (i == size - 1) {
			size *= 2;
			orig->children = (treeNode**)realloc(
				orig->children, size * sizeof(treeNode*)
			);
		}

		copySymbolTableToChildren(orig);
		if (j % 2 == 0) {
			// remainList node is correctly moreIds
			remainList = remainList->children[1];
		} else {
			// remainList node is actually idList node
			orig->children[i++] = remainList->children[0];
			orig->children[i-1]->parent = orig;
			remainList = remainList->children[1];
		}
		j++;

		// free(remainList_backup);
	}

	orig->curr_children = i;
	return orig;
}

treeNode* reduceSingleOrRecId(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);

	if (orig->children[1]->children[0]->symbol_type == eps) {
		// free(orig->children[1]);
		orig = orig->children[0];
		orig->parent = backup->parent;
		orig->curr_children = 0;
		// free(backup);
	} else {
		orig->children[1] = orig->children[1]->children[1];
		orig->children[1]->parent = orig;

		orig->curr_children = 2;
	}

	return orig;
}

treeNode* reduceAllVar(treeNode* orig) {
	treeNode* backup = orig;

	copySymbolTableToChildren(orig);
	if (orig->children[0]->symbol_type == some_types) {
		orig = orig->children[0]->children[0];
		orig->parent = backup->parent;
	} else {
		// other_types
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		if (orig->children[0]->children[1]->children[0]->symbol_type == eps) {
			orig->children[0] = orig->children[0]->children[0];
			orig->children[0]->parent = orig;
			orig->curr_children = 1;
		} else {
			orig->children[1] = backup->children[0]->children[1]->children[1];
			orig->children[0] = orig->children[0]->children[0];

			orig->children[0]->parent = orig;
			orig->children[1]->parent = orig;
			orig->parent = backup->parent;
			// free(backup);
			orig->curr_children = 2;
		}
	}

	return orig;
}

treeNode* reduceArithmeticExpr(treeNode* orig) {

	copySymbolTableToChildren(orig);
	orig->children[0] = reduceTerm(orig->children[0]);
	orig->children[1] = reduceExpPrime(orig->children[1]);
	treeNode* backup = orig;

	if (orig->children[0])
			orig->children[0]->parent = orig;
	if (orig->children[1])
			orig->children[1]->parent = orig;

	if (orig->children[1] == NULL
		|| orig->children[1]->symbol_type == eps
	) {
		orig = orig->children[0];
		orig->parent = backup->parent;
	} else {
		orig = orig->children[1];
		orig->children = (treeNode**)realloc(orig->children, sizeof(treeNode*) * 2);

		orig->children[1] = orig->children[0];
		orig->children[0] = backup->children[0];
		orig->children[0]->parent = orig;
		orig->curr_children = 2;
	}

	// free(backup);
	return orig;
}

treeNode* reduceExpPrime(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		// free(orig->children[0]);
		// free(orig);
		return NULL;
	} else {
		treeNode* backup = orig;
		copySymbolTableToChildren(orig);
		orig = orig->children[0]->children[0];

		orig->children = (treeNode**)malloc(2 * sizeof(treeNode*));
		orig->children[0] = reduceTerm(backup->children[1]);
		orig->children[1] = reduceExpPrime(backup->children[2]);

		if (orig->children[0])
			orig->children[0]->parent = orig;
		if (orig->children[1])
			orig->children[1]->parent = orig;

		orig->curr_children = 2;

		// free(backup);
		return orig;
	}
}

treeNode* reduceTerm(treeNode* orig) {
	treeNode* backup = orig;

	copySymbolTableToChildren(orig);
	orig->children[0] = reduceFactor(orig->children[0]);
	orig->children[1] = reduceTermPrime(orig->children[1]);

	if (orig->children[0])
		orig->children[0]->parent = orig;
	if (orig->children[1])
		orig->children[1]->parent = orig;

	if (orig->children[1] == NULL
		|| orig->children[1]->symbol_type == eps
	) {
		orig = orig->children[0];
		orig->parent = backup->parent;
	} else {
		orig = orig->children[1];
		orig->children = (treeNode**)realloc(orig->children, sizeof(treeNode*) * 2);

		orig->children[1] = orig->children[0];
		orig->children[0] = backup->children[0];
		orig->children[0]->parent = orig;
		orig->curr_children = 2;
	}

	// free(backup);
	return orig;
}

treeNode* reduceTermPrime(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		// free(orig->children[0]);
		// free(orig);
		return NULL;
	} else {
		treeNode* backup = orig;
		copySymbolTableToChildren(orig);
		orig = orig->children[0]->children[0];

		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));
		copySymbolTableToChildren(orig);

		orig->children[0] = reduceFactor(backup->children[1]);
		orig->children[1] = reduceTermPrime(backup->children[2]);

		orig->children[0]->parent = orig;

		if (orig->children[1]) {
			orig->children[1]->parent = orig;
		}

		orig->curr_children = 2;

		// free(backup);
		return orig;
	}
}

treeNode* reduceFactor(treeNode* orig) {
	treeNode* backup = orig;
	if (orig->children[0]->symbol_type == all) {
		copySymbolTableToChildren(orig);
		orig = reduceAll(orig->children[0]);
		orig->parent = backup->parent;
	} else {
		copySymbolTableToChildren(orig);
		orig = reduceArithmeticExpr(orig->children[1]);
		orig->parent = backup->parent;
		// free(backup);
	}

	return orig;
}

treeNode* reduceAll(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);
	if (orig->children[0]->symbol_type == TK_ID) {
		if (orig->children[1]->children[0]->symbol_type == eps) {
			// free(orig->children[1]->children[0]);
			// free(orig->children[1]);
			orig->children[1] = NULL;
			orig = orig->children[0];
			orig->parent = backup->parent;
			// free(backup);
		} else {
			backup = orig->children[1];
			orig->children[1] = orig->children[1]->children[1];
			orig->children[1]->parent = orig;
			// free(backup);
			orig->curr_children = 2;
		}
	} else {
		orig = orig->children[0];
		// free(backup);
	}

	return orig;
}

treeNode* reduceBooleanExpr(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);
	if (orig->children[0]->symbol_type == TK_OP) {
		orig = orig->children[3];
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		orig->children[0] = reduceBooleanExpr(backup->children[1]);
		orig->children[0]->parent = orig;
		// free(backup->children[0]);
		// free(backup->children[2]);
		// free(backup->children[4]);
		// free(backup->children[6]);
		orig->children[1] = reduceBooleanExpr(backup->children[5]);

		orig->curr_children = 2;
	} else if (orig->children[0]->symbol_type == TK_NOT) {
		orig = orig->children[0];
		orig->children = (treeNode**)realloc(orig->children, 1 * sizeof(treeNode*));

		orig->children[0] = reduceBooleanExpr(backup->children[2]);
		orig->children[0]->parent = orig;

		orig->curr_children = 1;

		// free(backup->children[1]);
		// free(backup->children[3]);
	} else {
		orig = orig->children[1]->children[0];
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));
		orig->children[0] = backup->children[0]->children[0];
		orig->children[1] = backup->children[2]->children[0];

		orig->children[0]->parent = orig;
		orig->children[1]->parent = orig;

		// free(backup->children[0]);
		// free(backup->children[1]);
		// free(backup->children[2]);
		orig->curr_children = 2;
	}

	return orig;
}
