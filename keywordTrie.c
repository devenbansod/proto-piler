#include "common.h"
#include "keywordTrieDef.h"

Trie *t;

void insertKeyword(Trie* t, char *str, int len, int value) {
    int i = 0, j, k;
    trieNode* curr = t->root;

    for(i = 0; i < len; i++) {
        j = CHAR_TO_INDEX(str[i]);
        if (curr->children[j] == NULL) {
            // add a new node and initialise its children
            curr->children[j] = (trieNode*)malloc(sizeof(trieNode));
            for(k = 0; k < 26; k++) {
                curr->children[j]->children[k] = NULL;
            }
        }
        curr = curr->children[j];
    }

    curr->state_id = value;

    t->size++;
    return;
}


/*
 * Initialise the keywords in Trie
 *
 * - used for lookup to check if given TK_FUNID or TK_ID
 *   is a keyword or not
 */
void initKeywordTrie() {
    t = (Trie*)malloc(sizeof(Trie));
    t->root = (trieNode*)malloc(sizeof(trieNode));
    int i = 0;
    for(i = 0; i < 26; i++) {
        t->root->children[i] = NULL;
    }
    t->size = 1;

    FILE* fp = fopen("keywords.txt", "r");

    i = 0;
    char temp_str[50];
    for (i = 61; i < 85; i++) {
        fscanf(fp, "%s", temp_str);
        insertKeyword(t, temp_str, strlen(temp_str), i);
    }

    fclose(fp);
}

// if present returns state_id, else returns -1
int checkIfKeyword(Trie* t, char *str, int len) {
    int i = 0, j, k;
    trieNode* curr = t->root;

    for(i = 0; i < len; i++) {
        j = CHAR_TO_INDEX(str[i]);

        if (curr->children[j] == NULL) {
            return -1;
        }

        curr = curr->children[j];
    }

    if (curr == NULL) return -1;
    else {
        return curr->state_id;
    }
}
