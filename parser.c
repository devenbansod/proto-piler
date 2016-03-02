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
            rules[i].lhs.NonT = atoi(token);
            rules[i].lhs.isTerminal = 0;

            token = strsep(&str, "-");
            char *right = strdup(token);
            int j = 0;
            while((token = strsep(&right, " "))) {
                if (atoi(token) < START_NON_TERMINAL) {
                    rules[i].rhs[j].Term = atoi(token);
                    rules[i].rhs[j].isTerminal = 1;
                } else {
                    rules[i].rhs[j].NonT = atoi(token);
                    rules[i].rhs[j].isTerminal = 0;
                }

                if (rules[i].rhs[j].NonT == EPS) {
                    rules[i].is_eps = 1;
                }
                if (rules[i].rhs[j].NonT == DOLLAR) {
                    rules[i].is_dollar = 1;
                    rules[i].rhs[j].Term = DOLLAR;
                    rules[i].rhs[j].NonT = -1;
                }
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
        NonTerminal x = G[i].lhs.NonT;
        int t;

        int k = 0;
        int eps_found = 0;
        int index;
        while (k < G[i].size_rhs) {
            if (G[i].rhs[k].NonT == EPS) {
                ++k;
                eps_found = 1;
                break;
            }

            if ((G[i].rhs[k].isTerminal
                && G[i].rhs[k].Term < START_NON_TERMINAL)
            ) {
                T[G[i].lhs.NonT - START_NON_TERMINAL][G[i].rhs[k].Term - START_TERMINAL]
                    = grammarRuleNumber;
                break;
            } else if (G[i].rhs[k].NonT == DOLLAR) {
                T[G[i].lhs.NonT - START_NON_TERMINAL][DOLLAR_STORE]
                    = grammarRuleNumber;
                break;
            } else {
                index = findMatchingSet(firstset, G[i].rhs[k].NonT, first_count);
                int j;
                for (j = 0; index >= 0 && j < firstset[index].size_rhs; ++j) {
                    if (firstset[index].rhs[j].NonT == EPS) {
                        eps_found = 1;
                    } else{
                        T[G[i].lhs.NonT - START_NON_TERMINAL][firstset[index].rhs[j].Term - START_TERMINAL]
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

termNonTerm getTermNonTerm(symbolType st, int error) {
	termNonTerm t;
    t.symbol_type.NonT = -1;
    t.symbol_type.Term = -1;
	t.isTerminal = st.isTerminal;

	if (st.isTerminal) {
		t.symbol_type.Term = st.Term;
        t.symbol_type.isTerminal = 1;
	} else {
		t.symbol_type.NonT = st.NonT;
        t.symbol_type.isTerminal = 0;
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
	new_node->t = getTermNonTerm(st, 0);
	new_node->curr_children = 0;
    new_node->processed_children = 0;

	return new_node;
}

void setTokenInfo(treeNode *t_node, tokenInfo tk_info) {
	t_node->tk_info = tk_info;
    t_node->parent->processed_children++;
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
    for (k = 0; k < rule->size_rhs; k++) {
        curr->children[k] = (treeNode *)malloc(sizeof(treeNode));
        curr->children[k]->parent = curr;
        curr->children[k]->curr_children = 0;

        curr->children[k]->t = getTermNonTerm(
            rule->rhs[k],
            0
        );
    }

    return curr;
}

treeNode* moveToNextNode(treeNode *current_tree_node) {
    int next_child = 0;

    while (current_tree_node
        && current_tree_node->curr_children <= current_tree_node->processed_children
    ) {
        current_tree_node = current_tree_node->parent;
        next_child = current_tree_node->processed_children;
    }

    return current_tree_node->children[next_child];
}


// void printParseTree(parseTree *) {

// }

parseTree* parseInputSourceCode(char *testcaseFile) {

    FileBuffer b;

    FILE *fp = fopen(testcaseFile, "r");
    fp = getStream(fp, &b, 4096);

    // read the grammar from file
    int rule_count = COUNT_RULE;
    FILE *grammar_file = fopen("./grammar_converted_2.txt", "r");
    grammarRule *G = readFileForRules(grammar_file, &rule_count);

    // FILE *first_file = fopen("./../Downloads/first_set_conv_saahil.txt", "r");
    FILE *follow_file = fopen("./follow_set_conv_3.txt", "r");
    FILE *first_file = fopen("./first_converted_final.txt", "r");
    // FILE *follow_file = fopen("follow_set_conv_3.txt", "r");

    parseTable **T;

    // create the parseTable from First and follow info
    T = createParseTable(G, rule_count, first_file, follow_file, T);

    printParseTable(stdout, T);

	// initialize new parse tree
	parseTree *new_tree;
	new_tree = createParseTree(new_tree);

	// // initialize the new stack
	Stack *s;
	s = createStack(s, 100);

	// push the EOS
	symbolType st;
    st.isTerminal = 1; st.Term = EOS;
	termNonTerm bottom = getTermNonTerm(st, 0);
	push(bottom, s);

	// push the start symbol
	st.isTerminal = 0; st.NonT = program;
	termNonTerm start_sym = getTermNonTerm(st, 0);
	push(start_sym, s);

	// create the Root Tree node with Start symbol
    treeNode* current_tree_node = getTreeNode(0, NULL, st);
    new_tree->root = current_tree_node;

    int error = 0;
    int line = 1;
    int moveAhead = 1;

    while (! isEmpty(s) && error == 0) {
        char lexeme[101];
        tokenInfo next_token; next_token.error = -1;
        memset(lexeme, '\0', 100);

        if (moveAhead) {
            next_token = getNextToken(&b, &line, lexeme);
            printf("READ TOKEN : %d -> `%s`\n", next_token.term_type, next_token.lexeme);
        }

        if (next_token.term_type == TK_COMMENT) {
            continue;
        } else if (next_token.error == 100) {
            printf("End of File\n");
            break;
        }

        termNonTerm top = pop(s);
        printf("POP : %d : %d\n", top.symbol_type.NonT, top.symbol_type.Term);

        moveAhead = 0;
        if (top.isTerminal
            || top.symbol_type.Term == EOS
        ) {
            // both are terminals
            if (top.symbol_type.Term == next_token.term_type) {
                moveAhead = 1;
                printf("Matched %d and Moved ahead\n", next_token.term_type);

                // match Leaf and put in tree
                setTokenInfo(current_tree_node, next_token);
                current_tree_node = moveToNextNode(current_tree_node);
            } else if (top.symbol_type.Term == EOS) {
                printf("End of Stack! ERROR!\n");
                break;
            } else {
                printf("End of File\n");
                break;
            }
        } else {
            if (top.symbol_type.Term == EOS) {
                printf("Error! Aborting\n");
                break;
            }
            int rule_no = T[top.symbol_type.NonT - START_NON_TERMINAL][next_token.term_type - START_TERMINAL];
            if (rule_no == -1) {
                printf("Error! Aborting since NO RULE FOUND for %d/%d -> %d (lexeme : `%s`)\n", top.symbol_type.NonT,
                    top.symbol_type.Term - START_NON_TERMINAL, next_token.term_type, next_token.lexeme);
                break;
            }
            printf("%d -> %d\n", top.symbol_type.NonT - START_NON_TERMINAL, next_token.term_type);
            grammarRule rule_to_apply = G[rule_no];
            int k = 0;

            if (rule_to_apply.lhs.NonT != top.symbol_type.NonT) {
                printf("Error! Rule %d does not match %d : %d!\n", rule_no, rule_to_apply.lhs.NonT, top.symbol_type.NonT);
                break;
            }
            printf("Putting in rule : %d\n", rule_no + 1);
            for (k = rule_to_apply.size_rhs - 1; k >= 0; k--) {
                termNonTerm curr;
                curr = getTermNonTerm(
                    rule_to_apply.rhs[k],
                    0
                );

                if (curr.symbol_type.NonT == EPS) {
                    continue;
                }

                printf("Pushing to stack %d : %d\n", curr.symbol_type.NonT, curr.symbol_type.Term);
                push(curr, s);
            }

            current_tree_node = applyRuleToTreeNode(
                current_tree_node, &rule_to_apply
            );
            current_tree_node = moveToNextNode(current_tree_node);
        }
    }



	// ALGO on pg. 227
	// OR
	// https://courses.cs.washington.edu/courses/cse401/04sp/slides/03b-LL1-example.pdf

}


int parser();
