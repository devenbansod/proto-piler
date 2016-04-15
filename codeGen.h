/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef CODEGEN_INTERFACE
#define CODEGEN_INTERFACE

#include "common.h"
#include "treeDef.h"
#include "AST.h"
#include "symbolTable.h"

#define BASE_ADDR 100

/*
 * Get a new Temporary variable,
 * insert it into Symbol Table,
 * fill new_temp
 *
 */
int getNewTemporary(char* type, int len, char* new_temp);

int generateArithmeticExpr(treeNode *orig, FILE* fp, int reg);
int getRegFromInt(int ret, char *reg);

int generateProgram(treeNode* orig, FILE *fp);
int generateDeclarations(treeNode* orig, FILE *fp);

int generateAssign(treeNode* orig, FILE *fp);
int generateIterative(treeNode* orig, FILE *fp);
int generateConditional(treeNode* orig, FILE *fp);
int generateIO(treeNode* orig, FILE *fp);

#endif
