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

int curr_number = 0;
int curr_number_backup = 0;
int sem_error = 0;

/*
 * Main Function call to create and return AST
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
FunctionTable *globalFT;

int curr_max = 9;
int curr_global_offset = OFFSET_START;
int curr_global_offset_backup = OFFSET_START;

/*
 * Helper function
 	Copies symbol table to all the children nodes
 */
void copySymbolTableToChildren(treeNode* orig) {
	if (orig == NULL) {
		return;
	} else {
		int i;
		// printf("%d -> %d\n", orig->symbol_type, orig->curr_children);
		for (i = 0; i < orig->curr_children; i++) {
			orig->children[i]->st = orig->st;
		}
	}
}

//  function to reduce program statement
treeNode* reduceProgram(treeNode* root) {
	globalTT = createTypeTable(10);

	int real = REAL_WIDTH;
	int integer = INT_WIDTH;
	insertType(globalTT, "int", 3, &integer, 0, NULL, NULL,  0);
	insertType(globalTT, "real", 4, &real, 0, NULL, NULL, 0);

	allST = (SymbolTable**)malloc(sizeof(SymbolTable*) * (curr_max+1));
	globalST = createSymbolTable(10);
	strcpy(globalST->scope, "global");

	globalFT = createFunctionTable(10);

	// start reduction of all functions from other function and main function
	root->children[0] = reduceOtherFunctions(root->children[0]);
	root->children[1] = reduceMainFunction(root->children[1]);

	if (root->children[0])
		root->children[0]->parent = root;

	root->children[1]->parent = root;
	root->curr_children = 2;

	checkForDuplicates();

	curr_number_backup = curr_number;
	curr_number = 0;

	curr_global_offset_backup = curr_global_offset;
	curr_global_offset = 0;

	return root;
}

treeNode* reduceMainFunction(treeNode* mainFuncNode) {

	// No sem_error checking as Main function always present
	treeNode* mainFuncNode_backup = mainFuncNode;
	mainFuncNode->st = createSymbolTable(20);
	strcpy(mainFuncNode->st->scope, "main");

	// add main function to allST all symbol table
	if (curr_number < curr_max) {
		allST[curr_number++] = mainFuncNode->st;
	} else {
		curr_max *= 2;
		allST = (SymbolTable**)realloc(allST, sizeof(SymbolTable) * curr_max);
		allST[curr_number++] = mainFuncNode->st;
	}

	copySymbolTableToChildren(mainFuncNode);

	mainFuncNode = reduceStmtsNode(mainFuncNode->children[1]);

	free(mainFuncNode_backup->children[0]);
	free(mainFuncNode_backup->children[2]);
	free(mainFuncNode_backup);
	insertFunction(globalFT, "main", 4, NULL, NULL, NULL, NULL, 0, 0);

	return mainFuncNode;
}
// reduce all programmed functions other than main
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

/*
* Reduce function, called from otherFunction to reduce all functions
*/
treeNode* reduceFunction(treeNode* funcNode) {
	funcNode->symbol_type = funcNode->children[0]->symbol_type;
	funcNode->tk_info = funcNode->children[0]->tk_info;
	funcNode->st = createSymbolTable(20);
	strcpy(funcNode->st->scope, funcNode->tk_info.lexeme);

	copySymbolTableToChildren(funcNode);

	if (curr_number < curr_max) {
		allST[curr_number++] = funcNode->st;
	} else {
		curr_max *= 2;
		allST = (SymbolTable**)realloc(allST, sizeof(SymbolTable) * curr_max);
		allST[curr_number++] = funcNode->st;
	}

	// now make the first child as input parameter list
	funcNode->children[0] = reduceInputPar(funcNode->children[1]);

	// now make the 2nd child as output parameter list
	funcNode->children[1] = reduceOutputPar(funcNode->children[2]);

	// now make the 3nd child as stmts list
	funcNode->children[2] = reduceStmtsNode(funcNode->children[4]);

	// insert() into Function Table

	// also check if returned ids' lexemes and types match
	// the ones declared in output parameters
	int id_len = strlen(funcNode->tk_info.lexeme);
	char *id = (char*)malloc(id_len*sizeof(char));
	strcpy(id, funcNode->tk_info.lexeme);

	int input_len = funcNode->children[0]->curr_children;
	int output_len;

	if (funcNode->children[1]) {
		output_len = funcNode->children[1]->curr_children;
	} else {
		output_len = 0;
	}
	int i = 0;

	// count will be used later for semantic checking
	int count;
	int children_count = funcNode->children[2]->curr_children;

	if( !funcNode->children[2]->children[children_count-1]) {
		count = 0;
	}
	else{
		count = funcNode->children[2]->children[children_count-1]->curr_children;
	}

	// create a new symbol table for each function

	char **input_types = (char**)malloc(input_len*sizeof(char*));
	char **output_types = (char**)malloc(output_len*sizeof(char*));
	char **input_ids = (char**)malloc(input_len*sizeof(char*));
	char **output_ids = (char**)malloc(output_len*sizeof(char*));

	for (i = 0; i < input_len; ++i) {
		int len = strlen(funcNode->children[0]->children[i]->tk_info.lexeme);

		if (i % 2 == 0) {
			input_types[i/2] = (char*)malloc((len+1)*sizeof(char));
			memset(input_types[i/2], '\0', len+1);
			strcpy(input_types[i/2], funcNode->children[0]->children[i]->tk_info.lexeme);
		} else {
			input_ids[i/2] = (char*)malloc(sizeof(char)*(len+1));
			memset(input_ids[i/2], '\0', len+1);
			strcpy(input_ids[i/2], funcNode->children[0]->children[i]->tk_info.lexeme);

			// insert all relevant symbols into the symbol table for this function
			insertSymbol(funcNode->st, funcNode->children[0]->children[i]->tk_info.lexeme,
				strlen(funcNode->children[0]->children[i]->tk_info.lexeme),
				funcNode->children[0]->children[i-1]->tk_info.lexeme,
				&curr_global_offset
			);
		}
	}
	for (i = 0; i < output_len; ++i) {
		int len = strlen(funcNode->children[1]->children[i]->tk_info.lexeme);

		if (i % 2 == 0) {
			output_types[i/2] = (char*)malloc((len+1)*sizeof(char));
			memset(output_types[i/2], '\0', len+1);
			strcpy(output_types[i/2], funcNode->children[1]->children[i]->tk_info.lexeme);

		} else{
			output_ids[i/2] = (char*)malloc(sizeof(char)*(len+1));
			memset(output_ids[i/2], '\0', len+1);
			strcpy(output_ids[i/2], funcNode->children[1]->children[i]->tk_info.lexeme);

			// insert symbol with type in Local ST
			insertSymbol(funcNode->st, funcNode->children[1]->children[i]->tk_info.lexeme,
				strlen(funcNode->children[1]->children[i]->tk_info.lexeme),
				funcNode->children[1]->children[i-1]->tk_info.lexeme,
				&curr_global_offset
			);
		}

	}

	// semantic error checking between return and output params list
	if (count != (output_len/2)) {
		fprintf(stderr,
			"*** ERROR: Return list (%d elements) and Output parameter list"
			" (%d elements) have unequal number of elements\n",
			count, output_len
		);
		sem_error++;
	} else {
		for (i = 0; i < count; ++i) {
			if(strcmp(output_ids[i], funcNode->children[2]->children[children_count-1]->children[i]->tk_info.lexeme) !=0 ) {
				fprintf(stderr, "*** ERROR: Output parameter list and return list have different ids\n");
				sem_error++;
			}
			symbolTableElem* looked_up = lookupSymbol(
				funcNode->st,
				funcNode->children[2]->children[children_count-1]->children[i]->tk_info.lexeme,
				strlen(funcNode->children[2]->children[children_count-1]->children[i]->tk_info.lexeme)
			);

			if (looked_up == NULL) {
				looked_up = lookupSymbol(
					globalST,
					funcNode->children[2]->children[children_count-1]->children[i]->tk_info.lexeme,
					strlen(funcNode->children[2]->children[children_count-1]->children[i]->tk_info.lexeme)
				);
			}

			if (strcmp(output_types[i], looked_up->type) !=0 ) {
				fprintf(stderr, "*** ERROR: Output parameter list and return list have different data types\n");
				sem_error++;
			}
		}
	}
	
	// insert function into the globalFT table 

	insertFunction(globalFT, id, id_len, input_types, output_types, input_ids, output_ids, input_len/2, output_len/2);
	funcNode->children[0]->parent = funcNode;

	// if returned value is not NULL
	if (funcNode->children[1]) {
		funcNode->children[1]->parent = funcNode;
	}

	// TK: check if this is redundant
	if (funcNode->children[2])
		funcNode->children[2]->parent = funcNode;

	funcNode->curr_children = 3;

	return funcNode;
}

/*
* reduce input parameter list and free nodes
*/

treeNode* reduceInputPar(treeNode* inputParNode) {
	treeNode* back_up = inputParNode;

	// remove unnecesarry nodes
	free(inputParNode->children[0]);
	free(inputParNode->children[1]);
	free(inputParNode->children[2]);
	free(inputParNode->children[3]);
	free(inputParNode->children[5]);

	inputParNode = reduceParameterList(inputParNode->children[4]);
	free(back_up);

	return inputParNode;
}

/*
* reduce output parameter list and free nodes
*/
treeNode* reduceOutputPar(treeNode* outputParNode) {
	treeNode* back_up = outputParNode;

	// remove unnecesarry nodes
	free(outputParNode->children[0]);
	free(outputParNode->children[1]);
	free(outputParNode->children[2]);
	free(outputParNode->children[3]);
	free(outputParNode->children[5]);

	if (outputParNode->children[0]->symbol_type == eps
		|| outputParNode->children[4]->symbol_type == eps
	) {
		// free(outputParNode->children[4]);
		// free(back_up);
		return NULL;
	} else {
		outputParNode = reduceParameterList(outputParNode->children[4]);
	}
	free(back_up);
	return outputParNode;
}

/*
* reduce parameter list, called from both input and output
*/
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

			remainList = remainList->children[2];
		}
		j++;
		free(remainList_backup);
	}

	orig->curr_children = i;
	return orig;
}

/*
* reduce datatype and update nodes, called from param lists
*/
treeNode* reduceDatatype(treeNode* datatypeNode) {
	treeNode* datatypeNode_backup = datatypeNode;

	if (datatypeNode->symbol_type == eps) {
		return NULL;
	}

	if (datatypeNode->children[0]->symbol_type == primitiveDatatype)
		datatypeNode = datatypeNode->children[0]->children[0];
	else
		datatypeNode = datatypeNode->children[0]->children[1];

	datatypeNode->parent = datatypeNode_backup->parent;
	datatypeNode->curr_children = 0;

	// free(datatypeNode_backup->children[0]);
	free(datatypeNode_backup);

	return datatypeNode;
}

/*
* reduce different type of statements and update relevant nodes
*/

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

/*
* reduce type definition, e.g. record
*/
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
		free(defns_backup);
	}

	orig->curr_children = i;
	return orig;
}

/*
* helper function to type defintions, reduce one type at a time
*/
treeNode* reduceTypeDefn(treeNode* orig) {
	copySymbolTableToChildren(orig);

	free(orig->children[0]);
	orig->children[0] = orig->children[1];

	orig->children[1] = reduceFieldDefns(orig->children[2]);

	free(orig->children[3]);
	free(orig->children[4]);

	orig->curr_children = 2;

	return orig;
}

/*
* reduce field definitions, iterate over all
*/
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

	strcpy(field_names[0], orig->children[0]->children[1]->tk_info.lexeme);
	strcpy(field_types[0], orig->children[0]->children[0]->tk_info.lexeme);

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

	strcpy(field_names[1], orig->children[1]->children[1]->tk_info.lexeme);
	strcpy(field_types[1], orig->children[1]->children[0]->tk_info.lexeme);

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

		// prepare information to store in type table
		field_names[i] = (char* )malloc(25*sizeof(char));
        memset(field_names[i], '\0', 25);
		strcpy(field_names[i], orig->children[i]->children[1]->tk_info.lexeme);

		field_types[i] = (char* )malloc(25*sizeof(char));
        memset(field_types[i], '\0', 25);
		strcpy(field_types[i], orig->children[i]->children[0]->tk_info.lexeme);

		if (orig->children[i]->children[0]->symbol_type == TK_REAL) {
			width[i] = REAL_WIDTH;
		} else{
			width[i] = INT_WIDTH;
		}
		offset[i] = offset[i-1] + width[i-1];

		defns = defns->children[1];
		free(defns_backup);
		i++;
	}
	orig->curr_children = i;
	char* type = (char*) malloc(25*sizeof(char));
	strcpy(type, orig->parent->children[0]->tk_info.lexeme);

	// insert type in global type table
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

/*
* reduces one field defn at a time and update relevant node
*/
treeNode* reduceFieldDefn(treeNode* orig) {
	copySymbolTableToChildren(orig);

	free(orig->children[0]);
	orig->children[0] = orig->children[1]->children[0];

	orig->children[1] = orig->children[3];

	free(orig->children[2]);
	free(orig->children[4]);

	orig->curr_children = 2;

	return orig;
}
/*
* reduce all declarations
*/
treeNode* reduceDeclarations(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		// free(orig->children[0]);
		free(orig);
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
		free(decns_backup);
	}

	orig->curr_children = i;

	int k;
	for (k = 0; k < i; ++k){
		treeNode* declarationNode = orig->children[k];

		if (declarationNode->curr_children > 2) {
			// insert in global symbol table

			int type_len = strlen(declarationNode->children[0]->tk_info.lexeme);
			char *type = (char*)malloc(type_len * sizeof(char));

			strcpy(type, declarationNode->children[0]->tk_info.lexeme);

			insertSymbol(globalST, declarationNode->children[1]->tk_info.lexeme,
				strlen(declarationNode->children[1]->tk_info.lexeme), type,
				&curr_global_offset
			);
			free(type);
		}
		else{
			int id_len = strlen(declarationNode->children[1]->tk_info.lexeme);
			char *id = (char*) malloc(id_len * sizeof(char));

			strcpy(id, declarationNode->children[1]->tk_info.lexeme);
			// check if exists in global
			if (lookupSymbol(globalST, id, id_len) == NULL) {
				// if does not exist in globalST, insert the symbol in local
				int type_len = strlen(declarationNode->children[0]->tk_info.lexeme);
				char *type = (char*)malloc(type_len * sizeof(char));

				strcpy(type, declarationNode->children[0]->tk_info.lexeme);

				insertSymbol(orig->st, id, id_len, type, &curr_global_offset);
				free(type);
			}
			else {
				fprintf(stderr, "*** ERROR: %s exists in global symbol table\n", id);
				sem_error++;
			}
			free(id);
		}
	}

	return orig;
}
/*
* reduce each declaration and update the nodes
*/
treeNode* reduceDeclaration(treeNode* orig) {
	free(orig->children[2]);
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
		free(orig->children[4]->children[0]);
		free(orig->children[4]);
	} else {
		orig->children[2] = orig->children[4]->children[1];
		orig->children[2]->parent = orig;
		orig->curr_children = 3;
	}

	return orig;
}
/*
* reduce other statements
*/
treeNode* reduceOtherStmts(treeNode* orig) {
	treeNode* stmts = orig;
	if (orig->children[0]->symbol_type == eps) {
		free(orig->children[0]);
		free(orig);
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
		free(stmts_backup);
	}

	orig->curr_children = i;
	return orig;
}
/*
* reduce each statement, called by reduceOtherStmts()
*/
treeNode* reduceStatement(treeNode* orig) {
	treeNode* backup = orig;

	// printf("%d : %d\n", orig->symbol_type, orig->curr_children, orig->st->size);
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
	free(backup);
	return orig;
}
/*
* reduce return statements
*/
treeNode* reduceReturnStmt(treeNode* orig) {
	copySymbolTableToChildren(orig);

	free(orig->children[0]);
	free(orig->children[2]);

	if (orig->children[1]->symbol_type == eps) {
		return NULL;
	} else {
		treeNode* backup = reduceIdList(orig->children[1]->children[1]);
		// if (backup)
		// 	backup->curr_children = 1;
		return backup;
	}
}
/*
* reduce assignment statements
*/
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
	free(backup);

	return orig;
}

/*
* reduce iterative statement and update nodes
*/
treeNode* reduceIterativeStmt(treeNode* orig) {
	copySymbolTableToChildren(orig);

	free(orig->children[0]);
	free(orig->children[1]);
	free(orig->children[3]);
	free(orig->children[6]);
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

/*
* reduce conditional statement & update nodes
*/
treeNode* reduceConditionalStmt(treeNode* orig) {
	copySymbolTableToChildren(orig);

	free(orig->children[0]);
	free(orig->children[1]);
	orig->children[0] = reduceBooleanExpr(orig->children[2]);
	orig->children[1] = reduceStatement(orig->children[5]);
	orig->children[2] = reduceOtherStmts(orig->children[6]);

	if (orig->children[0]) {
		orig->children[0]->parent = orig;
		orig->children[0]->st = orig->st;
	}
	if (orig->children[1]) {
		orig->children[1]->parent = orig;
		orig->children[1]->st = orig->st;
	}
	if (orig->children[2]) {
		orig->children[2]->parent = orig;
		orig->children[2]->st = orig->st;
	}


	if (orig->children[7]->children[0]->symbol_type == TK_ENDIF) {
		orig->curr_children = 3;
	} else {
		copySymbolTableToChildren(orig->children[7]);
		orig->children[3] = orig->children[7]->children[0];
		orig->children[3]->parent = orig;
		orig->children[3]->st = orig->st;

		orig->children[4] = reduceStatement(orig->children[7]->children[1]);
		orig->children[5] = reduceOtherStmts(orig->children[7]->children[2]);

		if (orig->children[4]) {
			orig->children[4]->parent = orig;
			orig->children[4]->st = orig->st;
		}
		if (orig->children[5]) {
			orig->children[5]->parent = orig;
			orig->children[5]->st = orig->st;
		}
		orig->curr_children = 6;
	}

	return orig;
}

/*
* reduce IO statements and update relevant nodes
*/
treeNode* reduceIoStmt(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);

	if (orig->children[0]->symbol_type == TK_READ) {
		free(orig->children[1]);
		orig = orig->children[0];
		copySymbolTableToChildren(orig);
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		orig->children[0] = reduceSingleOrRecId(backup->children[2]);
		orig->children[0]->parent = orig;
		orig->children[0]->st = backup->st;
		orig->curr_children = 1;
	} else {
		free(orig->children[1]);

		orig = orig->children[0];
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));
		orig->children[0] = reduceAllVar(backup->children[2]);
		orig->children[0]->st = backup->st;

		orig->curr_children = 1;
	}
	if (orig->children[0])
		orig->children[0]->parent = orig;

	orig->parent = backup->parent;
	orig->curr_children = 1;
	orig->st = backup->st;

	return orig;
}

/*
* reduce function call statement
*/
treeNode* reduceFunCallStmt(treeNode* orig) {
	copySymbolTableToChildren(orig);
	treeNode* backup = orig;

	orig = orig->children[2];
	orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

	orig->children[0] = reduceOutputParameters(backup->children[0]);
	orig->children[1] = reduceInputParameters(backup->children[5]);

	free(backup->children[1]);
	free(backup->children[3]);
	free(backup->children[4]);

	// Perform some semantic analysis here
	functionTableElem *func = lookupFunction(
		globalFT, orig->tk_info.lexeme,
		strlen(orig->tk_info.lexeme)
	);

	// if Undeclared previously
	if (func == NULL) {
		fprintf(stderr,
			"*** ERROR: The function %s called at line no. %d is previously undeclared!\n",
			orig->tk_info.lexeme, orig->tk_info.line_no
		);
		sem_error++;
	} else {
		int i;
		symbolTableElem* lookup = NULL;

		// check input parameter list 
		for (i = 0; i < func->input_len && i < orig->children[1]->curr_children; i++) {
			lookup = lookupSymbol(
				orig->st,
				orig->children[1]->children[i]->tk_info.lexeme,
				strlen(orig->children[1]->children[i]->tk_info.lexeme)
			);

			// if not found, look in Global Symbol Table
			if (lookup == NULL) {
				lookup = lookupSymbol(
					globalST,
					orig->children[1]->children[i]->tk_info.lexeme,
					strlen(orig->children[1]->children[i]->tk_info.lexeme)
				);
			}

			// else if not found still, it is undeclared
			if (lookup == NULL) {
				fprintf(
					stderr,
					"*** ERROR: The symbol %s is not declared before it use on line %d 2\n",
					orig->children[1]->children[i]->tk_info.lexeme,
					orig->children[1]->children[i]->tk_info.line_no
				);
				continue;
			}

			if (strcmp(
				func->input_types[i], lookup->type) != 0
			) {
				fprintf(stderr,
					"*** ERROR: The input argument %s does NOT match the expected parameter type '%s' on line %d\n",
					orig->children[1]->children[i]->tk_info.lexeme,
					func->input_types[i],
					orig->children[1]->children[i]->tk_info.line_no
				);
				sem_error++;
			}
		}

		if (func->input_len != orig->children[1]->curr_children) {
			fprintf(stderr,
				"*** ERROR: The function %s expects %d parameters, %d passed on line %d\n",
				func->id, func->input_len, orig->children[1]->curr_children,
				orig->tk_info.line_no
			);
			sem_error++;
		}

		// check output parameter list
		for (i = 0; i < func->output_len && orig->children[0] && i < orig->children[0]->curr_children; i = i + 1) {
			lookup = lookupSymbol(
				orig->st,
				orig->children[0]->children[i]->tk_info.lexeme,
				strlen(orig->children[0]->children[i]->tk_info.lexeme)
			);

			// if not found, look in Global Symbol Table
			if (lookup == NULL) {
				lookup = lookupSymbol(
					globalST,
					orig->children[0]->children[i]->tk_info.lexeme,
					strlen(orig->children[0]->children[i]->tk_info.lexeme)
				);
			}

			// else if not found still, it is undeclared
			if (lookup == NULL) {
				fprintf(
					stderr,
					"*** ERROR: The symbol %s is not declared before it use on line %d\n",
					orig->children[0]->children[i]->tk_info.lexeme,
					orig->children[0]->children[i]->tk_info.line_no
				);
				continue;
			}

			if (strcmp(
				func->output_types[i], lookup->type) != 0
			) {
				fprintf(stderr,
					"*** ERROR: The output argument %s does NOT match the expected parameter type '%s' on line %d\n",
					orig->children[0]->children[i]->tk_info.lexeme,
					func->output_types[i/2],
					orig->children[0]->children[i]->tk_info.line_no
				);
				sem_error++;
			}
		}

		// check for semantic error
		if (orig->children[0] && func->output_len != orig->children[0]->curr_children) {
			fprintf(stderr,
				"*** ERROR: The function %s returns %d parameters, %d catched on line %d\n",
				func->id, func->output_len, orig->children[0]->curr_children,
				orig->tk_info.line_no
			);
			sem_error++;
		} else if (orig->children[0] == NULL) {
			fprintf(stderr,
				"*** ERROR: The function %s returns %d parameters, %d catched on line %d\n",
				func->id, func->output_len, 0,
				orig->tk_info.line_no
			);
			sem_error++;
		}
	}

	if (orig->children[0])
		orig->children[0]->parent = orig;
	if (orig->children[1])
		orig->children[1]->parent = orig;

	orig->parent = backup->parent;
	orig->curr_children = 2;
	return orig;
}

/*
* reduce output parameter from the list
*/
treeNode* reduceOutputParameters(treeNode* orig) {
	treeNode* backup = orig;
	if (orig->symbol_type == eps) {
		free(orig);
		return NULL;
	} else {
		copySymbolTableToChildren(orig);
		free(orig->children[0]);
		free(orig->children[2]);
		free(orig->children[3]);

		orig = reduceIdList(orig->children[1]);
		if (orig)
			orig->parent = backup->parent;
		free(backup);
	}
	return orig;
}
/*
* reduce input params from the list
*/
treeNode* reduceInputParameters(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);

	free(orig->children[0]);
	free(orig->children[2]);

	orig = reduceIdList(orig->children[1]);
	orig->parent = backup->parent;
	free(backup);

	return orig;
}

/*
* reduce ID list
*/

treeNode* reduceIdList(treeNode* orig) {

	if (orig == NULL || orig->symbol_type == eps) {
		return NULL;
	}
	copySymbolTableToChildren(orig);

	int size = 10;
	orig->children = (treeNode**)realloc(orig->children, 10 * sizeof(treeNode*));

	int i = 1, j = 0;
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

		free(remainList_backup);
	}

	orig->curr_children = i;
	return orig;
}

/*
* reduce both single or record id
*/
treeNode* reduceSingleOrRecId(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);

	if (orig->children[1]->children[0]->symbol_type == eps) {
		free(orig->children[1]);
		orig = orig->children[0];
		orig->st = backup->st;
		orig->parent = backup->parent;
		orig->curr_children = 0;
		free(backup);
	} else {
		orig->children[1] = orig->children[1]->children[1];
		orig->children[1]->parent = orig;
		orig->children[1]->st = backup->st;

		orig->curr_children = 2;
	}

	return orig;
}

/*
* reduce all var
*/
treeNode* reduceAllVar(treeNode* orig) {
	treeNode* backup = orig;

	copySymbolTableToChildren(orig);
	if (orig->children[0]->symbol_type == some_types) {
		orig = orig->children[0];
		orig->parent = backup->parent;
		orig->st = backup->st;
		copySymbolTableToChildren(orig);
	} else {
		// other_types
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		if (orig->children[0]->children[1]->children[0]->symbol_type == eps) {
			orig->children[0] = orig->children[0]->children[0];
			orig->children[0]->parent = orig;

			orig->st = backup->st;
			copySymbolTableToChildren(orig);
			// printf("Here size %d, %s\n", orig->st->size, orig->children[0]->tk_info.lexeme);

			orig->curr_children = 1;
		} else {
			orig->children[1] = backup->children[0]->children[1]->children[1];
			orig->children[0] = orig->children[0]->children[0];

			orig->children[0]->parent = orig;
			orig->children[1]->parent = orig;
			orig->parent = backup->parent;

			orig->st = backup->st;
			// printf("Here size %d, %s\n", orig->st->size, orig->children[0]->tk_info.lexeme);
			copySymbolTableToChildren(orig);

			free(backup);
			orig->curr_children = 2;
		}
	}

	return orig;
}

/*
* reduce pull up
*/
treeNode* pullUp(treeNode* left_child, treeNode* right_child) {

	if (right_child == NULL) {
		return NULL;
	} else if (right_child->children[1] == NULL || right_child->children[1]->symbol_type == eps) {
		right_child->children[1] = right_child->children[0];
		right_child->children[0] = left_child;
		right_child->curr_children = 2;
		return right_child;
	} else {
		right_child->children[1] = pullUp(right_child->children[0], right_child->children[1]);
		right_child->children[0] = left_child;
		right_child->curr_children = 2;
		return right_child;
	}
}

/*
* reduce artihmetic expression
*/
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
		copySymbolTableToChildren(orig);
		orig->parent = backup->parent;
	} else {

		// orig = orig->children[1];
		orig = pullUp(orig->children[0], orig->children[1]);
		orig->children = (treeNode**)realloc(orig->children, sizeof(treeNode*) * 2);

		// orig->children[1] = orig->children[0];
		// orig->children[0] = backup->children[0];
		orig->children[0]->parent = orig;
		orig->children[1]->parent = orig;
		// orig->children[1]->parent = orig;
		orig->curr_children = 2;
		return orig;
	}

	free(backup);
	orig->st = backup->st;
	copySymbolTableToChildren(orig);
	return orig;
}

/*
* reduce expression 
*/
treeNode* reduceExpPrime(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
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

		free(backup);
		return orig;
	}
}

/*
* reduce each term in an expression
*/
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

	free(backup);
	return orig;
}

/*
* reduce term prime
*/
treeNode* reduceTermPrime(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		free(orig->children[0]);
		free(orig);
		return NULL;
	} else {
		treeNode* backup = orig;
		copySymbolTableToChildren(orig);
		orig = orig->children[0]->children[0];

		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));
		copySymbolTableToChildren(orig);

		// call to reduce both children and update here
		orig->children[0] = reduceFactor(backup->children[1]);
		orig->children[1] = reduceTermPrime(backup->children[2]);

		orig->children[0]->parent = orig;

		if (orig->children[1]) {
			orig->children[1]->parent = orig;
		}

		orig->curr_children = 2;

		free(backup);
		return orig;
	}
}

/*
* reduce factor
*/
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
		free(backup);
	}

	return orig;
}

/*
* reduce all node
*/
treeNode* reduceAll(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);
	if (orig->children[0]->symbol_type == TK_ID) {
		// update tree to AST nodes from parse tree nodes
		if (orig->children[1]->children[0]->symbol_type == eps) {
			free(orig->children[1]->children[0]);
			free(orig->children[1]);
			orig->children[1] = NULL;
			orig = orig->children[0];
			orig->parent = backup->parent;
			orig->st = backup->st;
			free(backup);
		} else {
			backup = orig->children[1];
			orig->children[1] = orig->children[1]->children[1];
			orig->children[1]->parent = orig;
			free(backup);
			orig->children[1]->st = backup->st;
			orig->curr_children = 2;
		}
	} else {
		orig = orig->children[0];
		orig->st = backup->st;
		free(backup);
	}

	return orig;
}

/*
* reduce boolean expression
*/
treeNode* reduceBooleanExpr(treeNode* orig) {
	treeNode* backup = orig;
	copySymbolTableToChildren(orig);

	if (orig->children[0]->symbol_type == TK_OP) {
		orig = orig->children[3]->children[0];
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		// recursive call to reduce boolean expression
		orig->children[0] = reduceBooleanExpr(backup->children[1]);
		orig->children[0]->parent = orig;
		free(backup->children[0]);
		free(backup->children[2]);
		free(backup->children[4]);
		free(backup->children[6]);
		// recursive call to reduce boolean expression
		orig->children[1] = reduceBooleanExpr(backup->children[5]);
		if (orig->children[1])
			orig->children[1]->parent = orig;

		orig->st = backup->st;
		orig->curr_children = 2;

		copySymbolTableToChildren(orig);
	} else if (orig->children[0]->symbol_type == TK_NOT) {
		orig = orig->children[0];
		orig->children = (treeNode**)realloc(orig->children, 1 * sizeof(treeNode*));

		// recursive call to reduce boolean expression
		orig->children[0] = reduceBooleanExpr(backup->children[2]);
		orig->children[0]->parent = orig;

		orig->curr_children = 1;

		free(backup->children[1]);
		free(backup->children[3]);
	} else {
		orig = orig->children[1]->children[0];
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));
		orig->children[0] = backup->children[0]->children[0];
		orig->children[1] = backup->children[2]->children[0];

		orig->children[0]->parent = orig;
		orig->children[1]->parent = orig;
		orig->parent = backup->parent;
		orig->st = backup->st;

		free(backup->children[0]);
		free(backup->children[1]);
		free(backup->children[2]);
		orig->curr_children = 2;
		copySymbolTableToChildren(orig);
	}

	orig->st = backup->st;
	copySymbolTableToChildren(orig);
	return orig;
}

/*
 * For every symbol in GlobalST, look it up in every other ST
 *   If found, give an sem_error
 *   Else Move ahead
 *	Helper function
 */
int checkForDuplicates() {
	int i, j;

	for (i = 0; i < globalST->size; ++i) {
        if (globalST->symbolArray[i] != NULL) {
            symbolTableElem* curr = globalST->symbolArray[i];

            while (curr != NULL) {
	            for (j = 0; j < curr_number; j++) {
	            	if (lookupSymbol(allST[j], curr->lexeme, strlen(curr->lexeme))) {
	            		fprintf(stderr,
	            			"*** ERROR: The Symbol %s is declared globally and later redeclared inside a function.\n",
	            			curr->lexeme
	            		);
	            		sem_error++;
	            		return -1;
	            	}
	            }
                curr = curr->next;
            }
        }
    }

    return 0;
}
