#include "common.h"
#include "lexerDef.h"
#include "lexer.h"

#include "keywordTrieDef.h"
#include "keywordTrie.h"

#include "stackDef.h"
#include "stack.h"

extern char **final_states;
extern Trie *t;

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("*** ERROR : Incorrect usage! Correct syntax is ./lexer <filename>\n");
        return 0;
    }

    // Initialization
    initStateNames();
    initKeywordTrie();

    // Run lexical analysis
    lexicalAnalysis(argv[1]);

    // Lexical analysis
    free(final_states);
    free(t);

    return 0;

}