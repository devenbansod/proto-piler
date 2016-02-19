#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "keywords_trie.h"

#define MAX_FUNID_LEN 30
#define MAX_ID_LEN 20

char **final_states;
Trie *t;

void initKeywordTrie(Trie *t, char **final_states) {
    t->root = (trieNode*)malloc(sizeof(trieNode));
    int i = 0;
    for(i = 0; i < 26; i++) {
        t->root->children[i] = NULL;
    }
    t->size = 1;

    FILE* fp = fopen("keywords.txt", "r");

    i = 0;
    char temp_str[50];
    for (i = 61; i < 85; i++) {
        fscanf(fp, "%s", temp_str);
        insertKeyword(t, temp_str, strlen(temp_str), i);
    }

    fclose(fp);
}

void initStateNames(char **final_states) {

    strcpy(final_states[1] , "TK_SQL");
    strcpy(final_states[2] , "TK_SQR");
    strcpy(final_states[3] , "TK_SEM");
    strcpy(final_states[4] , "TK_COLON");
    strcpy(final_states[5] , "TK_DOT");
    strcpy(final_states[6] , "TK_OP");
    strcpy(final_states[7] , "TK_CL");
    strcpy(final_states[8] , "TK_PLUS");
    strcpy(final_states[9] , "TK_MINUS");
    strcpy(final_states[10], "TK_MUL");
    strcpy(final_states[11], "TK_DIV");
    strcpy(final_states[12], "TK_NOT");
    strcpy(final_states[14], "TK_NE");
    strcpy(final_states[16], "TK_EQ");
    strcpy(final_states[18], "TK_GE");
    strcpy(final_states[19], "TK_GT");
    strcpy(final_states[21], "TK_LE");
    strcpy(final_states[24], "TK_ASSIGNOP");
    strcpy(final_states[25], "TK_LT");
    strcpy(final_states[28], "TK_AND");
    strcpy(final_states[31], "TK_RECORDID");
    strcpy(final_states[34], "TK_NOT");
    strcpy(final_states[36], "TK_COMMENT");
    strcpy(final_states[40], "TK_RNUM");
    strcpy(final_states[41], "TK_NUM");
    strcpy(final_states[45], "TK_FUNID");
    strcpy(final_states[49], "TK_ID");
    strcpy(final_states[51], "TK_FIELDID");
    strcpy(final_states[52], "TK_COMMA");

    // keywords
    strcpy(final_states[61], "TK_WITH");
    strcpy(final_states[62], "TK_PARAMETERS");
    strcpy(final_states[63], "TK_END");
    strcpy(final_states[64], "TK_WHILE");
    strcpy(final_states[65], "TK_INT");
    strcpy(final_states[66], "TK_REAL");
    strcpy(final_states[67], "TK_TYPE");
    strcpy(final_states[68], "TK_MAIN");
    strcpy(final_states[69], "TK_GLOBAL");
    strcpy(final_states[70], "TK_PARAMETER");
    strcpy(final_states[71], "TK_LIST");
    strcpy(final_states[72], "TK_INPUT");
    strcpy(final_states[73], "TK_OUTPUT");
    strcpy(final_states[74], "TK_ENDWHILE");
    strcpy(final_states[75], "TK_IF");
    strcpy(final_states[76], "TK_THEN");
    strcpy(final_states[77], "TK_ENDIF");
    strcpy(final_states[78], "TK_READ");
    strcpy(final_states[79], "TK_WRITE");
    strcpy(final_states[80], "TK_RETURN");
    strcpy(final_states[81], "TK_RECORD");
    strcpy(final_states[82], "TK_ENDRECORD");
    strcpy(final_states[83], "TK_ELSE");
    strcpy(final_states[84], "TK_CALL");

}

int isAllExcept(char c, char next_char) {
    if (c == next_char) {
        return 0;
    } else if (isalpha(next_char) || isdigit(next_char) ? 1 : 0
        || next_char == ']' || next_char == '['
        || next_char == ';' || next_char == ':'
        || next_char == '.' || next_char == '('
        || next_char == ')' || next_char == '+'
        || next_char == '-' || next_char == '*'
        || next_char == '/' || next_char == '~'
        || next_char == '!' || next_char == '='
        || next_char == '<' || next_char == '>'
        || next_char == '&' || next_char == '@'
        || next_char == '-' || next_char == '%'
        || next_char == '\n' || next_char == ' '
        || next_char == '\t'
    ) {
        return 1;
    } else {
        return -1;
    }

}

int isSmallAlpha(char next_char) {
    if (tolower(next_char) == next_char
        && isalpha(next_char)
    ) {
        return 1;
    } else {
        return 0;
    }
}

int isBtoD(char next_char) {
    if (next_char >= 'b' && next_char <= 'd') {
        return 1;
    } else {
        return 0;
    }
}

int is2to7(char next_char) {
    if (next_char >= 50 && next_char <= 55) {
        return 1;
    } else {
        return 0;
    }
}



State getNextToken(
    char *buf, int buf_len, int *start, int *line, int *check_error,
    char *lexeme
) {
    State curr; curr.state_id = 0; curr.final = 0; curr.error = -1;
    int tmp, tmp2;
    int backup_start = *start;

    // loop till it is a final state
    // or a lexical error has occured
    while(curr.final == 0 && curr.error == -1 && *start < buf_len) {
        char next_char = buf[(*start)++];

        switch(curr.state_id) {
            // START state
            case 0:
                switch (next_char) {
                    case ' ':
                        backup_start++;
                        break;
                    case '[' :
                        curr.state_id = 1;
                        curr.final = 1;
                        break;
                    case ']' :
                        curr.state_id = 2;
                        curr.final = 1;
                        break;
                    case ';' :
                        curr.state_id = 3;
                        curr.final = 1;
                        break;
                    case ':' :
                        curr.state_id = 4;
                        curr.final = 1;
                        break;
                    case '.' :
                        curr.state_id = 5;
                        curr.final = 1;
                        break;
                    case '(' :
                        curr.state_id = 6;
                        curr.final = 1;
                        break;
                    case ')' :
                        curr.state_id = 7;
                        curr.final = 1;
                        break;
                    case '+' :
                        curr.state_id = 8;
                        curr.final = 1;
                        break;
                    case '-' :
                        curr.state_id = 9;
                        curr.final = 1;
                        break;
                    case '*' :
                        curr.state_id = 10;
                        curr.final = 1;
                        break;
                    case '/' :
                        curr.state_id = 11;
                        curr.final = 1;
                        break;
                    case '~' :
                        curr.state_id = 12;
                        curr.final = 1;
                        break;
                    case ',' :
                        curr.state_id = 52;
                        curr.final = 1;
                        break;
                    // ONLY SINGLE ENDS

                    case '!' :
                        curr.state_id = 13;
                        break;
                    case '=' :
                        curr.state_id = 15;
                        break;
                    case '>' :
                        curr.state_id = 17;
                        break;
                    case '<' :
                        curr.state_id = 20;
                        break;
                    case '&' :
                        curr.state_id = 26;
                        break;
                    case '#' :
                        curr.state_id = 29;
                        break;
                    case '@' :
                        curr.state_id = 32;
                        break;
                    case '%' :
                        curr.state_id = 35;
                        break;
                    case '_' :
                        curr.state_id = 42;
                        break;
                    // SIMPLE THINGS FINISH

                    case '\n':
                        (*line)++;
                        printf("\nLINE : %d\n", *line);
                        backup_start++;
                        break;
                    case EOF:
                        curr.state_id = 100;
                        curr.final = 1;
                        break;

                    default:
                        tmp = isdigit(next_char) ? 1 : 0;
                        switch (tmp) {
                            case 0:
                                tmp2 = isBtoD(next_char);
                                switch (tmp2) {
                                    case 0:
                                        curr.state_id = 50;
                                        break;
                                    case 1:
                                        curr.state_id = 46;
                                        break;
                                    default:
                                        curr.error = 1;
                                };
                                break;
                            case 1:
                                curr.state_id = 37;
                                break;
                            default:
                                curr.error = 1;
                        }

                };
                break;

            case 13:
                switch (next_char) {
                    case '=' :
                        curr.state_id = 14;
                        curr.final = 1;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 15:
                switch (next_char) {
                    case '=' :
                        curr.state_id = 16;
                        curr.final = 1;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 17:
                switch (next_char) {
                    case '=':
                        curr.state_id = 18;
                        curr.final = 1;
                        break;
                    default:
                        if (isAllExcept('=', next_char)) {
                            curr.state_id = 19;
                            curr.final = 1;
                            (*start)--;
                        } else {
                            curr.error = 1;
                        }
                };
                break;

            case 20:
                switch (next_char) {
                    case '=' :
                        curr.state_id = 21;
                        curr.final = 1;
                        break;
                    case '-' :
                        curr.state_id = 22;
                        next_char = buf[(*start)++];

                        switch (next_char) {
                            case '-':
                                curr.state_id = 23;
                                next_char = buf[(*start)++];
                                switch (next_char) {
                                    case '-':
                                        curr.state_id = 24;
                                        curr.final = 1;
                                        break;
                                    default:
                                        curr.error = 1;
                                }
                                break;
                            default:
                                curr.error = 1;
                        }
                        break;

                    default:
                        if (isAllExcept('-', next_char) && isAllExcept('=', next_char)) {
                            curr.state_id = 25;
                            curr.final = 1;
                            (*start)--;
                        } else {
                            curr.error = 1;
                        }
                };
                break;

            case 26:
                switch (next_char) {
                    case '&' :
                        curr.state_id = 27;
                        next_char = buf[(*start)++];

                        switch (next_char) {
                            case '&':
                                curr.state_id = 28;
                                curr.final = 1;
                                break;
                        }
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 29:
                tmp = isSmallAlpha(next_char);
                switch (tmp) {
                    case 1:
                        curr.state_id = 30;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 32:
                switch (next_char) {
                    case '@' :
                        curr.state_id = 33;
                        next_char = buf[(*start)++];
                        switch (next_char) {
                            case '@':
                                curr.state_id = 34;
                                curr.final = 1;
                                break;
                            default :
                                curr.error = 1;
                        }
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 35:
                switch (next_char) {
                    case '\n' :
                        curr.state_id = 36;
                        curr.final = 1;
                        (*start)--;
                        break;
                    default:
                        curr.error = -1;
                };
                break;

            case 30:
                tmp = isSmallAlpha(next_char);
                switch (tmp) {
                    case 1:
                        break;
                    case 0:
                        curr.state_id = 31;
                        curr.final = 1;
                        (*start)--;
                        break;
                    default:
                        curr.error = 1;

                };
                break;

            case 42:
                tmp = isalpha(next_char) > 0 ? 1 : 0;
                switch (tmp) {
                    case 0:
                        curr.error = 1;
                        break;
                    case 1:
                        curr.state_id = 43;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 43:
                if (*start - backup_start > MAX_FUNID_LEN) {
                    curr.error = 1;
                    break;
                }

                tmp = isalpha(next_char) > 0 ? 1 : 0;
                switch (tmp) {
                    case 0:
                        tmp2 = isdigit(next_char) ? 1 : 0;
                        switch(tmp2) {
                            case 0:
                                curr.state_id = 45;
                                curr.final = 1;
                                (*start)--;
                                break;
                            case 1:
                                curr.state_id = 44;
                                break;
                            default:
                                curr.error = 1;
                        }
                        break;
                    case 1:
                        curr.state_id = 43;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 44:
                if (*start - backup_start > MAX_FUNID_LEN) {
                    curr.error = 1;
                    break;
                }
                tmp = isdigit(next_char) ? 1 : 0;
                switch(tmp) {
                    case 0:
                        curr.state_id = 45;
                        curr.final = 1;
                        (*start)--;
                        break;
                    case 1:
                        curr.state_id = 44;
                        break;
                    default:
                        curr.state_id = 45;
                        (*start)--;
                        curr.final = 1;
                };
                break;

            case 37:
                tmp = isdigit(next_char) ? 1 : 0;
                switch (tmp) {
                    case 0:
                        tmp2 = isAllExcept('.', next_char);
                        switch (tmp2) {
                            case 0:
                                curr.state_id = 38;
                                break;
                            case 1:
                                curr.state_id = 41;
                                curr.final = 1;
                                (*start)--;
                                break;
                            default:
                                curr.error = 1;
                        };
                        break;
                    case 1:
                        curr.state_id = 37;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 38:
                tmp = isdigit(next_char) ? 1 : 0;
                switch (tmp) {
                    case 0:
                        curr.error = 1;
                        break;
                    case 1:
                        curr.state_id = 39;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 39:
                tmp = isdigit(next_char) ? 1 : 0;
                switch (tmp) {
                    case 0:
                        curr.error = 1;
                        break;
                    case 1:
                        curr.state_id = 40;
                        curr.final = 1;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 50:
                tmp = isSmallAlpha(next_char);
                switch (tmp) {
                    case 0:
                        curr.state_id = 51;
                        curr.final = 1;
                        (*start)--;
                        break;
                    case 1:
                        curr.state_id = 50;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 46:
                tmp = is2to7(next_char);
                switch (tmp) {
                    case 0:
                        tmp2 = isSmallAlpha(next_char);
                        if (tmp2) {
                            curr.state_id = 50;
                        } else {
                            curr.error = 1;
                        }
                        break;
                    case 1:
                        curr.state_id = 47;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 47:
                if (*start - backup_start > MAX_ID_LEN) {
                    curr.error = 1;
                    break;
                }
                tmp = isBtoD(next_char);
                switch (tmp) {
                    case 0:
                        tmp2 = is2to7(next_char);
                        if (tmp2) {
                            curr.state_id = 48;
                        } else {
                            curr.state_id = 49;
                            curr.final = 1;
                            (*start)--;
                        }
                        break;
                    case 1:
                        curr.state_id = 47;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            case 48:
                if (*start - backup_start > MAX_ID_LEN) {
                    curr.error = 1;
                    break;
                }
                tmp = is2to7(next_char);
                switch (tmp) {
                    case 0:
                        curr.state_id = 49;
                        curr.final = 1;
                        (*start)--;
                        break;
                    case 1:
                        curr.state_id = 47;
                        break;
                    default:
                        curr.error = 1;
                };
                break;

            default:
                curr.error = 1;
        }
    }

    // store the lexeme for the token
    int i, j;
    for (i = backup_start, j = 0; j < ((*start) - backup_start); i++, j++) {
        lexeme[j] = buf[i];
    }
    lexeme[j] = '\0';

    int new_sid;
    if ((curr.state_id == 45 || curr.state_id == 51)) {

        int k = checkIfKeyword(t, lexeme, j);
        if (k != -1) {
            curr.state_id = k;
            curr.error = -1;
            curr.final = 1;
        }
    }

    // if final state is not reached
    if (curr.final == 0 && *start < buf_len) {
        curr.error = 0;
    }
    return curr;

}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("*** ERROR : Incorrect usage! Correct syntax is ./lexer <filename>\n");
        return 0;
    }

    // Initialization
    final_states = (char**) malloc(sizeof(char*) * 100);
    int i = 0;
    for (i = 0; i < 100; i++) {
        final_states[i] = (char*)malloc(sizeof(char) * 20);
    }
    initStateNames(final_states);

    t = (Trie*)malloc(sizeof(Trie));
    initKeywordTrie(t, final_states);


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

    return 0;

}
