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

    // File operations
    int fd1 = open(argv[1], 0400), n;
    char read_char[32768];

    n = read(fd1, read_char, 32768);

    int start = 0;
    int line = 1;
    int check_error = 0;

    State a; a.error = -1;
    char lexeme[100];

    printf("\nLINE : %d\n", line);

    // read tokens and print them
    while (start < n) {
        strcat(read_char, "$");

        a = getNextToken(read_char, n + 1, &start, &line, &check_error, lexeme);

        if (a.error != -1) {
            printf("**** ERROR! INVALID TOKEN at %s ON LINE : %d\n", lexeme, line);
            break;
        }

        printf("`%s` : %s\n", lexeme, final_states[a.state_id]);
    }

    free(final_states);
    free(t);

    return 0;

}
