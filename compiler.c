#include "common.h"
#include "lexer.h"
#include "keywords_trie.h"
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
