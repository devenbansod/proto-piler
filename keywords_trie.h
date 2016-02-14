
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
	else return curr->state_id;
}

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
