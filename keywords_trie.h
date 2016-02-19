#ifndef KEYWORD_TRIE
#define KEYWORD_TRIE

#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

struct trieNode
{
	struct trieNode* children[26];
	int isFinal;
	int state_id;
};

typedef struct trieNode trieNode;

struct Trie {
	int size;
	trieNode* root;
};

typedef struct Trie Trie;


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