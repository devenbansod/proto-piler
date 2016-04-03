/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef AST_INTERFACE
#define AST_INTERFACE

#include "treeDef.h"

treeNode* createAST(treeNode *parseTreeRoot);

treeNode* reduceProgram(treeNode* orig);

treeNode* reduceMainFunction(treeNode* orig);
treeNode* reduceOtherFunctions(treeNode* orig);
treeNode* reduceFunction(treeNode* orig);

treeNode* reduceInputPar(treeNode* orig);
treeNode* reduceOutputPar(treeNode* orig);
treeNode* reduceParameterList(treeNode* orig);
treeNode* reduceDatatype(treeNode* orig);

treeNode* reduceStmtsNode(treeNode* orig);
treeNode* reduceTypeDefns(treeNode* orig);
treeNode* reduceTypeDefn(treeNode* orig);

treeNode* reduceFieldDefns(treeNode* orig);
treeNode* reduceFieldDefn(treeNode* orig);
treeNode* reduceMoreFields(treeNode* orig);
treeNode* reduceDeclarations(treeNode* orig);
treeNode* reduceDeclaration(treeNode* orig);
treeNode* reduceGlobalOrNot(treeNode* orig);

treeNode* reduceOtherStmts(treeNode* orig);
treeNode* reduceStatement(treeNode* orig);
treeNode* reduceAssignStmt(treeNode* orig);
treeNode* reduceIterativeStmt(treeNode* orig);
treeNode* reduceConditionalStmt(treeNode* orig);
treeNode* reduceFunCallStmt(treeNode* orig);
treeNode* reduceIoStmt(treeNode* orig);
treeNode* reduceReturnStmt(treeNode* orig);
treeNode* reduceIdList(treeNode* orig);

treeNode* reduceSingleOrRecId(treeNode* orig);
treeNode* reduceOutputParameters(treeNode* orig);
treeNode* reduceInputParameters(treeNode* orig);
treeNode* reduceOtherStmts(treeNode* orig);

treeNode* reduceAllVar(treeNode* orig);
treeNode* reduceArithmeticExpr(treeNode* orig);
treeNode* reduceTerm(treeNode* orig);
treeNode* reduceFactor(treeNode* orig);
treeNode* reduceTermPrime(treeNode* orig);
treeNode* reduceFactorPrime(treeNode* orig);
treeNode* reduceExpPrime(treeNode* orig);
treeNode* reduceAll(treeNode* orig);
treeNode* reduceBooleanExpr(treeNode* orig);


#endif
