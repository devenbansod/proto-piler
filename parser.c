#include "common.h"
#include "parserDef.h"
#include "lexer.h"
#include "stack.h"

/*
 * Find the matching set for a given terminal/non-terminal
 *
 */
int findMatchingSet(grammarRule *rules, int NT, int count) {
    int i;
    for(i = 0; i < count; ++i) {
        if (rules[i].lhs.NonT == NT) {
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
int readFileForRules(FILE *file, grammarRule** rules){
    char* line;
    size_t len = 0;
    ssize_t read;
    int i = 0;

    if (file) {
        while ((read = getline(&line, &len, file)) != -1) {
            char* str = strdup(line);
            char* token = strsep(&str, "-");
            *rules[i].lhs.NonT = atoi(token);
            *rules[i].lhs.isTerminal = 0;

            token = strsep(&str, "-");
            char *right = strdup(token);
            int j = 0;
            while((token = strsep(&right, " "))) {
                if (atoi(token) < START_NON_TERMINAL) {
                    *rules[i].rhs[j].Term = atoi(token);
                    *rules[i].rhs[j].isTerminal = 1;
                } else {
                    *rules[i].rhs[j].NonT = atoi(token);
                    *rules[i].rhs[j].isTerminal = 0;
                }

                if (*rules[i].rhs[j].NonT == EPS) {
                    *rules[i].is_eps = 1;
                }
                if (*rules[i].rhs[j].NonT == DOLLAR) {
                    *rules[i].is_dollar = 1;
                }
                ++j;
                *rules[i].size_rhs = j;
            }
            ++i;
        }
    } else{
        printf("Error opening file\n");
        return 0;
    }
    fclose(file);

    return i; //number of rules read
}

void printGrammar(grammarRule* G, int count){
    // Routine to Print the grammar G
    int i;
    for (i = 0; i < count; ++i){
        printf("%d ->", G[i].lhs.NonT);
        int j;
        for(j = 0; j < G[i].size_rhs;++j){
            printf("%d ", G[i].rhs[j].Term);
        }
        printf("\n");
    }
}

parseTable** createParseTable(
    grammarRule* G, int rule_count, FILE *first_file, FILE* follow_file, parseTable **T
) {

    // Declaring the able inside the function as a temporary hack
    int i;

    T = (parseTable**) malloc(sizeof(parseTable*) * COUNT_NON_TERMINAL);
    for (i = 0; i < COUNT_NON_TERMINAL; i++) {
        T[i] = (parseTable*)malloc(sizeof(parseTable) * COUNT_TERMINAL);
    }
    // Routine to read in the first and follow sets
    grammarRule firstset[COUNT_NON_TERMINAL];
    int first_count = readFileForRules(first_file, &firstset);

    grammarRule followset[COUNT_NON_TERMINAL];
    int follow_count = readFileForRules(follow_file, &followset);

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
        NonTerminal x = G[i].lhs.NonT;

        int k = 0;
        int eps_found = 0;
        int index;
        while (k < G[i].size_rhs) {
            if (G[i].rhs[k].NonT == EPS) {
                ++k;
                eps_found = 1;
            }

            if (G[i].rhs[k].isTerminal && G[i].rhs[k].Term < START_NON_TERMINAL) {
                T[G[i].lhs.NonT - START_NON_TERMINAL][G[i].rhs[k].Term - START_TERMINAL]
                    = grammarRuleNumber;
                break;
            } else {
                index = findMatchingSet(firstset, G[i].rhs[k].NonT, first_count);
                int j;
                for (j = 0; j < firstset[index].size_rhs; ++j) {
                    if (firstset[index].rhs[j].NonT == 33) {
                        ++k;
                        eps_found = 1;
                    } else{
                        T[G[i].lhs.NonT - START_NON_TERMINAL][firstset[index].rhs[j].Term - START_TERMINAL]
                            = grammarRuleNumber;
                    }
                }

                if (j == firstset[index].size_rhs && eps_found == 0) {
                    break;
                }
            }
        }

        if (eps_found == 1 && G[i].size_rhs == k) {
            int fl_index = findMatchingSet(followset, G[i].lhs.NonT, follow_count);
            int p;
            for (p = 0; p  < followset[fl_index].size_rhs; ++p) {
                T[G[i].lhs.NonT - START_NON_TERMINAL][followset[fl_index].rhs[p].Term - START_TERMINAL]
                    = grammarRuleNumber;
            }
        }
    }

    return T;

}

termNonTerm getTermNonTerm(int isTerminal, symbolType st, int error) {
	termNonTerm t;
	t.isTerminal = isTerminal;
	if (isTerminal) {
		t.symbol_type.Term = st.Term;
	} else {
		t.symbol_type.NonT = st.NonT;
	}

	t.error = error;

	return t;
}

treeNode* getTreeNode(
	int isTerminal,
	treeNode *parent, symbolType st
) {
	treeNode* new_node = (treeNode*)malloc(sizeof(treeNode));
	new_node->parent = parent;
	new_node->symbol_type = st;
	new_node->curr_children = 0;

	return new_node;
}

void setTokenInfo(treeNode *t_node, tokenInfo tk_info) {
	t_node->tk_info = tk_info;
}

void createParseTree(parseTree *new_tree) {
	new_tree = (parseTree*)malloc(sizeof(parseTree));
	new_tree->root = NULL;
	new_tree->size = 0;
}


parseTree parseInputSourceCode(char *testcaseFile) {

    FileBuffer b;

    FILE *fp = fopen(testcaseFile, "r");
    fp = getStream(fp, &b, 4096);

    // read the grammar from file
    grammarRule *G;
    FILE *grammar_file = fopen("grammar_converted.txt", "r");
    int rule_count = readFileForRules(grammar_file, &G);

    FILE *first_file = fopen("first_sets_converted.txt", "r");
    FILE *follow_file = fopen("follow_sets_converted.txt", "r");

    parseTable **T;

    // create the parseTable from First and follow info
    T = createParseTable(G, rule_count, first_file, follow_file, T);

    // routine to print the damn table
    int i = 0;
    for(i = 0; i < COUNT_NON_TERMINAL; ++i){
        int j;
        for(j = 0; j < COUNT_TERMINAL; ++j){
            printf("%d\t", T[i][j]);
        }
        printf("\n");
    }
	// initialize new parse tree
	// parseTree *new_tree;
	// createParseTree(new_tree);

	// // initialize the new stack
	// Stack *s;
	// createStack(s);

	// // push the EOS
	// symbolType st; st.isTerminal = 0; st.Term = EOS;
	// termNonTerm bottom = getTermNonTerm(0, st, 0);
	// push(bottom, s);

	// // push the start symbol
	// st.isTerminal = 0; st.NonT = program;
	// termNonTerm start_sym = getTermNonTerm(0, st, 0);
	// push(start_sym, s);

	// // create the Root Tree node with Start symbol
 //    treeNode* current = getTreeNode(0, NULL, st);

 //    int error = 0;
 //    int line = 1;
 //    int moveAhead = 1;
 //    while (0 && ! isEmpty(s) && error == 0) {
 //        termNonTerm top = pop(s);

 //        char lexeme[101];
 //        tokenInfo next_token; next_token.error = -1;

 //        if (moveAhead)
 //            next_token = getNextToken(&b, &line, lexeme);
 //        moveAhead = 0;
 //        if (next_token.error == 100) {
 //            // both are terminals
 //            if (top.symbol_type.Term == next_token.state_id) {
 //                moveAhead = 1;
 //                // match - put in tree
 //            } else {
 //                printf("End of File\n");
 //                break;
 //            }
 //        } else {
 //            int rule_no = T[top.symbol_type.NonT][next_token.state_id];
 //            if (rule_no == -1) {
 //                printf("Error! Aborting\n");
 //                break;
 //            }
 //            grammarRule rule_to_apply = G[rule_no];
 //            int k = 0;

 //            if (rule_to_apply.lhs.NonT != top.symbol_type.NonT) {
 //                printf("Error! Rule does not match!\n");
 //                break;
 //            }

 //            for (k = rule_to_apply.size_rhs - 1; k >= 0; k--) {
 //                // push(rule_to_apply.rhs[k], s);
 //            }

 //            // applyRuleToTreeNode();
 //        }
 //    }



	// ALGO on pg. 227
	// OR
	// https://courses.cs.washington.edu/courses/cse401/04sp/slides/03b-LL1-example.pdf

}


int parser();
