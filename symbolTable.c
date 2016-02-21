#include "common.h"
#include "symbolTableDef.h"

void insertSymbol(SymbolTable* st, char *id, int id_len, struct sym_value v);

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
SymbolTable* createSymbolTable(int size) {
    SymbolTable* st = (SymbolTable*)malloc(sizeof(SymbolTable));

    int i = 0;

    st->symbolArray = (symbol**)malloc(sizeof(symbol*) * size);
    for(i = 0; i < size; i++) {
        st->symbolArray[i] = NULL;
    }

    st->size = size;
    st->curr_size = 0;

    return st;
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
 * Create a new hashtable with double size
 * Rehash all the elements of original table
 * and insert into the new one
 * and return the new one
 *
 */
SymbolTable* rehash_table(SymbolTable *st, int new_size) {
    SymbolTable *newst = createSymbolTable(new_size);
    int i;
    symbol *curr;

    for (i = 0; i < st->size; i++) {
        if (st->symbolArray[i]) {
            for (curr = st->symbolArray[i]; curr != NULL; curr = curr->next){
                insertSymbol(newst, curr->id, curr->id_len, curr->v);
            }
        }
    }

    destroySymbolTable(st);

    return newst;
}

/*
 * Insert a symbol into symbol table
 *  - If not present, add it to the end of chain
 *  - Else, update the value
 *
 */
void insertSymbol(SymbolTable* st, char *id, int id_len, struct sym_value v) {

    int h_index = hash(id, id_len);
    symbol* curr = st->symbolArray[h_index];

    int i = 0, found = 0;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            curr->v = v;
            found = 1;
            break;
        }
        curr = curr->next;
        i++;
    }
    curr = (symbol*)malloc(sizeof(symbol));

    strcpy(curr->id, id);
    curr->id_len = id_len;
    curr->next = NULL;

    st->curr_size++;

    // If too much chaining spotted,
    // double the size
    if (i >= MAX_CHAIN_LENGTH) {
        st = rehash_table(st, st->size * 2);
    }
}

/*
 * Find a symbol in the symbol table
 *  - if present, a pointer to symbol is returned
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
