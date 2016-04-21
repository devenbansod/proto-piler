/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "parserDef.h"
#include "lexer.h"
#include "stack.h"

/*
 * FUNCTIONS RELATED TO FIRST FOLLOW SET
 *
 */
int isTerminal(int x){
  if ((x < START_NON_TERMINAL || x == EPS) && x > 0){
    return 1;
  }
  else{
    return 0;
  }
}
int exists(int x, grammarRule* firstset, int count) {
    int i;
    for (i = 0; i < count; ++i) {
        if (x == firstset[i].lhs) {
            return 1;
        }
    }
    return 0;
}


int findIndex(grammarRule  *firstset, int x, int fcount){
    int i;
    for(i = 0; i < fcount; ++i){
        if(firstset[i].lhs == x){
            return i;
        }
    }
    return -1;
}

/*
 * Calculate First sets
 *
 */
void createFirstSet(grammarRule* G, grammarRule* firstset, int *first_count, int count){
    int i;
    int fcount = 0;
    for(i = 0; i < count; ++i){
        if(!exists(G[i].lhs, firstset, count)){
            firstset[fcount].lhs = G[i].lhs;
            firstset[fcount].size_rhs = 0;
            fcount++;
        }
    }
    *first_count = fcount;
    for(i = 0; i < count; ++i){
        int eps_found = 0;
        int k = 0;
        while (k < G[i].size_rhs) {
            if (isTerminal(G[i].rhs[k])) {
                int p = findIndex(firstset, G[i].lhs, fcount);
                    firstset[p].rhs[firstset[p].size_rhs] = G[i].rhs[k];
                    firstset[p].size_rhs++;
                    break;
            } else if (! isTerminal(G[i].rhs[k])) {
                int t = findIndex(firstset, G[i].lhs, fcount);
                int p = findIndex(firstset, G[i].rhs[k], fcount);

                int q;
                for (q = 0; q < firstset[p].size_rhs; ++q) {
                    if(firstset[p].rhs[q] == EPS) {
                        eps_found = 1;
                    } else {
                        firstset[t].rhs[firstset[t].size_rhs] = firstset[p].rhs[q];
                        firstset[t].size_rhs++;
                    }
                }
                if (eps_found == 0) {
                    break;
                }
            }
            ++k;
        }
    }
}


/*
 * Print the appropriate string for the ENUM of symbolType
 *
 */
int getStringForSymbolEnum(char *ret, int st) {
    switch(st) {
        case 1:
            strcpy(ret, "TK_SQL");
            break;
        case 2:
            strcpy(ret, "TK_SQR");
            break;
        case 3:
            strcpy(ret, "TK_SEM");
            break;
        case 4:
            strcpy(ret, "TK_COLON");
            break;
        case 5:
            strcpy(ret, "TK_DOT");
            break;
        case 6:
            strcpy(ret, "TK_OP");
            break;
        case 7:
            strcpy(ret, "TK_CL");
            break;
        case 8:
            strcpy(ret, "TK_PLUS");
            break;
        case 9:
            strcpy(ret, "TK_MINUS");
            break;
        case 10:
            strcpy(ret, "TK_MUL");
            break;
        case 11:
            strcpy(ret, "TK_DIV");
            break;
        case 12:
            strcpy(ret, "TK_NOT");
            break;
        case 13:
            strcpy(ret, "TK_NE");
            break;
        case 14:
            strcpy(ret, "TK_EQ");
            break;
        case 15:
            strcpy(ret, "TK_GE");
            break;
        case 16:
            strcpy(ret, "TK_GT");
            break;
        case 17:
            strcpy(ret, "TK_LE");
            break;
        case 18:
            strcpy(ret, "TK_ASSIGNOP");
            break;
        case 19:
            strcpy(ret, "TK_LT");
            break;
        case 20:
            strcpy(ret, "TK_AND");
            break;
        case 21:
            strcpy(ret, "TK_RECORDID");
            break;
        case 22:
            strcpy(ret, "TK_OR");
            break;
        case 23:
            strcpy(ret, "TK_COMMENT");
            break;
        case 24:
            strcpy(ret, "TK_RNUM");
            break;
        case 25:
            strcpy(ret, "TK_NUM");
            break;
        case 26:
            strcpy(ret, "TK_FUNID");
            break;
        case 27:
            strcpy(ret, "TK_ID");
            break;
        case 28:
            strcpy(ret, "TK_FIELDID");
            break;
        case 29:
            strcpy(ret, "TK_COMMA");
            break;
        case 30:
            strcpy(ret, "TK_WITH");
            break;
        case 31:
            strcpy(ret, "TK_PARAMETERS");
            break;
        case 32:
            strcpy(ret, "TK_END");
            break;
        case 33:
            strcpy(ret, "TK_WHILE");
            break;
        case 34:
            strcpy(ret, "TK_INT");
            break;
        case 35:
            strcpy(ret, "TK_REAL");
            break;
        case 36:
            strcpy(ret, "TK_TYPE");
            break;
        case 37:
            strcpy(ret, "TK_MAIN");
            break;
        case 38:
            strcpy(ret, "TK_GLOBAL");
            break;
        case 39:
            strcpy(ret, "TK_PARAMETER");
            break;
        case 40:
            strcpy(ret, "TK_LIST");
            break;
        case 41:
            strcpy(ret, "TK_INPUT");
            break;
        case 42:
            strcpy(ret, "TK_OUTPUT");
            break;
        case 43:
            strcpy(ret, "TK_ENDWHILE");
            break;
        case 44:
            strcpy(ret, "TK_IF");
            break;
        case 45:
            strcpy(ret, "TK_THEN");
            break;
        case 46:
            strcpy(ret, "TK_ENDIF");
            break;
        case 47:
            strcpy(ret, "TK_READ");
            break;
        case 48:
            strcpy(ret, "TK_WRITE");
            break;
        case 49:
            strcpy(ret, "TK_RETURN");
            break;
        case 50:
            strcpy(ret, "TK_RECORD");
            break;
        case 51:
            strcpy(ret, "TK_ENDRECORD");
            break;
        case 52:
            strcpy(ret, "TK_ELSE");
            break;
        case 53:
            strcpy(ret, "TK_CALL");
            break;
        case 1000:
            strcpy(ret, "DOLLAR");
            break;
        case -1:
            strcpy(ret, "ERROR");
            break;
        case 100:
            strcpy(ret, "EOI");
            break;
        case 500:
            strcpy(ret, "EPS");
            break;
        case 201:
            strcpy(ret, "program");
            break;
        case 202:
            strcpy(ret, "mainFunction");
            break;
        case 203:
            strcpy(ret, "otherFunctions");
            break;
        case 204:
            strcpy(ret, "function");
            break;
        case 205:
            strcpy(ret, "input_par");
            break;
        case 206:
            strcpy(ret, "output_par");
            break;
        case 207:
            strcpy(ret, "parameter_list");
            break;
        case 208:
            strcpy(ret, "dataType");
            break;
        case 209:
            strcpy(ret, "primitiveDatatype");
            break;
        case 210:
            strcpy(ret, "constructedDatatype");
            break;
        case 211:
            strcpy(ret, "remaining_list");
            break;
        case 212:
            strcpy(ret, "stmts");
            break;
        case 213:
            strcpy(ret, "typeDefinitions");
            break;
        case 214:
            strcpy(ret, "typeDefinition");
            break;
        case 215:
            strcpy(ret, "fieldDefinitions");
            break;
        case 216:
            strcpy(ret, "fieldDefinition");
            break;
        case 217:
            strcpy(ret, "moreFields");
            break;
        case 218:
            strcpy(ret, "declarations");
            break;
        case 219:
            strcpy(ret, "declaration");
            break;
        case 220:
            strcpy(ret, "global_or_not");
            break;
        case 221:
            strcpy(ret, "otherStmts");
            break;
        case 222:
            strcpy(ret, "stmt");
            break;
        case 223:
            strcpy(ret, "assignmentStmt");
            break;
        case 224:
            strcpy(ret, "singleOrRecId");
            break;
        case 225:
            strcpy(ret, "new_24");
            break;
        case 226:
            strcpy(ret, "funCallStmt");
            break;
        case 227:
            strcpy(ret, "outputParameters");
            break;
        case 228:
            strcpy(ret, "inputParameters");
            break;
        case 229:
            strcpy(ret, "iterativeStmt");
            break;
        case 230:
            strcpy(ret, "conditionalStmt");
            break;
        case 231:
            strcpy(ret, "elsePart");
            break;
        case 232:
            strcpy(ret, "ioStmt");
            break;
        case 233:
            strcpy(ret, "allVar");
            break;
        case 234:
            strcpy(ret, "arithmeticExpression");
            break;
        case 235:
            strcpy(ret, "expPrime");
            break;
        case 236:
            strcpy(ret, "term");
            break;
        case 237:
            strcpy(ret, "termPrime");
            break;
        case 238:
            strcpy(ret, "factor");
            break;
        case 239:
            strcpy(ret, "highPrecedenceOperator");
            break;
        case 240:
            strcpy(ret, "lowPrecedenceOperators");
            break;
        case 241:
            strcpy(ret, "all");
            break;
        case 242:
            strcpy(ret, "temp");
            break;
        case 243:
            strcpy(ret, "operator");
            break;
        case 244:
            strcpy(ret, "booleanExpression");
            break;
        case 245:
            strcpy(ret, "var");
            break;
        case 246:
            strcpy(ret, "logicalOp");
            break;
        case 247:
            strcpy(ret, "relationalOp");
            break;
        case 248:
            strcpy(ret, "returnStmt");
            break;
        case 249:
            strcpy(ret, "optionalReturn");
            break;
        case 250:
            strcpy(ret, "idList");
            break;
        case 251:
            strcpy(ret, "more_ids");
            break;
        case 252:
            strcpy(ret, "some_types");
            break;
        case 253:
            strcpy(ret, "other_types");
            break;
        case 254:
            strcpy(ret, "for_ll1");
            break;
        default:
            strcpy(ret, "No Matching symbol");
            break;
    }

    return strlen(ret);
}

/*
 * Find the index for matching set
 * for a given terminal/non-terminal
 *
 */
int findMatchingSet(grammarRule *rules, int NT, int count) {
    int i;
    for(i = 0; i < count; ++i) {
        if (rules[i].lhs == NT) {
            return i;
        }
    }
    return -1;
}

/*
 * FIRST, FOLLOW and GRAMMAR have same structure in File
 * Use this to read all of them separately
 *
 */
grammarRule* readFileForRules(FILE *file, int *count){
    char* line;
    size_t len = 0;
    ssize_t read;
    int i = 0;

    grammarRule* rules = (grammarRule *)malloc(sizeof(grammarRule) * (*count));

    if (file) {
        while ((read = getline(&line, &len, file)) != -1) {
            char* str = strdup(line);
            char* token = strsep(&str, "-");
            rules[i].lhs = atoi(token);

            token = strsep(&str, "-");
            char *right = strdup(token);
            int j = 0;
            while((token = strsep(&right, " "))) {
                rules[i].rhs[j] = atoi(token);
                ++j;
                rules[i].size_rhs = j;
            }
            ++i;
        }
    } else{
        printf("Error opening file\n");
        return 0;
    }

    *count = i;
    return rules; //number of rules read
}


/*
 * Used for printing the grammar, first or follow set
 * read from file
 *
 * - Used for debug purpose only
 *
 */
void printGrammar(grammarRule* G, int count){
    // Routine to Print the grammar G
    int i;
    for (i = 0; i < count; ++i){
        printf("%d ->", G[i].lhs);
        int j;
        for(j = 0; j < G[i].size_rhs;++j){
            printf("%d ", G[i].rhs[j]);
        }
        printf("\n");
    }
}

/*
 * Create parse table from Grammar, First set and follow set
 *
 */
parseTable** createParseTable(
    grammarRule* G, int rule_count, parseTable **T
) {
    FILE *follow_file = fopen("./follow_set.txt", "r");

    // Declaring the able inside the function as a temporary hack
    int i;

    T = (parseTable**) malloc(sizeof(parseTable*) * COUNT_NON_TERMINAL + 1);
    for (i = 0; i < COUNT_NON_TERMINAL + 1; i++) {
        T[i] = (parseTable*)malloc(sizeof(parseTable) * COUNT_TERMINAL + 1);
    }
    // Routine to read in the first and follow sets
    int first_count = COUNT_NON_TERMINAL + 1;

    grammarRule *firstset = (grammarRule*)malloc(sizeof(grammarRule) * first_count);
    createFirstSet(G, firstset, &first_count, rule_count);
    // grammarRule *firstset = readFileForRules(first_file, &first_count);
    int follow_count = COUNT_NON_TERMINAL + 1;
    grammarRule *followset = readFileForRules(follow_file, &follow_count);

    // initialise the parse table elements to -1
    for (i = 0; i < COUNT_NON_TERMINAL; ++i) {
        int j = 0;
        for (j = 0; j < COUNT_TERMINAL; ++j) {
            T[i][j] = -1;
        }
    }

    // Routine to populate the parse table with grammar rule numbers
    for (i = 0; i < rule_count; ++i) {
        // for each rule in grammar
        int grammarRuleNumber = i;

        int k = 0;
        int eps_found = 0;
        int index;
        while (k < G[i].size_rhs) {
            if (G[i].rhs[k] == EPS) {
                ++k;
                eps_found = 1;
                break;
            }

            if (G[i].rhs[k]< START_NON_TERMINAL) {
                T[G[i].lhs - START_NON_TERMINAL][G[i].rhs[k] - START_TERMINAL]
                    = grammarRuleNumber;
                break;
            } else if (G[i].rhs[k] == DOLLAR) {
                T[G[i].lhs - START_NON_TERMINAL][DOLLAR_STORE]
                    = grammarRuleNumber;
                break;
            } else {
                index = findMatchingSet(firstset, G[i].rhs[k], first_count);
                int j;
                for (j = 0; index >= 0 && j < firstset[index].size_rhs; ++j) {
                    if (firstset[index].rhs[j] == EPS) {
                        eps_found = 1;
                    } else{
                        T[G[i].lhs - START_NON_TERMINAL][firstset[index].rhs[j] - START_TERMINAL]
                            = grammarRuleNumber;
                    }
                }

                if (j == firstset[index].size_rhs && eps_found == 0) {
                    break;
                } else {
                    ++k;
                }
            }
        }

        if (eps_found == 1 && G[i].size_rhs == k) {
            int fl_index = findMatchingSet(followset, G[i].lhs, follow_count);
            int p;
            for (p = 0; p  < followset[fl_index].size_rhs; ++p) {
                T[G[i].lhs - START_NON_TERMINAL][followset[fl_index].rhs[p] - START_TERMINAL]
                    = grammarRuleNumber;
            }
        }
    }

    fclose(follow_file);

    return T;

}

/*
 * Used for printing the parse table
 *
 * - Used for debug purpose only
 *
 */
void printParseTable(FILE *fp, parseTable **T) {
    // routine to print the damn table
    int j;
    for(j = 0; j < COUNT_TERMINAL; ++j){
        fprintf(fp, "%d\t", j + START_TERMINAL);
    }
    printf("\n");
    int i = 0;
    for (i = 0; i < COUNT_NON_TERMINAL; ++i) {
        for (j = 0; j < COUNT_TERMINAL; ++j) {
            fprintf(fp, "%d\t", T[i][j] + 1);
        }
        fprintf(fp, "\n");
    }
}

/*
 * Create and return a tree node with given symbol type
 *
 */
treeNode* getTreeNode(
	treeNode *parent, stackElem st
) {
	treeNode* new_node = (treeNode*)malloc(sizeof(treeNode));
	new_node->parent = parent;
	new_node->symbol_type = st.symbol_type;
	new_node->curr_children = 0;
    new_node->processed_children = 0;

	return new_node;
}

/*
 * Set tokenInfo for a leaf node (same as match terminal from token
 * to terminal on top of stack)
 *
 */
void setTokenInfo(treeNode *t_node, tokenInfo tk_info) {
	t_node->tk_info = tk_info;
    t_node->processed_children++;
}

/*
 * Create and return a parse tree
 *
 */
parseTree* createParseTree(parseTree *new_tree) {
	new_tree = (parseTree*)malloc(sizeof(parseTree));
	new_tree->root = NULL;
	new_tree->size = 0;
    return new_tree;
}


/*
 * Helper function to recurse over child nodes
 * while printing the parse tree
 *Nide
 */

 

int calcNodeSize(treeNode *currNode){
    int size = 0;
    size += sizeof(currNode->parent) + sizeof(currNode->next);
    size += sizeof(currNode->symbol_type) + sizeof(currNode->tk_info);
    size += sizeof(currNode->st) + sizeof(currNode->curr_children) + sizeof(currNode->processed_children);
    int i = 0;
    for(; i < currNode->curr_children;++i){
        size += sizeof(currNode->children[i]);
    } 
    return size;
}

int treeSize = 0;
int numberNodes = 0;
void parseTreeSize(treeNode *current_tree_node) {
    if (current_tree_node) {
        // printf("%d %d\n", calcNodeSize(current_tree_node), current_tree_node->symbol_type);
        treeSize += calcNodeSize(current_tree_node);
        numberNodes++;
        int k = 0;
        for(k = 0; current_tree_node && k < current_tree_node->curr_children; k++) {
            parseTreeSize(current_tree_node->children[k]);
        }
    }    
}


void printParseTreeHelper(treeNode *current_tree_node, FILE *out_file) {
    char par[100], cur[100];
    int i = 0;

    if (current_tree_node
        && current_tree_node->symbol_type >= START_NON_TERMINAL
        && current_tree_node->symbol_type != EPS
    ) {
        memset(par, '\0', 100);
        memset(cur, '\0', 100);
        getStringForSymbolEnum(
            cur, current_tree_node->symbol_type
        );
        int i = 0;

        // lexeme and padding
        fprintf(out_file, "----");
        for (i = 4; i < 24; i++) {
            fprintf(out_file, " ");
        }

        // line number and padding
        fprintf(out_file, "----");
        for (i = 4; i < 8; i++) {
            fprintf(out_file," ");
        }

        // token and padding
        fprintf(out_file, "----");
        for (i = 4; i < 16; i++) {
            fprintf(out_file," ");
        }

        // valueIfNum and padding
        fprintf(out_file, "----");
        for (i = 4; i < 12; i++) {
            fprintf(out_file," ");
        }

        // parent node and padding
        if (current_tree_node->parent) {
            getStringForSymbolEnum(
                par, current_tree_node->parent->symbol_type
            );
            fprintf(out_file, "%s", par);
            for (i = strlen(par); i < 26; i++) {
                fprintf(out_file," ");
            }
        } else {
            fprintf(out_file, "ROOT");
            for (i = 0; i < 22; i++) {
                fprintf(out_file," ");
            }
        }

        fprintf(out_file, "NO");
        for (i = 2; i < 14; i++) {
            fprintf(out_file," ");
        }

        fprintf(out_file, "%s\n", cur);
    } else if (current_tree_node) {
        memset(par, '\0', 100);
        memset(cur, '\0', 100);

        getStringForSymbolEnum(
            cur, current_tree_node->symbol_type
        );

        // lexeme and padding
        fprintf(out_file, "%s", current_tree_node->tk_info.lexeme);
        for (i = strlen(current_tree_node->tk_info.lexeme); i < 24; i++) {
            fprintf(out_file, " ");
        }

        // line number and padding
        fprintf(out_file, "%d", current_tree_node->tk_info.line_no);
        char line[10];
        sprintf(line, "%d", current_tree_node->tk_info.line_no);

        for (i = strlen(line); i < 8; i++) {
            fprintf(out_file," ");
        }

        // token and padding
        fprintf(out_file, "%s", cur);
        for (i = strlen(cur); i < 16; i++) {
            fprintf(out_file," ");
        }

        // valueIfNum and padding
        if (current_tree_node->tk_info.symbol_type == TK_NUM
            || current_tree_node->tk_info.symbol_type == TK_RNUM
        ) {
            fprintf(out_file, "%s", current_tree_node->tk_info.lexeme);
            for (i = strlen(current_tree_node->tk_info.lexeme); i < 12; i++) {
                fprintf(out_file," ");
            }
        } else {
            fprintf(out_file, "----");
            for (i = 4; i < 12; i++) {
                fprintf(out_file," ");
            }
        }

        // parent node and padding
        if (current_tree_node->parent) {
            getStringForSymbolEnum(
                par, current_tree_node->parent->symbol_type
            );
            fprintf(out_file, "%s", par);
            for (i = strlen(par); i < 26; i++) {
                fprintf(out_file," ");
            }
        } else {
            fprintf(out_file, "ROOT");
            for (i = 0; i < 22; i++) {
                fprintf(out_file," ");
            }
        }

        fprintf(out_file, "YES");
        for (i = 3; i < 14; i++) {
            fprintf(out_file," ");
        }

        fprintf(out_file, "----\n");

    }

    int k = 0;
    for(k = 0; current_tree_node && k < current_tree_node->curr_children; k++) {
        printParseTreeHelper(current_tree_node->children[k], out_file);
    }
}

/*
 * Print the parse Tree in file named char* filename
 *
 */
void printParseTree(parseTree *new_tree, FILE* filename) {
    FILE *out_file = filename;

    // print the Header line
    fprintf(out_file, "\t\t\t\t\t\t\t\t\t************** PARSE TREE ***************\n");
    fprintf(out_file,
        "LexemeCurrentNode \t\tLineNo \tToken  \t\t\tValueIfNum \tParentNodeSymbol \t\tisLeafNode  \tNodeSymbol\n"
    );

    // print the whole tree
    printParseTreeHelper(new_tree->root, out_file);
}

/*
 * Checks if the terminal exists in Follow set of given NT
 *
 * Un-used
 */
int checkIfInFollow(grammarRule *rules, int f_count,
    int NT_symbol_type, int TK_symbol_type
) {
    int k = 0;
    int index = findMatchingSet(rules, NT_symbol_type, f_count);
    for(k = 0; k < rules[index].size_rhs; k++) {
        if (rules[index].rhs[k] == TK_symbol_type) {
            return 1;
        }
    }

    return 0;
}

/*
 * Main parse function
 *
 */
parseTree* parseInputSourceCode(
    char *testcaseFile, int *error
) {

    FileBuffer b;

    FILE *fp = fopen(testcaseFile, "r");
    fp = getStream(fp, &b, 4096);

    // read the grammar from file
    int rule_count = COUNT_RULE;
    FILE *grammar_file = fopen("./grammar.txt", "r");
    grammarRule *G = readFileForRules(grammar_file, &rule_count);

    parseTable **T = NULL;

    // create the parseTable from First and follow info
    T = createParseTable(G, rule_count, T);

	// initialize new parse tree
	parseTree *new_tree = NULL;
	new_tree = createParseTree(new_tree);

	// initialize the new stack
	Stack *s = NULL;
	s = createStack(s, 100);

	// push the EOS
	stackElem st;
    st.symbol_type = EOS;
	push(st, s);

    st.symbol_type = program;

    // create the Root Tree node with Start symbol
    treeNode* current_tree_node = getTreeNode(NULL, st);
    new_tree->root = current_tree_node;

    // push the start symbol
    st.t_node = current_tree_node;
    push(st, s);

    *error = 1;
    int line = 1;
    int moveAhead = 1;

    int errorRecovery = 0;

    while (! isEmpty(s)) {
        char lexeme[101];
        tokenInfo next_token; next_token.error = -1;
        memset(lexeme, '\0', 100);

        if (moveAhead) {
            next_token = getNextToken(&b, &line, lexeme);
        }


        // If lexer has reported error, ask for the next error-free token
        if (next_token.symbol_type == ERROR) {
            reportError(stderr, next_token.error, next_token);
            errorRecovery = 1;

            memset(lexeme, '\0', 100);
            continue;
        } else if (next_token.symbol_type == TK_COMMENT) {
            continue;
        } else if (next_token.error == 1) {
            // but if it's only the Long Identifier error,
            // cut it short and accept it as valid token

            if (! errorRecovery) {
                reportError(stderr, next_token.error, next_token);
                fprintf(stderr, "I cut short your Identifier on line %d\n", next_token.line_no);
            }
        }

        stackElem top = pop(s);
        current_tree_node = top.t_node;

        if (top.symbol_type == EOS &&
            next_token.symbol_type == EOI
        ) {
            // End of Input, input accepted
            *error = 0;
            break;
        } else if (next_token.error == 100
            || next_token.symbol_type == EOI
        ) {
            // input ended, but stack not empty

            char str[40];
            memset(str, '\0', 40);
            getStringForSymbolEnum(str, top.symbol_type);
            fprintf(stderr, "*** ERROR 4: Input is consumed "
                "while it is expected to have token <%s> at line number <%d>\n",
                str, next_token.line_no
            );
            break;
        } else if (top.symbol_type == EOS) {
            fprintf(stderr, "End of Stack! ERROR!\n");
            break;
        }

        moveAhead = 0;

        if (top.symbol_type == EPS) {
            moveAhead = 0;
            continue;
        }


        // If the top is a Terminal
        if (top.symbol_type < START_NON_TERMINAL) {

            // both are terminals and match
            if (top.symbol_type == next_token.symbol_type) {
                moveAhead = 1;

                // match Leaf and put in tree
                setTokenInfo(current_tree_node, next_token);
            } else {
                // Both are terminals but NOT matching

                if (! errorRecovery) {
                    char err_str[100], err_str2[100];
                    memset(err_str, '\0', 100);
                    memset(err_str2, '\0', 100);
                    getStringForSymbolEnum(err_str, top.symbol_type);
                    getStringForSymbolEnum(err_str2, next_token.symbol_type);

                    fprintf(stderr, "*** ERROR 5: The token <%s> for lexeme <%s>"
                        " at line <%d>. Expected is <%s>\n",
                        err_str2, next_token.lexeme, next_token.line_no, err_str
                    );
                    errorRecovery = 1;
                }

                // Put the expected one back, in hopes
                // that this was just a stray token in input file
                push(top, s);
                moveAhead = 1;
                continue;
            }
        } else {
            int rule_no = T[top.symbol_type - START_NON_TERMINAL][next_token.symbol_type - START_TERMINAL];

            // table entry is empty
            if (rule_no == -1) {
                    char err_str[100], err_str2[100];
                    memset(err_str, '\0', 100);
                    memset(err_str2, '\0', 100);
                    getStringForSymbolEnum(err_str, top.symbol_type);
                    getStringForSymbolEnum(err_str2, next_token.symbol_type);

                if (! errorRecovery) {
                    fprintf(stderr, "*** ERROR 6 : NO RULE FOUND for <%s>"
                        " -> <%s> (lexeme : <%s>) at line : <%d>\n",
                        err_str, err_str2,
                        next_token.lexeme, next_token.line_no
                    );
                    fprintf(stderr, "Trying PANIC MODE! Skipping the token <%s> !\n", err_str2);
                    errorRecovery = 1;
                }

                push(top, s);
                moveAhead = 1;
                continue;
            }

            grammarRule rule_to_apply = G[rule_no];
            int k = 0;

            // Never taken in action
            if (rule_to_apply.lhs != top.symbol_type) {
                if (! errorRecovery) {
                    fprintf(stderr, "Rule's NT does not match! Expected %d : Got %d!\n",
                        rule_to_apply.lhs, top.symbol_type
                    );
                    fprintf(stderr, "Moving ahead till valid Expected token\n");
                    errorRecovery = 1;
                }
            }

            current_tree_node->curr_children = rule_to_apply.size_rhs;
            current_tree_node->children
                = (treeNode**)malloc(sizeof(treeNode*) * rule_to_apply.size_rhs);
            current_tree_node->processed_children = 0;

            // add the pointer to the TreeNode to the stackElem
            for (k = rule_to_apply.size_rhs - 1; k >= 0; k--) {
                stackElem curr;
                curr.symbol_type = rule_to_apply.rhs[k];

                current_tree_node->children[k]
                    = (treeNode*) malloc(sizeof(treeNode));
                current_tree_node->children[k]->parent
                    = current_tree_node;
                current_tree_node->children[k]->symbol_type
                    = curr.symbol_type;

                curr.t_node = current_tree_node->children[k];

                push(curr, s);
            }
        }
    }

	/* ALGO on pg. 227
	 * OR
	 * https://courses.cs.washington.edu/courses/cse401/04sp/slides/03b-LL1-example.pdf
     */
    if (*error == 0
        && errorRecovery == 0
    ) {
        fprintf(stderr, "\nCompiled Successfully: \n"
            "Input source code is syntactically correct!!\n\n"
        );
    } else if (*error == 0 && errorRecovery == 1) {
        fprintf(stderr, "\nCompiled Successfully: \n"
        );
        fprintf(stderr, "\nCompiled with PANIC MODE. \nSome tokens might have been skipped"
            " \nand some errors might have been present! \nPANIC MODE parse tree is returned! \n"
        );
    } else {
        fprintf(stderr, "\nCompilation failed: "
            "Input source code is syntactically NOT correct!!\n\n"
        );
    }

    fclose(grammar_file);

    return new_tree;

}
