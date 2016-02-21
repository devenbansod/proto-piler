#ifndef KEYWORD_TRIE_DEF
#define KEYWORD_TRIE_DEF

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

#endif