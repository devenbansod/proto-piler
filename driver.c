#include "common.h"
#include "lexerDef.h"
#include "lexer.h"

#include "keywordTrieDef.h"
#include "keywordTrie.h"

#include "stackDef.h"
#include "stack.h"
#include "treeDef.h"

#include "parser.h"

extern Trie *t;

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("*** ERROR : Incorrect usage! \n
            Correct syntax is ./compiler.out <in_file> <out_file>\n"
        );
        return 0;
    }
    parseTree *tree = NULL;

    // FILE *fp = fopen(argv[1], "r");

    // Initialization
    initKeywordTrie();

    // Run lexical analysis
    // lexicalAnalysis(fp, 4096);

    // Parse the code
    parseInputSourceCode(argv[1]);

    // Print the parse tree
    tree = par


    // Lexical analysis
    free(t);

    return 0;

}
