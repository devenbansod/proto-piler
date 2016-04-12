/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef TYPE_CHECKER_INTERFACE
#define TYPE_CHECKER_INTERFACE


#include "treeDef.h"

int checkAndReturnType(treeNode* orig, char* type_name);
int performTypeChecking (treeNode* orig);


#endif