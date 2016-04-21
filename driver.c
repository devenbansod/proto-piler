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
#include "AST.h"
#include "typeChecker.h"
#include "symbolTable.h"
#include "codeGen.h"

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
    parseTree* AST = NULL;
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
        printf("\t1. Print Token stream\n");
        printf("\t2. Check for syntactic correctness and print Parse Tree\n");
        printf("\t3. Print the Abstract syntax tree\n");
        printf("\t4. Display compression statistics (from parse tree to AST)\n");
        printf("\t5. Print the symbol table\n");
        printf("\t6. Compile (Front-end only)\n");
        printf("\t7. Compile and Generate Assembly\n");
        printf("\n\tEnter -1 to exit\n");

        scanf("%d", &input);

        switch (input) {
            // case 1:
            //     printCommentFreeSource(argv[1]);
            //     fprintf(stderr, "\n");
            //     break;

            case 1:
                fp = fopen(argv[1], "r");
                lexicalAnalysis(fp, 4096);
                fclose(fp);
                fprintf(stderr, "\n");
                break;

            case 2:
                tree = parseInputSourceCode(argv[1], &error);
                if (error == 0) {
                    fprintf(stderr, "\nThe input is syntactically valid\n\n");
                    printParseTree(tree, stdout);
                } else {
                    fprintf(stderr, "\n*** ERROR! The Input could NOT be parsed!\n\n");
                    error = 0;
                    free(tree);
                }
                break;

            case 3:
                tree = parseInputSourceCode(argv[1], &error);
                if (error == 0) {
                    AST = (parseTree*)malloc(sizeof(parseTree));
                    AST->root = createAST(tree->root);
                    printParseTree(AST, stdout);
                } else {
                    fprintf(stderr, "\n*** ERROR! The Input could NOT be parsed!\n\n");
                    error = 0;
                    free(tree);
                }
                break;

            case 4:
                // calculate Compression Stats
                break;

            case 5:
                tree = parseInputSourceCode(argv[1], &error);
                if (error == 0) {
                    AST = (parseTree*)malloc(sizeof(parseTree));
                    AST->root = createAST(tree->root);
                    printf("aa rha\n");
                    printAllSymbolTables();
                } else {
                    fprintf(stderr, "\n*** ERROR! The Input could NOT be parsed!\n\n");
                    error = 0;
                    free(tree);
                }
                break;

            case 6:
                tree = parseInputSourceCode(argv[1], &error);
                if (error == 0) {
                    AST = (parseTree*)malloc(sizeof(parseTree));
                    AST->root = createAST(tree->root);
                    performTypeChecking(tree->root);
                } else {
                    fprintf(stderr, "\n*** ERROR! The Input could NOT be parsed!\n\n");
                    error = 0;
                    free(tree);
                }
                break;


            case 7:
                tree = parseInputSourceCode(argv[1], &error);
                if (error == 0) {
                    AST = (parseTree*)malloc(sizeof(parseTree));
                    AST->root = createAST(tree->root);
                    performTypeChecking(tree->root);

                    if (sem_error == 0) {
                        FILE *f = fopen(argv[2], "w");
                        generateProgram(AST->root, f);
                        fclose(f);
                    } else {
                        fprintf(stderr, "\n*** ERROR! The Input could NOT be semantically verified!\n\n");
                    }
                } else {
                    fprintf(stderr, "\n*** ERROR! The Input could NOT be parsed!\n\n");
                    error = 0;
                    free(tree);
                }
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
