#ifndef KEYWORD_TRIE_INTERFACE
#define KEYWORD_TRIE_INTERFACE
#include "keywordTrieDef.h"
/*
 * Interface to keyword_trie
 *
 */

// if present returns state_id, else returns -1
int checkIfKeyword(Trie* t, char *str, int len);

// For initiliasation from file I/O
void insertKeyword(Trie* t, char *str, int len, int value);

void initKeywordTrie();

#endif