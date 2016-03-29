/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "lexerDef.h"
#include "lexer.h"

#include "stackDef.h"
#include "stack.h"
#include "treeDef.h"

#include "parser.h"

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("*** ERROR : Incorrect usage! \n \
            Correct syntax is ./compiler.out <in_file> <out_file> \n"
        );
        return 0;
    }

    FILE *check_in_file = fopen(argv[1], "r");
    if (check_in_file == NULL) {
        fprintf(stderr, "*** ERROR : The input file : %s does not exist!\n", argv[1]);
        return -1;
    }

    int input = 0;
    FILE *fp = NULL;
    parseTree *tree = NULL;
    int error = 0;

    // Initialization
    initKeywordTrie();


    fprintf(stderr, "\n*** INFORMATION ***");
    fprintf(stderr, "\n\t* FIRST SET automated");
    fprintf(stderr, "\n\t* FOLLOW SET NOT automated");
    fprintf(stderr, "\n\t* Lexer  module implemented");
    fprintf(stderr, "\n\t* Parser module implemented");
    fprintf(stderr, "\n\t* Appropriate parse tree created, whenever possible");
    fprintf(stderr, "\n\t* Meaningful error reports generated");
    fprintf(stderr, "\n\t* Panic mode recovery from errors implemented");
    fprintf(stderr, "\n\t* No segmentation faults");
    fprintf(stderr, "\n\n");


    while (input != -1) {
        printf("Select an option :\n");
        printf("\t1. Print Comment-free source code\n");
        printf("\t2. Print Token stream\n");
        printf("\t3. Check for syntactic correctness\n");
        printf("\t4. Print the syntax tree\n");
        printf("\n\tEnter -1 to exit\n");

        scanf("%d", &input);

        switch (input) {
            case 1:
                printCommentFreeSource(argv[1]);
                fprintf(stderr, "\n");
                break;

            case 2:
                fp = fopen(argv[1], "r");
                lexicalAnalysis(fp, 4096);
                fclose(fp);
                fprintf(stderr, "\n");
                break;

            case 3:
                parseInputSourceCode(argv[1], &error);
                if (error == 0) {
                    fprintf(stderr, "\nThe input is syntactically valid\n\n");
                }

                error = 0;
                break;
            case 4:
                tree = parseInputSourceCode(argv[1], &error);

                if (error != 1) {
                    printParseTree(tree, argv[2]);
                    fprintf(stderr, "\nThe Parse tree has been printed at : %s\n\n", argv[2]);
                } else
                    fprintf(stderr, "\n*** ERROR! The Input could NOT be parsed!\n\n");

                error = 0;

                free(tree);
                break;

            case -1:
                fprintf(stderr, "Exiting ... \n");
                break;

            default:
                fprintf(stderr, "*** Incorrect input. Please enter correct input\n");
                break;
        }
    }

    return EXIT_SUCCESS;

}
