/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "parserDef.h"


int isTerminal(int x){
  if ((x < START_NON_TERMINAL || x == EPS) && x > 0){
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

/*
 * Calculate First sets
 *
 */
void createFirstSet(grammarRule* G, grammarRule* firstset, int fcount, int count){

    int i;
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

int exists(int x, grammarRule* firstset, int count) {
    int i;
    for (i = 0; i < count; ++i) {
        if (x == firstset[i].lhs) {
            return 1;
        }
    }
    return 0;
}

int followIsCalculated(grammarRule *G) {

    if (G->size_rhs == 0){
        return 0;
    } else{
        int p;
        for(p = 0; p < G->size_rhs; ++p){
            if(! isTerminal(G->rhs[p])){
                return 0;
            }
        }
    }

    return 1;
}


/*
 * Calculate Follow sets - NOT implemented completely
 *
 */
void createFollowSet(grammarRule* G, int count, grammarRule* firstset, int first_count,
    grammarRule* followset, int follow_count){

    int i = findIndex(followset, START_NON_TERMINAL, follow_count);
    followset[i].rhs[0] = 54; // assign dollar store
    followset[i].size_rhs ++;
    int change = 1;

    while (change > 0) {
        change = 0;
        for(i = 0; i < follow_count; ++i){
            int NT = followset[i].lhs;

            if(followIsCalculated(&followset[i])){
                continue;
            } else{
                int j;
                for (j = 0; j < count; ++j) {
                    // for every rule R in G
                    int k;
                    for (k = 0; k < G[j].size_rhs; ++k) {
                        if (G[j].rhs[k] == NT) {
                            if ((k+1) < G[j].size_rhs) {
                            //  if next exists
                                int temp = findIndex(firstset, G[j].rhs[k+1],first_count);
                                if (temp!=-1) {
                                    int eps_found = 0;
                                    int q;
                                    for (q = 0; q < firstset[temp].size_rhs; ++q) {
                                        if (firstset[temp].rhs[q] == EPS) {
                                            eps_found = 1;
                                        } else{
                                            printf("Adding %d \t from first(%d)"
                                                " to \t follow(%d) \n",
                                                firstset[temp].rhs[q],
                                                firstset[temp].lhs, followset[i].lhs
                                            );

                                            followset[i].rhs[followset[i].size_rhs] = firstset[temp].rhs[q];
                                            followset[i].size_rhs++;
                                        }
                                    }
                                    if (eps_found == 1) {
                                        int t_Index = findIndex(followset, G[j].rhs[k+1], follow_count);

                                        if (t_Index != -1) {
                                            if (followIsCalculated(&followset[t_Index])) {
                                                int r;
                                                for (r = 0; r < followset[t_Index].size_rhs; ++r) {
                                                    printf("Adding %d \t from follow(%d) to \t follow(%d) \n",
                                                        firstset[t_Index].rhs[r],
                                                        followset[t_Index].lhs, followset[i].lhs
                                                    );

                                                    followset[i].rhs[followset[i].size_rhs] = followset[t_Index].rhs[r];
                                                    followset[i].size_rhs++;
                                                }
                                            } else{
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
