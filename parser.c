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
    fclose(file);

    *count = i;
    return rules; //number of rules read
}

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

parseTable** createParseTable(
    grammarRule* G, int rule_count, FILE *first_file, FILE* follow_file, parseTable **T
) {

    // Declaring the able inside the function as a temporary hack
    int i;

    T = (parseTable**) malloc(sizeof(parseTable*) * COUNT_NON_TERMINAL + 1);
    for (i = 0; i < COUNT_NON_TERMINAL + 1; i++) {
        T[i] = (parseTable*)malloc(sizeof(parseTable) * COUNT_TERMINAL + 1);
    }
    // Routine to read in the first and follow sets
    int first_count = COUNT_NON_TERMINAL + 1;
    grammarRule *firstset = readFileForRules(first_file, &first_count);

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
        Symbol x = G[i].lhs;
        int t;

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

    return T;

}

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

// termNonTerm getTermNonTerm(Symbol st, int error) {
	

// 	if (st.isTerminal) {
// 		t.symbol_type.Term = st.Term;
//         t.symbol_type.isTerminal = 1;
// 	} else {
// 		t.symbol_type.NonT = st.NonT;
//         t.symbol_type.isTerminal = 0;
// 	}

// 	t.error = error;

// 	return t;
// }

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

void setTokenInfo(treeNode *t_node, tokenInfo tk_info) {
	t_node->tk_info = tk_info;
    t_node->processed_children++;
}

parseTree* createParseTree(parseTree *new_tree) {
	new_tree = (parseTree*)malloc(sizeof(parseTree));
	new_tree->root = NULL;
	new_tree->size = 0;
    return new_tree;
}

treeNode* applyRuleToTreeNode(treeNode *curr, grammarRule *rule) {
    curr->curr_children = rule->size_rhs;
    curr->children = (treeNode**)malloc(sizeof(treeNode*) * rule->size_rhs);
    curr->processed_children = 0;

    int k = 0;
    printf("Apply rule of size : %d\n", rule->size_rhs);
    for (k = 0; k < rule->size_rhs; k++) {
        curr->children[k] = (treeNode *)malloc(sizeof(treeNode));
        curr->children[k]->parent = curr;
        curr->children[k]->curr_children = 0;

        curr->children[k]->symbol_type = rule->rhs[k];
        printf("\nAssigning %d's parent as %d \n\n",
            curr->children[k]->symbol_type,
            curr->symbol_type
        );
    }

    // Special case for EPS
    if (curr->curr_children == 1 && rule->rhs[0] == EPS) {
        curr->processed_children = 1;
    }

    return curr;
}

treeNode* moveToNextNode(treeNode *current_tree_node) {
    int next_child = 0;

    printf("Moving from %d child %d out of %d\n",
        current_tree_node->symbol_type,
        current_tree_node->processed_children,
        current_tree_node->curr_children
    );

    while (current_tree_node
        && current_tree_node->curr_children <= current_tree_node->processed_children
    ) {
        // if (current_tree_node->parent) current_tree_node->parent->processed_children++;
        current_tree_node = current_tree_node->parent;
        printf("Here C : %d, parent : %d\n",
            current_tree_node->symbol_type,
            current_tree_node->parent->symbol_type);
        if (current_tree_node) {
            current_tree_node->processed_children++;
            next_child = current_tree_node->processed_children;
        }
    }

    printf("Moving to %d child %d out of %d\n",
        current_tree_node->symbol_type, next_child,
        current_tree_node->curr_children
    );

    return current_tree_node->children[next_child];
}


void printParseTreeHelper(treeNode *current_tree_node) {

    if (current_tree_node->parent) {
        printf("Token : %d\tP : %d\tLexeme : `%s`\n",
            current_tree_node->symbol_type,
            current_tree_node->parent->symbol_type,
            current_tree_node->tk_info.lexeme
        );
    } else {
        printf("Token : %d\tP : 0/0 \tLexeme : `%s`\n",
            current_tree_node->symbol_type,
            current_tree_node->tk_info.lexeme
        );
    }

    int k = 0;
    for(k = 0; k < current_tree_node->curr_children; k++) {
        printParseTreeHelper(current_tree_node->children[k]);
    }
}

void printParseTree(parseTree *new_tree) {
    printParseTreeHelper(new_tree->root);
}

parseTree* parseInputSourceCode(char *testcaseFile) {

    FileBuffer b;

    FILE *fp = fopen(testcaseFile, "r");
    fp = getStream(fp, &b, 4096);

    // read the grammar from file
    int rule_count = COUNT_RULE;
    FILE *grammar_file = fopen("./grammar_converted.txt", "r");
    grammarRule *G = readFileForRules(grammar_file, &rule_count);

    FILE *follow_file = fopen("./follow_set_converted.txt", "r");
    FILE *first_file = fopen("./first_set_converted.txt", "r");

    parseTable **T;

    // create the parseTable from First and follow info
    T = createParseTable(G, rule_count, first_file, follow_file, T);

    printParseTable(stdout, T);

	// initialize new parse tree
	parseTree *new_tree;
	new_tree = createParseTree(new_tree);

	// initialize the new stack
	Stack *s;
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

    int error = 0;
    int line = 1;
    int moveAhead = 1;

    while (! isEmpty(s) && error == 0) {
        char lexeme[101];
        tokenInfo next_token; next_token.error = -1;
        memset(lexeme, '\0', 100);

        if (moveAhead) {
            next_token = getNextToken(&b, &line, lexeme);
            printf("READ TOKEN : %d -> `%s`\n", next_token.symbol_type, next_token.lexeme);
        }

        if (next_token.symbol_type == TK_COMMENT) {
            continue;
        } else if (next_token.error == 100) {
            printf("End of File\n");
            break;
        }

        stackElem top = pop(s);
        current_tree_node = top.t_node;

        printf("POP : %d\n", top.symbol_type);

        moveAhead = 0;
        if (top.symbol_type < START_NON_TERMINAL
            || top.symbol_type == EOS
        ) {
            // both are terminals
            if (top.symbol_type == next_token.symbol_type) {
                moveAhead = 1;

                // match Leaf and put in tree
                setTokenInfo(current_tree_node, next_token);
                printf("Matched %d and Moved ahead\n", next_token.symbol_type);
            } else if (top.symbol_type == EOS) {
                printf("End of Stack! ERROR!\n");
                break;
            } else {
                printf("End of File: Expected : %d, Got %d -> `%s`\n", top.symbol_type, next_token.symbol_type, next_token.lexeme);
                break;
            }
        } else {
            if (top.symbol_type == EOS) {
                printf("Error! Aborting\n");
                break;
            }
            int rule_no = T[top.symbol_type - START_NON_TERMINAL][next_token.symbol_type - START_TERMINAL];
            if (rule_no == -1) {
                printf("Error! Aborting since NO RULE FOUND for %d -> %d (lexeme : `%s`)\n", top.symbol_type,
                    next_token.symbol_type, next_token.lexeme);
                break;
            }

            grammarRule rule_to_apply = G[rule_no];
            int k = 0;

            if (rule_to_apply.lhs != top.symbol_type) {
                printf("Error! Rule %d does not match %d : %d!\n", rule_no, rule_to_apply.lhs, top.symbol_type);
                break;
            }

            current_tree_node->curr_children = rule_to_apply.size_rhs;
            current_tree_node->children = (treeNode**)malloc(sizeof(treeNode*) * rule_to_apply.size_rhs);
            current_tree_node->processed_children = 0;

            printf("Putting in rule : %d\n", rule_no + 1);
            for (k = rule_to_apply.size_rhs - 1; k >= 0; k--) {
                stackElem curr;
                curr.symbol_type = rule_to_apply.rhs[k];
                current_tree_node->children[k]
                    = (treeNode*) malloc(sizeof(treeNode));
                current_tree_node->children[k]->parent
                    = current_tree_node;
                current_tree_node->children[k]->symbol_type
                    = curr.symbol_type;

                if (curr.symbol_type == EPS) {
                    continue;
                }

                curr.t_node = current_tree_node->children[k];

                printf("Pushing to stack %d\n", curr.symbol_type);
                push(curr, s);
            }
        }
    }


    printParseTree(new_tree);

	// ALGO on pg. 227
	// OR
	// https://courses.cs.washington.edu/courses/cse401/04sp/slides/03b-LL1-example.pdf

}


int parser();
