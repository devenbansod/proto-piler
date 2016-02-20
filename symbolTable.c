#include "common.h"
#include "symbolTable.h"


/*
 * HASH FUNCTION
 *
 */
int hash(char *str, int len) {
	int i = 0, sum = 0;

	for (i = 0; i < len; i++) {
		sum += str[i];
	}

	return sum % MAX_BUCKETS;
}


/*
 * Create a new symbol table
 *
 */
void createSymbolTable(SymbolTable* st) {
	st = (SymbolTable*)malloc(sizeof(SymbolTable));

	int i = 0;
	for(i = 0; i < MAX_BUCKETS; i++) {
		st->symbolArray[i] = NULL;
	}

	st->size = 0;
}

/*
 * Destroy a symbol table completely
 *
 */
void destroySymbolTable(SymbolTable *st) {
	int i = 0;
	symbol *curr, *prev;

	for(i = 0; i < MAX_BUCKETS; i++) {
		prev = st->symbolArray[i];

		while (prev != NULL) {
			curr = prev->next;
			free(prev);
			prev = curr;
		}
	}

	free(st);
}

/*
 * Insert a symbol into symbol table
 *
 */
void insertSymbol(SymbolTable* st, char *id, int id_len) {
	symbol *new_s = (symbol*)malloc(sizeof(symbol));

	strcpy(new_s->id, id);
	new_s->id_len = id_len;

	int h_index = hash(id, id_len);

	if (st->symbolArray[h_index]) {
		new_s->next = st->symbolArray[h_index];
	} else {
		new_s->next = NULL;
	}
	st->symbolArray[h_index] = new_s;

	st->size++;
}


/*
 * Find a symbol in the symbol table
 *	- if present, a pointer to symbol is returned
 *  - else, NULL is returned
 *
 */
symbol* lookupSymbol(SymbolTable* st, char *id, int id_len) {
	int h_index = hash(id, id_len);
	symbol* curr = st->symbolArray[h_index];

	while (curr != NULL) {
		if (strcmp(curr->id, id) == 0) {
			return curr;
		}
		curr = curr->next;
	}

	return NULL;
}
