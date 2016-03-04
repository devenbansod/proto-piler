#include "common.h"
struct grammarRule
{
    int lhs;
    int rhs[20];
    int is_eps;
    int size_rhs;
    int is_dollar;
};
typedef struct grammarRule grammarRule;
int start_non_terminal = 201;
// int start_non_terminal = 41;
int start_terminal = 1;
// int start_terminal = 27;
// int eps = 500;
int EPS = 500;
// int eps = 33;

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
            char* str = strdup(line);
            char* token = strsep(&str, "-");
            rule[i].lhs = atoi(token);
            token = strsep(&str, "-");
            char *right = strdup(token);
            int j = 0;
            while((token = strsep(&right, " "))) {            
                rule[i].rhs[j] = atoi(token);
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

void printGrammar(grammarRule* G, int count){
    // Routine to Print the grammar G
    int i;
    for (i = 0; i < count; ++i){
        printf("%d -> ", G[i].lhs);
        int j;
        for(j = 0; j < G[i].size_rhs;++j){
            printf("%d ", G[i].rhs[j]);
        }
        printf("\n");
    }
}

int isTerminal(int x){
  if ((x < start_non_terminal || x == EPS) && x > 0){
    return 1;
  }
  else{
    return 0;
  }
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

void createFirstSet(grammarRule* G, grammarRule* firstset, int fcount, int count){
    // printGrammar(G, count);
    // grammarRule firstset[count];
    int i;
    for(i = 0; i < count; ++i){
        int eps_found = 0;
        int k = 0;
        while(k < G[i].size_rhs){
            if(isTerminal(G[i].rhs[k])){
                int p = findIndex(firstset, G[i].lhs, fcount);
                // printf("Adding %d to %d \n", G[i].rhs[k], G[i].lhs);
                firstset[p].rhs[firstset[p].size_rhs] = G[i].rhs[k];
                firstset[p].size_rhs++;                
                break;
            }
            else if (!isTerminal(G[i].rhs[k])){
                int t = findIndex(firstset, G[i].lhs, fcount); 
                // printf("%d \n", G[i].rhs[k]);
                int p = findIndex(firstset, G[i].rhs[k], fcount);
                // firstset[p] is the set whose elements need to be copied to firstset[t]
                // printf("%d Elements corresponding to %d to be added : ", firstset[p].size_rhs, G[i].rhs[k]);
                int q;
                for(q = 0; q < firstset[p].size_rhs; ++q){
                    // printf("%d ", firstset[p].rhs[q]);
                    if(firstset[p].rhs[q] == EPS){
                        // printf("EPS FOUND\n");
                        eps_found = 1;
                    }else{
                        firstset[t].rhs[firstset[t].size_rhs] = firstset[p].rhs[q];
                        firstset[t].size_rhs++;
                    }
                }
                if(eps_found == 0){
                    break;
                }                
            }
            ++k;
        }
    }
    // printGrammar(firstset, fcount);    
}

int exists(int x, grammarRule* firstset, int count){
    int i;
    for(i = 0; i < count; ++i){
        if(x == firstset[i].lhs){
            return 1;
        }
    }
    return 0;
}

int followIsCalculated(grammarRule *G){
    // printf("%d \n", G->size_rhs);
    if(G->size_rhs == 0){
        return 0;
    }
    else{
        int p;
        for(p = 0; p < G->size_rhs; ++p){
            if(!isTerminal(G->rhs[p])){
                return 0;
            }
        }
    }
    return 1;
}

void createFollowSet(grammarRule* G, int count, grammarRule* firstset, int first_count,
    grammarRule* followset, int follow_count){
    int i = findIndex(followset, 201, follow_count);
    followset[i].rhs[0] = 54; // assign dollar store
    followset[i].size_rhs ++;
    int change = 1;
    // printf("%d %d\n", followset[i].lhs, followIsCalculated(&followset[i]));    
    while(change > 0){
        change = 0;
        for(i = 0; i < follow_count; ++i){
            int NT = followset[i].lhs;
            // printf("%d %d\n", followset[i].lhs, followIsCalculated(&followset[i]));
            if(followIsCalculated(&followset[i])){
                continue;
            }
            else{
                int j;
                for(j = 0; j < count; ++j){
                    // for every rule R in G
                    int k; for(k = 0; k < G[j].size_rhs; ++k){
                        if(G[j].rhs[k]==NT){
                            // printf("follow(%d), %d %d\n", NT, G[i].lhs, k);
                            if((k+1) < G[j].size_rhs){
                            //  if next exists
                                // printf("%d\n", G[j].size_rhs);
                                int temp = findIndex(firstset, G[j].rhs[k+1],first_count);
                                if(temp!=-1){
                                    int eps_found = 0;
                                    int q; for(q = 0; q < firstset[temp].size_rhs; ++q){
                                        if(firstset[temp].rhs[q] == EPS){
                                            eps_found = 1;
                                        }
                                        else{
                                            printf("Adding %d \t from first(%d) to \t follow(%d) \n",firstset[temp].rhs[q], firstset[temp].lhs, followset[i].lhs);
                                            followset[i].rhs[followset[i].size_rhs] = firstset[temp].rhs[q];
                                            followset[i].size_rhs++;
                                        }
                                    }
                                    if (eps_found == 1){
                                        int t_Index = findIndex(followset, G[j].rhs[k+1], follow_count);
                                        if(t_Index != -1){
                                            if(followIsCalculated(&followset[t_Index])){
                                                int r; for(r = 0 ; r < followset[t_Index].size_rhs; ++r){
                                                    printf("Adding %d \t from follow(%d) to \t follow(%d) \n",
                                                        firstset[t_Index].rhs[r], followset[t_Index].lhs, followset[i].lhs);
                                                    followset[i].rhs[followset[i].size_rhs] = followset[t_Index].rhs[r];
                                                    followset[i].size_rhs++;
                                                }
                                            }else{
                                                // follow is not calculated
                                                followset[i].rhs[followset[i].size_rhs+1] = followset[t_Index].lhs;
                                            }
                                        }
                                    }
                                }
                            }
                            else{
                            //     // if next does not exist
                            }
                        }
                    }
                }    
            }
        }
    }
}
int main(int argc, char const *argv[]){
    int i;
    int count = 100; //number of lines in the grammar, can be made read from file

    grammarRule rules[count];
    char *filename = "grammar2.txt";
    count = readFile(filename, rules);
    grammarRule firstset[count];
    int fcount = 0; 
    for(i = 0; i < count; ++i){
        if(!exists(rules[i].lhs, firstset, count)){
            int j;
            firstset[fcount].lhs = rules[i].lhs;
            firstset[fcount].size_rhs = 0;
            fcount++;
        }
    }
    grammarRule followset[count];
    int follow_count = 0;
    for(i = 0; i < count; ++i){
        if(!exists(rules[i].lhs, followset, count)){
            int j;
            followset[follow_count].lhs = rules[i].lhs;
            followset[follow_count].size_rhs = 0;
            follow_count++;
        }
    }
    createFirstSet(rules, firstset, fcount, count);
    createFollowSet(rules, count, firstset, fcount, followset,  follow_count);
    // printGrammar(firstset, fcount);
    printf("\n");
    return 0;
}