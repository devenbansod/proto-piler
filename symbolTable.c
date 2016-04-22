/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "symbolTableDef.h"
#include "symbolTable.h"
#include "AST.h"

TypeTable *globalTT;
SymbolTable *globalST;
SymbolTable **allST;

/*
 * HASH FUNCTION
 *
 */
int hash(char* str, int len, int size) {
    int i = 0, sum = 0;

    for (i = 0; i < len; i++) {
        sum += str[i];
    }

    return sum % size;
}


/*
 * Create a new symbol table
 *
 */
SymbolTable* createSymbolTable(int size) {
    SymbolTable* st = (SymbolTable*)malloc(sizeof(SymbolTable));

    int i = 0;

    st->symbolArray = (symbolTableElem**)malloc(sizeof(symbolTableElem*) * size);
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
    symbolTableElem *curr, *prev;

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
SymbolTable* rehash_symbol_table(SymbolTable *st, int new_size) {
    SymbolTable *newst = createSymbolTable(new_size);
    int i;
    symbolTableElem *curr;

    for (i = 0; i < st->size; i++) {
        if (st->symbolArray[i]) {
            for (curr = st->symbolArray[i]; curr != NULL; curr = curr->next){
                insertSymbol(newst, curr->lexeme, curr->lex_len, curr->type, &curr->offset);
            }
        }
    }

    destroySymbolTable(st);

    return newst;
}

/*
 * Insert a symbol into symbol table
 *  - If not present, add it to the end of chain
 *  - Else, return -1 to indicate duplicate key error
 *
 */
int insertSymbol(SymbolTable* st, char* id, int id_len, char* type, int* offset) {

    int h_index = hash(id, id_len, st->size);
    symbolTableElem* curr = st->symbolArray[h_index];
    // symbolTableElem* temp = ;
    if (lookupSymbol(st, id, id_len) != NULL) {
        fprintf(stderr, "*** ERROR: Multiple declarations of %s in scope %s \n", id, st->scope);
        return -1;
    }

    int i = 0;
    while (curr) {
        if (strcmp(curr->lexeme, id) == 0) {
            return -1;
            break;
        }
        curr = curr->next;
        i++;
    }

    curr = (symbolTableElem *)malloc(sizeof(symbolTableElem));

    curr->lexeme = (char*)malloc((id_len+1) * sizeof(char));
    memset(curr->lexeme, '\0', id_len+1);

    strcpy(curr->lexeme, id);
    curr->lex_len = id_len;

    curr->type = (char*)malloc((strlen(type)+1) * sizeof(char));
    memset(curr->type, '\0', strlen(type)+1);

    strcpy(curr->type, type);
    curr->next = NULL;
    curr->offset = *offset;
    st->curr_size++;

    if (st->symbolArray[h_index] == NULL) {
        st->symbolArray[h_index] = curr;
    } else {
        curr->next = st->symbolArray[h_index];
        st->symbolArray[h_index] = curr;
    }

    typeTableElem* looked_up = lookupType(globalTT, type, strlen(type));
    if (looked_up) {
        for (i = 0; i < looked_up->fields_count; i++) {
            *offset += looked_up->width[i];
        }

        // for real and int
        if (looked_up->fields_count == 0) {
            if (strcmp(looked_up->type_name, "int") == 0) {
                *offset += INT_WIDTH;
            } else if (strcmp(looked_up->type_name, "real") == 0) {
                *offset += REAL_WIDTH;
            }
        }
    }

    // If too much chaining spotted,
    // double the size
    if (i >= MAX_CHAIN_LENGTH) {
        st = rehash_symbol_table(st, st->size * 2);
    }

    return 0;
}

/*
 * Find a symbol in the symbol table
 *  - if present, a pointer to symbol is returned
 *  - else, NULL is returned
 *
 */
symbolTableElem* lookupSymbol(SymbolTable* st, char *id, int id_len) {
    int h_index = hash(id, id_len, st->size);

    symbolTableElem* curr = st->symbolArray[h_index];

    while (curr != NULL) {

        if (strcmp(curr->lexeme, id) == 0) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}


void printSymbolTable(SymbolTable* st, char* scope) {
    int i;
    printf("------\t|-----\t|------\t|-------\n");
    printf("Lexeme\t| Type\t| Scope\t| Offset\n");
    printf("------\t|-----\t|------\t|-------\n");
    for (i = 0; i < st->size; ++i) {
        if (st->symbolArray[i] != NULL) {
            symbolTableElem* curr = st->symbolArray[i];
            while (curr != NULL) {
                printf("%s\t| %s\t| %s\t| %d\t\n", curr->lexeme, curr->type, scope, curr->offset);
                curr = curr->next;
            }
        }
    }
}


void printAllSymbolTables() {
    int j;

    for (j = 0; j < curr_number_backup; j++) {
        printSymbolTable(allST[j], allST[j]->scope);
    }

    // then print Global as well
    printSymbolTable(globalST, globalST->scope);
}

/*
 * TYPE TABLE Functions
 * =====================
 */

/*
 * Create a new type table
 *
 */
TypeTable* createTypeTable(int size) {
    TypeTable* typeTbl = (TypeTable*)malloc(sizeof(TypeTable));

    int i = 0;

    typeTbl->typeArray = (typeTableElem**)malloc(sizeof(typeTableElem*) * size);
    for(i = 0; i < size; i++) {
        typeTbl->typeArray[i] = NULL;
    }

    typeTbl->size = size;
    typeTbl->curr_size = 0;

    return typeTbl;
}

/*
 * Destroy a type table completely
 *
 */
void destroyTypeTable(TypeTable *typeTbl) {
    int i = 0;
    typeTableElem *curr, *prev;

    for(i = 0; i < MAX_BUCKETS; i++) {
        prev = typeTbl->typeArray[i];

        while (prev != NULL) {
            curr = prev->next;
            free(prev);
            prev = curr;
        }
    }

    free(typeTbl);
}

/*
 * Create a new hashtable with double size
 * Rehash all the elements of original table
 * and insert into the new one
 * and return the new one
 *
 */
TypeTable* rehash_type_table(TypeTable *typeTbl, int new_size) {
    TypeTable *newst = createTypeTable(new_size);
    int i;
    typeTableElem* curr = NULL;

    for (i = 0; i < typeTbl->size; i++) {
        if (typeTbl->typeArray[i]) {
            for (curr = typeTbl->typeArray[i]; curr != NULL; curr = curr->next){
                insertType(newst, curr->type_name, strlen(curr->type_name),
                    curr->width, curr->offset, curr->field_names,
                    curr->field_types, curr->fields_count
                );
            }
        }
    }

    destroyTypeTable(typeTbl);

    return newst;
}

/*
 * Insert a type into type table
 *  - If not present, add it to the end of chain
 *  - Else, return -1 to indicate duplicate key error
 *
 */
void printTypeTable(TypeTable *typeTbl){
    printf("h_index, type, width, offset, field_names, fields_count\n");
    int i;
    for(i = 0; i < typeTbl->size; ++i){
        if(typeTbl->typeArray[i]!=NULL){
            typeTableElem* curr = typeTbl->typeArray[i];
            while(curr!=NULL){
                printf("%d, %s, %d\n", i, curr->type_name, curr->fields_count);

                int i;
                for(i = 0; i < curr->fields_count; ++i){
                    printf("width: %d | offset: %d | field_names: %s | field_types: %s\n",
                        curr->width[i], curr->offset[i],
                        curr->field_names[i], curr->field_types[i]
                    );
                }
                curr = curr->next;
            }
        }
    }
}

int insertType(TypeTable* typeTbl, char* type, int id_len,
    int* width, int* offset, char** field_names,
    char** field_types, int fields_count
) {

    int h_index = hash(type, id_len, typeTbl->size);
    typeTableElem* curr = typeTbl->typeArray[h_index];

    if(lookupType(typeTbl, type, id_len)!=NULL){
        return -1;
    }
    curr = (typeTableElem *)malloc(sizeof(typeTableElem));
    memset(curr->type_name, '\0', 25);
    strcpy(curr->type_name, type);
    curr->type_len = id_len;

    curr->width = (int*)malloc(sizeof(int) * fields_count);
    curr->offset = (int*)malloc(sizeof(int) * fields_count);
    curr->field_names = (char**)malloc(sizeof(char*) * fields_count);
    curr->field_types = (char**)malloc(sizeof(char*) * fields_count);
    int i = 0;
    for(i = 0; i < fields_count; i++) {
        curr->width[i] = width[i];
        curr->offset[i] = offset[i];

        curr->field_names[i] = (char*)malloc(sizeof(char) * 25);
        memset(curr->field_names[i], '\0', 25);
        strcpy(curr->field_names[i], field_names[i]);

        curr->field_types[i] = (char*)malloc(sizeof(char) * 25);
        memset(curr->field_types[i], '\0', 25);
        strcpy(curr->field_types[i], field_types[i]);
    }
    curr->next = NULL;
    curr->fields_count = fields_count;

    typeTbl->curr_size++;

    if(typeTbl->typeArray[h_index]==NULL){
        typeTbl->typeArray[h_index] = curr;
    }else{
        curr->next = typeTbl->typeArray[h_index];
        typeTbl->typeArray[h_index] = curr;
    }

    // If too much chaining spotted,
    // double the size
    if (i >= MAX_CHAIN_LENGTH) {
        typeTbl = rehash_type_table(typeTbl, typeTbl->size * 2);
    }

    return 0;
}

/*
 * Find a type in the type table
 *  - if present, a pointer to type is returned
 *  - else, NULL is returned
 *
 */
typeTableElem* lookupType(TypeTable* typeTbl, char *id, int id_len) {
    int h_index = hash(id, id_len, typeTbl->size);
    typeTableElem* curr = typeTbl->typeArray[h_index];

    while (curr != NULL) {
        if (strcmp(curr->type_name, id) == 0) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

/*
 * FUNCTION TABLE Functions
 * =====================
 */

/*
 * Create a new function table
 *
 */
FunctionTable* createFunctionTable(int size) {
    FunctionTable* fxTable = (FunctionTable*)malloc(sizeof(FunctionTable));

    int i = 0;

    fxTable->functionArray = (functionTableElem**)malloc(sizeof(functionTableElem*) * size);
    for(i = 0; i < size; i++) {
        fxTable->functionArray[i] = NULL;
    }

    fxTable->size = size;
    fxTable->curr_size = 0;

    return fxTable;
}

/*
 * Destroy a type table completely
 *
 */
void destroyFunctionTable(FunctionTable *fxTable) {
    int i = 0;
    functionTableElem *curr = NULL, *prev = NULL;

    for(i = 0; i < fxTable->size; i++) {
        prev = fxTable->functionArray[i];

        while (prev != NULL) {
            curr = prev->next;
            free(prev);
            prev = curr;
        }
    }

    free(fxTable);
}

/*
 * Create a new hashtable with double size
 * Rehash all the elements of original table
 * and insert into the new one
 * and return the new one
 *
 */
FunctionTable* rehash_function_table(FunctionTable *fxTable, int new_size) {
    FunctionTable *newst = createFunctionTable(new_size);
    int i;
    functionTableElem* curr = NULL;

    for (i = 0; i < fxTable->size; i++) {
        if (fxTable->functionArray[i]) {
            for (curr = fxTable->functionArray[i]; curr != NULL; curr = curr->next){
                insertFunction(
                    newst, curr->id, strlen(curr->id),
                    curr->input_types, curr->output_types, curr->input_ids,
                    curr->output_ids, curr->input_len, curr->output_len
                );
            }
        }
    }

    destroyFunctionTable(fxTable);

    return newst;
}

/*
 * Insert a type into type table
 *  - If not present, add it to the end of chain
 *  - Else, return -1 to indicate duplicate key error
 *
 */
void printFunctionTable(FunctionTable *fxTable){
    printf("FUNCTION TABLE\n");
    int i;
    for(i = 0; i < fxTable->size; ++i){
        if(fxTable->functionArray[i]!=NULL){
            functionTableElem* curr = fxTable->functionArray[i];
            while(curr!=NULL){
                // print table elements here
                printf("id: %s, id_len: %d, input_len: %d, output_len: %d\n",
                    curr->id, curr->id_len, curr->input_len, curr->output_len
                );
                int i = 0;
                for (; i < curr->input_len; ++i){
                    printf("input_types[%d]=%s \n", i, curr->input_types[i]);
                    printf("input_ids[%d]=%s \n", i, curr->input_ids[i]);
                }
                for (; i < curr->output_len; ++i){
                    printf("output_types[%d]=%s \n", i, curr->output_types[i]);
                    printf("output_ids[%d]=%s \n", i, curr->output_ids[i]);
                }
                curr = curr->next;
            }
        }
    }
}

int insertFunction(FunctionTable *fxTable, char *id, int id_len,
    char **input_types, char **output_types, char **input_ids,
    char **output_ids, int input_len, int output_len
) {

    int h_index = hash(id, id_len, fxTable->size);
    functionTableElem* curr = fxTable->functionArray[h_index];

    int i = 0;
    if(lookupFunction(fxTable, id, id_len)!=NULL){
        fprintf(stderr, "*** ERROR: function %s already declared\n", id);
        return -1;
        // exit(-1);
    }
    curr = (functionTableElem *)malloc(sizeof(functionTableElem));
    curr->id = (char*)malloc(sizeof(char) * (id_len+1));
    memset(curr->id, '\0', id_len+1);
    strcpy(curr->id, id);
    curr->id_len = id_len;
    curr->input_len = input_len;
    curr->output_len = output_len;

    curr->input_ids = (char**)malloc(sizeof(char*) * input_len);
    curr->input_types = (char**)malloc(sizeof(char*) * input_len);
    curr->output_types = (char**)malloc(sizeof(char*) * output_len);
    curr->output_ids = (char**)malloc(sizeof(char*) * output_len);

    for(i = 0; i < input_len; i++) {
        curr->input_types[i] = (char*) malloc(sizeof(char)*25);
        memset(curr->input_types[i], '\0', 25);
        strcpy(curr->input_types[i], input_types[i]);

        curr->input_ids[i] = (char*) malloc(sizeof(char)*25);
        memset(curr->input_ids[i], '\0', 25);
        strcpy(curr->input_ids[i], input_ids[i]); 
    }

    for(i = 0; i< output_len; i++){
        curr->output_types[i] = (char*) malloc(sizeof(char)*25);
        memset(curr->output_types[i], '\0', 25);
        strcpy(curr->output_types[i], output_types[i]);

        curr->output_ids[i] = (char*) malloc(sizeof(char)*25);
        memset(curr->output_ids[i], '\0', 25);
        strcpy(curr->output_ids[i], output_ids[i]); 
    }

    curr->next = NULL;
    fxTable->curr_size++;

    if(fxTable->functionArray[h_index]==NULL){
        fxTable->functionArray[h_index] = curr;
    }else{
        curr->next = fxTable->functionArray[h_index];
        fxTable->functionArray[h_index] = curr;
    }
    // If too much chaining spotted,
    // double the size
    if (i >= MAX_CHAIN_LENGTH) {
        fxTable = rehash_function_table(fxTable, fxTable->size * 2);
    }
    return 0;
}

/*
 * Find a type in the type table
 *  - if present, a pointer to type is returned
 *  - else, NULL is returned
 *
 */
functionTableElem* lookupFunction(FunctionTable* fxTable, char *id, int id_len) {
    int h_index = hash(id, id_len, fxTable->size);
    functionTableElem* curr = fxTable->functionArray[h_index];
    while (curr != NULL) {
        if (strcmp(curr->id, id) == 0) {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}
