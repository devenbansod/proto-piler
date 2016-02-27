#include "common.h"
#include "parserDef.h"

termNonTerm getTermNonTerm(int isTerminal, symbolType st, int error) {
	termNonTerm t;
	t.isTerminal = isTerminal;
	if (isTerminal) {
		t.st.b = st.a;
	} else {
		t.st.a = st.b;
	}

	t.error = error;

	return t;
}

treeNode* getTreeNode(
	int isTerminal,
	treeNode *parent, symbolType st
) {
	treeNode* new_node = (treeNode*)malloc(sizeof(treeNode));
	new_node->isTerminal = isTerminal;
	new_node->parent = parent;
	new_node->st = st;
	new_node->curr_children = 0;

	return new_node;
}

void setTokenInfo(treeNode *t_node, tokenInfo tk_info) {
	t_node->t = tk_info;
}

void createParseTree(parseTree *new_tree) {
	new_tree = (parseTree*)malloc(sizeof(parseTree));
	new_tree->root = NULL;
	new_tree->size = 0;
}


parseTree parseInputSourceCode(char *testcaseFile, parseTable T) {

	// initialize new parse tree
	parseTree *new_tree;
	createParseTree(new_tree);

	// initialize the new stack
	Stack *s;
	createStack(s);

	// push the EOS
	symbolType st; st.isTerminal = 0; st.a = EOS;
	termNonTerm bottom = getTermNonTerm(0, st, 0);
	push(bottom, s);

	// push the start symbol
	st.isTerminal = 0; st.a = program;
	termNonTerm start_sym = getTermNonTerm(0, st, 0);
	push(start_sym, s);

	// create the Root Tree node with Start symbol


	// ALGO on pg. 227
	// OR
	// https://courses.cs.washington.edu/courses/cse401/04sp/slides/03b-LL1-example.pdf

}


struct grammarRule
{
    int lhs;
    int rhs[20];
    int is_eps;
    int size_rhs;
    int is_dollar;
};

// else_or_not = 1005;
// product = 1002; arexp = 1003; pdash = 1004
typedef struct grammarRule grammarRule;

typedef int table;
char **final_states;
int count_terminal = 50; int count_non_terminal = 50;
int readFile(char* filename, grammarRule *rule){
    FILE * file = fopen(filename, "r");
    char* line;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    if(file){
        while ((read = getline(&line, &len, file)) != -1) {
            // printf("Retrieved line of length %zu :\n", read);
            // printf("%s\n", line);
            char* str = strdup(line);
            char* token = strsep(&str, "-");
            rule[i].lhs = atoi(token);
            // printf("%d\n", rule[i].lhs);
            // printf("LHS : %s\n", token);
            token = strsep(&str, "-");
            // token = strsep(&str, "=");
            // printf("%s", token);            
            char *right = strdup(token);
            int j = 0;
            while((token = strsep(&right, " "))) {
                // printf("%s ", token);
                rule[i].rhs[j] = atoi(token);
                // printf("%s %d \n", token, rule[i].rhs[j]);
    //             rule[i].is_eps = 0;
    //             rule[i].is_dollar = 0;
                if (strstr(token, "eps") != NULL) {
    // //                 // strstr checks if string contains "eps" instead of exact match. 
    // //                 // needed because eps might be trailing with newline and/or space characters
                    rule[i].is_eps = 1;
                }
                if (strstr(token, "$") != NULL) {
                    rule[i].is_dollar = 1;
                }
                ++j;
                rule[i].size_rhs = j;
            }
            ++i;
        }
    }
    else{
        printf("Error opening file\n");
        return 0;
    }
    fclose(file);
    return i; //number of grammar rules
}


// int readPosition(char* filename, char* string[]){
//     // input: filename, pointer where the characters are to be stored
//     // output: characters stored
//     // output: number of terminals-1
//     FILE * file = fopen(filename, "r");
//     char* line;
//     size_t len = 0;
//     ssize_t read;
//     int i = 0;
//     if(file){
//         while ((read = getline(&line, &len, file)) != -1) {
//             // printf("Retrieved line of length %zu :\n", read);
//             // printf("%s\n", line);
//             string[i] = strdup(line);
//             ++i;
//         }
//     }
//     return i;
// }

// int returnPosition(char* c, char* string[], int count){
//     // input: char* c, the string we are looking for
//     // input: char* string, the string in which position is to be found
//     int i = 0;
//     while(i<count){
//         // printf("%s, %s, %d\n", string[i], c, i);
//         if(strstr(string[i], c)!=NULL){
//             // printf("%s | %s | returning %d \n", string[i], c, i);
//             return i;
//         }
//         ++i;
//     }
// }
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



void createParseTable(grammarRule* G, int count, char* firstset_filename, char* followset_filename){
    
    // Declaring the able inside the function as a temporary hack
    int i ;
    int count_non_terminal = 5;
    int count_terminal = 7;
    table T[count_non_terminal][count_terminal];
    for (i = 0; i < count_non_terminal; ++i){
        int j = 0;
        for (j = 0; j < count_terminal; ++j)
        {
            T[i][j] = -1;
        }
    }
    // Routine to read in the first and follow sets
    grammarRule firstset[count];
    int first_count = readFile(firstset_filename, firstset);
    grammarRule followset[count];
    int follow_count = readFile(followset_filename, followset);
    printf("Grammar:\n");
    printGrammar(G, count);
    printf("firstset:\n");printGrammar(firstset, first_count);
    printf("followset\n");printGrammar(followset, follow_count);
    

    // Routine to populate the parse table with grammar rule numbers
    // i = 0;
    printf("populating the parse table\n");
    for (i = 0; i < count; ++i){
    	int grammarRuleNumber = i;
    	int x = G[i].lhs; //x, y is the address where the grammar rule will enter
    	printf("%d\n", G[i].lhs);
    	int j;
    	for (j = 0; j < first_count; ++j){
    		if(firstset[j].lhs == G[i].lhs){
    			printf("First is: ");
    			int k;
    			for(k = 0; k < firstset[j].size_rhs; ++k){
    				printf("%d ", firstset[j].rhs[k]);
    				int y = firstset[j].rhs[k];
    				T[x-41][y-27] = i;

    				if (y==33) //eps == 33
    				{
    					int p;
    					for (p = 0; p < follow_count; ++p){
			    			if (followset[p].lhs == G[i].lhs){
				    			printf("Follow is: ");
	    						int q;
	    						for(q = 0; q < followset[p].size_rhs; ++q){
	    							printf("%d ", followset[p].rhs[q]);
	    						}
			    			}
    					}
    				}
    			}
    			printf("\n");
    		}
    	}
    }    
    // routine to print the damn table
    for(i = 0; i < count_non_terminal; ++i){
        int j;
        for(j = 0; j < count_terminal; ++j){
            printf("%d\t", T[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char const *argv[]){
    int i;
    int count = 100; //number of lines in the grammar, can be made read from file

    grammarRule rules[count];
    char *filename = "unused_docs/grammar1_converted.txt";
    count = readFile(filename, rules);
    // int i;

    // char *terminals[count_terminal], *non_terminals[count_non_terminal];
    // count_terminal = readPosition("grammar1_terminals.txt", terminals);
    // for(i = 0; i < count_terminal; ++i){
    //     // printf("%d: %s", i, terminals[i]);
    // }

    // printf("\n%d Terminals Read\n", count_terminal);
    // count_non_terminal = readPosition("grammar1_non_terminals.txt", non_terminals);
    // for(i = 0; i < count_non_terminal; ++i){
    //     // printf("%d: %s", i, non_terminals[i]);
    // }

    // printf("\n%d Non-Terminals Read\n", count_non_terminal);
    

    // Hard coded test cases | Do not clear
    // printf("+ :%d\n", returnPosition("+", terminals, count_terminal));
    // printf("* :%d\n", returnPosition("*", terminals, count_terminal));
    // printf("( :%d\n", returnPosition("(", terminals, count_terminal));
    // printf(") :%d\n", returnPosition(")", terminals, count_terminal));
    // printf("id :%d\n", returnPosition("id", terminals, count_terminal));
    // printf("$ :%d\n", returnPosition("$", terminals, count_terminal));
    // printf("eps :%d\n", returnPosition("eps", terminals, count_terminal));

    // printf("E :%d\n", returnPosition("E ", non_terminals, count_non_terminal));
    // printf("E' :%d\n", returnPosition("E' ", non_terminals, count_non_terminal));
    // printf("F :%d\n", returnPosition("F ", non_terminals, count_non_terminal));
    // printf("T :%d\n", returnPosition("T ", non_terminals, count_non_terminal));
    // printf("T' :%d\n", returnPosition("T' ", non_terminals, count_non_terminal));
    
    createParseTable(rules, count, "unused_docs/grammar1_first.txt", "unused_docs/grammar1_follow.txt");
    return 0;
}
