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

treeNode* createAST(treeNode *parseTreeRoot) {
	if (parseTreeRoot == NULL
		&& parseTreeRoot->symbol_type == eps
	) {
		return NULL;
	} else {
		return reduceProgram(parseTreeRoot);
	}
}


treeNode* reduceProgram(treeNode* root) {

	root->children[0] = reduceOtherFunctions(root->children[0]);
	root->children[1] = reduceMainFunction(root->children[1]);

	root->children[0]->parent = root;
	root->children[1]->parent = root;
	root->curr_children = 2;

	return root;
}

treeNode* reduceMainFunction(treeNode* mainFuncNode) {

	// No error checking as Main function always present
	treeNode* mainFuncNode_backup = mainFuncNode;
	mainFuncNode = reduceStmtsNode(mainFuncNode->children[1]);

	free(mainFuncNode_backup->children[0]);
	free(mainFuncNode_backup->children[2]);
	free(mainFuncNode_backup);

	return mainFuncNode;
}

treeNode* reduceOtherFunctions(treeNode* orig) {

	// Might reduce to EPS, so check
	if (orig->children[0]->symbol_type == eps) {
		// free(orig->children[0]);
		// free(orig);
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

	// now make the first child as input paramenter list
	funcNode->children[0] = reduceInputPar(funcNode->children[1]);

	// now make the 2nd child as output paramenter list
	funcNode->children[1] = reduceOutputPar(funcNode->children[2]);

	// now make the 3nd child as stmts list
	funcNode->children[2] = reduceStmtsNode(funcNode->children[4]);

	funcNode->children[0]->parent = funcNode;
	funcNode->children[1]->parent = funcNode;

	// if returned value is not NULL
	if (funcNode->children[1])
		funcNode->children[2]->parent = funcNode;

	funcNode->curr_children = 3;
	return funcNode;
}


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

treeNode* reduceOutputPar(treeNode* outputParNode) {
	treeNode* back_up = outputParNode;

	// remove unnecesarry nodes
	free(outputParNode->children[0]);
	free(outputParNode->children[1]);
	free(outputParNode->children[2]);
	free(outputParNode->children[3]);
	free(outputParNode->children[5]);

	if (outputParNode->children[4]->symbol_type == eps) {
		free(outputParNode->children[4]);
		free(back_up);
		return NULL;
	} else {
		outputParNode = reduceParameterList(outputParNode->children[4]);
		free(back_up);
	}

	return outputParNode;
}


treeNode* reduceParameterList(treeNode* paramListNode) {
	treeNode* orig = paramListNode;

	orig->children[0] = reduceDatatype(orig->children[0]);
	orig->children[0]->parent = orig;

	orig->children = (treeNode**)realloc(orig->children, 10 * sizeof(treeNode*));
	int size = 10;

	int i = 1, j = 1;
	free(orig->children[1]);

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
			orig->children[i-1]->parent = orig;

			free(remainList->children[1]);
			remainList = remainList->children[2];
		}
		j++;
		free(remainList_backup);
	}

	orig->curr_children = i;
	return orig;
}

treeNode* reduceDatatype(treeNode* datatypeNode) {
	treeNode* datatypeNode_backup = datatypeNode;
	datatypeNode = datatypeNode->children[0]->children[0];
	datatypeNode->curr_children = 0;

	free(datatypeNode_backup->children[0]);
	free(datatypeNode_backup);

	return datatypeNode;
}

treeNode* reduceStmtsNode(treeNode* stmtsNode) {

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

	stmtsNode->curr_children = 4;

	return stmtsNode;
}

treeNode* reduceTypeDefns(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		return NULL;
	}

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

		orig->children[i++] = reduceTypeDefn(defns->children[0]);
		defns = defns->children[1];
		free(defns_backup);
	}

	orig->curr_children = i;
	return orig;
}

treeNode* reduceTypeDefn(treeNode* orig) {
	free(orig->children[0]);
	orig->children[0] = orig->children[1];

	orig->children[1] = reduceFieldDefns(orig->children[2]);

	free(orig->children[3]);
	free(orig->children[4]);

	orig->curr_children = 2;

	return orig;
}

treeNode* reduceFieldDefns(treeNode* orig) {
	orig->children[0] = reduceFieldDefn(orig->children[0]);
	orig->children[1] = reduceFieldDefn(orig->children[1]);

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
		}

		orig->children[i++] = reduceFieldDefn(defns->children[0]);
		defns = defns->children[1];
		free(defns_backup);
	}

	orig->curr_children = i;
	return orig;
}

treeNode* reduceFieldDefn(treeNode* orig) {
	free(orig->children[0]);
	orig->children[0] = orig->children[1]->children[0];

	orig->children[1] = orig->children[3];

	free(orig->children[2]);
	free(orig->children[4]);

	orig->curr_children = 2;

	return orig;
}

treeNode* reduceDeclarations(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		return NULL;
	}

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
	return orig;
}

treeNode* reduceDeclaration(treeNode* orig) {
	free(orig->children[2]);
	if (orig->symbol_type == eps) {
		return NULL;
	}

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
		orig->children[2] = orig->children[4]->children[0];
		orig->children[2]->parent = orig;
		orig->curr_children = 3;
	}

	return orig;
}

treeNode* reduceOtherStmts(treeNode* orig) {
	treeNode* stmts = orig;
	if (orig->children[0]->symbol_type == eps) {
		free(orig->children[0]);
		free(orig);
		return NULL;
	}

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

		orig->children[i++] = reduceStatement(stmts->children[0]);

		if (orig->children[i-1])
			orig->children[i-1]->parent = orig;

		stmts = stmts->children[1];
		free(stmts_backup);
	}

	orig->curr_children = i;
	return orig;
}

treeNode* reduceStatement(treeNode* orig) {
	treeNode* backup = orig;

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

treeNode* reduceReturnStmt(treeNode* orig) {
	free(orig->children[0]);
	free(orig->children[2]);

	if (orig->children[1]->symbol_type == eps) {
		return NULL;
	} else {
		return reduceIdList(orig->children[1]);
	}
}

treeNode* reduceAssignStmt(treeNode* orig) {
	treeNode* backup = orig;
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

treeNode* reduceIterativeStmt(treeNode* orig) {
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

treeNode* reduceConditionalStmt(treeNode* orig) {
	free(orig->children[0]);
	free(orig->children[1]);
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

	if (orig->children[0]->symbol_type == TK_READ) {
		free(orig->children[1]);
		orig = orig->children[0];
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		orig->children[0] = reduceSingleOrRecId(backup->children[2]);
		orig->curr_children = 1;
	} else {
		free(orig->children[1]);

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
	orig->children[0] = reduceOutputParameters(orig->children[0]);
	free(orig->children[1]);

	orig->children[1] = orig->children[2];

	free(orig->children[3]);
	free(orig->children[4]);
	orig->children[2] = reduceInputParameters(orig->children[5]);

	if (orig->children[0])
		orig->children[0]->parent = orig;
	if (orig->children[1])
		orig->children[1]->parent = orig;
	if (orig->children[2])
		orig->children[2]->parent = orig;

	orig->curr_children = 3;
	return orig;
}

treeNode* reduceOutputParameters(treeNode* orig) {
	treeNode* backup = orig;
	if (orig->symbol_type == eps) {
		free(orig);
		return NULL;
	} else {
		free(orig->children[0]);
		free(orig->children[2]);
		free(orig->children[3]);

		orig = reduceIdList(orig->children[1]);
		orig->parent = backup->parent;
		free(backup);
	}
	return orig;
}

treeNode* reduceInputParameters(treeNode* orig) {
	treeNode* backup = orig;
	free(orig->children[0]);
	free(orig->children[2]);

	orig = reduceIdList(orig->children[1]);
	orig->parent = backup->parent;
	free(backup);

	return orig;
}

treeNode* reduceIdList(treeNode* orig) {

	if (orig->children[0]->symbol_type == eps) {
		return NULL;
	}

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

treeNode* reduceSingleOrRecId(treeNode* orig) {
	treeNode* backup = orig;
	if (orig->children[1]->children[0]->symbol_type == eps) {
		free(orig->children[1]);
		orig = orig->children[0];
		orig->parent = backup->parent;
		orig->curr_children = 0;
		free(backup);
	} else {
		orig->children[1] = orig->children[1]->children[1];
		orig->children[1]->parent = orig;

		orig->curr_children = 2;
	}

	return orig;
}

treeNode* reduceAllVar(treeNode* orig) {
	treeNode* backup = orig;
	printf("%d\n", backup->parent->symbol_type);

	if (orig->children[0]->symbol_type == some_types) {
		orig = orig->children[0]->children[0];
		orig->parent = backup->parent;
		// free(backup);
		// free(backup->children[0]);
	} else {
		// other_types
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		if (orig->children[0]->children[1]->children[0]->symbol_type == eps) {
			orig->children[0] = orig->children[0]->children[0];
			orig->children[0]->parent = orig;
			orig->curr_children = 1;
			// free(backup);
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
	treeNode* backup = orig;
	orig->children[0] = reduceTerm(orig->children[0]);
	orig->children[1] = reduceExpPrime(orig->children[1]);

	if (orig->children[0])
			orig->children[0]->parent = orig;
	if (orig->children[1])
			orig->children[1]->parent = orig;

	if (orig->children[1] == NULL) {
		orig = orig->children[0];
	} else {
		orig = orig->children[1];
	}

	free(backup);
	return orig;
}

treeNode* reduceExpPrime(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		free(orig->children[0]);
		free(orig);
		return NULL;
	} else {
		treeNode* backup = orig;
		orig = orig->children[0]->children[0];

		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));
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

treeNode* reduceTerm(treeNode* orig) {
	treeNode* backup = orig;
	orig->children[0] = reduceFactor(orig->children[0]);
	orig->children[1] = reduceTermPrime(orig->children[1]);

	if (orig->children[0])
		orig->children[0]->parent = orig;
	if (orig->children[1])
		orig->children[1]->parent = orig;

	if (orig->children[1] == NULL) {
		orig = orig->children[0];
	} else {
		orig = orig->children[1];
	}

	free(backup);
	return backup;
}

treeNode* reduceTermPrime(treeNode* orig) {
	if (orig->children[0]->symbol_type == eps) {
		free(orig->children[0]);
		free(orig);
		return NULL;
	} else {
		treeNode* backup = orig;
		orig = orig->children[0];
		orig->children[0] = reduceFactor(backup->children[1]);
		orig->children[1] = reduceTermPrime(backup->children[2]);

		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		orig->children[0]->parent = orig;

		if (orig->children[1]) {
			orig->children[1]->parent = orig;
		}

		orig->curr_children = 2;

		free(backup);
		return orig;
	}
}

treeNode* reduceFactor(treeNode* orig) {
	treeNode* backup = orig;
	if (orig->children[0]->symbol_type == all) {
		orig = reduceAll(orig->children[0]);
		orig->parent = backup->parent;
	} else {
		orig = reduceArithmeticExpr(orig->children[1]);
		orig->parent = backup->parent;
		free(backup);
	}

	return orig;
}

treeNode* reduceAll(treeNode* orig) {
	treeNode* backup = orig;

	if (orig->children[0]->symbol_type == TK_ID) {
		if (orig->children[1]->children[0]->symbol_type == eps) {
			free(orig->children[1]->children[0]);
			free(orig->children[1]);
			orig->children[1] = NULL;
			orig = orig->children[0];
			orig->parent = backup->parent;
			free(backup);
		} else {
			backup = orig->children[1];
			orig->children[1] = orig->children[1]->children[1];
			orig->children[1]->parent = orig;
			free(backup);
			orig->curr_children = 2;
		}
	} else {
		orig = orig->children[0];
		free(backup);
	}

	return orig;
}

treeNode* reduceBooleanExpr(treeNode* orig) {
	treeNode* backup = orig;

	if (orig->children[0]->symbol_type == TK_OP) {
		orig = orig->children[3];
		orig->children = (treeNode**)realloc(orig->children, 2 * sizeof(treeNode*));

		orig->children[0] = reduceBooleanExpr(backup->children[1]);
		orig->children[0]->parent = orig;
		free(backup->children[0]);
		free(backup->children[2]);
		free(backup->children[4]);
		free(backup->children[6]);
		orig->children[1] = reduceBooleanExpr(backup->children[5]);

		orig->curr_children = 2;
	} else if (orig->children[0]->symbol_type == TK_NOT) {
		orig = orig->children[0];
		orig->children = (treeNode**)realloc(orig->children, 1 * sizeof(treeNode*));

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

		free(backup->children[0]);
		free(backup->children[1]);
		free(backup->children[2]);
		orig->curr_children = 2;
	}

	return orig;
}