#include "common.h"
#include "keywordTrieDef.h"
#include "fileBufferDef.h"
#include "lexerDef.h"

#define MAX_FUNID_LEN 30
#define MAX_ID_LEN 20

#define STR_CPY_STATE_NAME(X) strcpy(final_states[X], #X)

char **final_states;
extern Trie *t;

/*
 * Initialise the names of keywords
 *
 */
void initStateNames() {
    final_states = (char**) malloc(sizeof(char*) * 100);

    int i = 0;
    for (i = 0; i < 100; i++) {
        final_states[i] = (char*)malloc(sizeof(char) * 20);
    }

    STR_CPY_STATE_NAME(TK_SQL);
    STR_CPY_STATE_NAME(TK_SQR);
    STR_CPY_STATE_NAME(TK_SEM);
    STR_CPY_STATE_NAME(TK_COLON);
    STR_CPY_STATE_NAME(TK_DOT);
    STR_CPY_STATE_NAME(TK_OP);
    STR_CPY_STATE_NAME(TK_CL);
    STR_CPY_STATE_NAME(TK_PLUS);
    STR_CPY_STATE_NAME(TK_MINUS);
    STR_CPY_STATE_NAME(TK_MUL);
    STR_CPY_STATE_NAME(TK_DIV);
    STR_CPY_STATE_NAME(TK_NOT);
    STR_CPY_STATE_NAME(TK_NE);
    STR_CPY_STATE_NAME(TK_EQ);
    STR_CPY_STATE_NAME(TK_GE);
    STR_CPY_STATE_NAME(TK_GT);
    STR_CPY_STATE_NAME(TK_LE);
    STR_CPY_STATE_NAME(TK_ASSIGNOP);
    STR_CPY_STATE_NAME(TK_LT);
    STR_CPY_STATE_NAME(TK_AND);
    STR_CPY_STATE_NAME(TK_RECORDID);
    STR_CPY_STATE_NAME(TK_NOT);
    STR_CPY_STATE_NAME(TK_COMMENT);
    STR_CPY_STATE_NAME(TK_RNUM);
    STR_CPY_STATE_NAME(TK_NUM);
    STR_CPY_STATE_NAME(TK_FUNID);
    STR_CPY_STATE_NAME(TK_ID);
    STR_CPY_STATE_NAME(TK_FIELDID);
    STR_CPY_STATE_NAME(TK_COMMA);

    // keywords
    STR_CPY_STATE_NAME(TK_WITH);
    STR_CPY_STATE_NAME(TK_PARAMETERS);
    STR_CPY_STATE_NAME(TK_END);
    STR_CPY_STATE_NAME(TK_WHILE);
    STR_CPY_STATE_NAME(TK_INT);
    STR_CPY_STATE_NAME(TK_REAL);
    STR_CPY_STATE_NAME(TK_TYPE);
    STR_CPY_STATE_NAME(TK_MAIN);
    STR_CPY_STATE_NAME(TK_GLOBAL);
    STR_CPY_STATE_NAME(TK_PARAMETER);
    STR_CPY_STATE_NAME(TK_LIST);
    STR_CPY_STATE_NAME(TK_INPUT);
    STR_CPY_STATE_NAME(TK_OUTPUT);
    STR_CPY_STATE_NAME(TK_ENDWHILE);
    STR_CPY_STATE_NAME(TK_IF);
    STR_CPY_STATE_NAME(TK_THEN);
    STR_CPY_STATE_NAME(TK_ENDIF);
    STR_CPY_STATE_NAME(TK_READ);
    STR_CPY_STATE_NAME(TK_WRITE);
    STR_CPY_STATE_NAME(TK_RETURN);
    STR_CPY_STATE_NAME(TK_RECORD);
    STR_CPY_STATE_NAME(TK_ENDRECORD);
    STR_CPY_STATE_NAME(TK_ELSE);
    STR_CPY_STATE_NAME(TK_CALL);

}


/*
 * HELPER FUNCTIONS
 *
 */
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


void reportError (FILE *fp, int error_code, tokenInfo err_tok) {
    char error[100];
    switch (error_code) {
        case -1 :
        case 100:
        case 50 :
            return;

        case 1  :
            fprintf(stderr, "*** ERROR 1: Identifier `%s` at line : %d "
                "is longer than maximum prescribed length of 20 characters\n",
                err_tok.lexeme, err_tok.line_no
            );
            break;
        case 2  :
            fprintf(stderr, "*** ERROR 2: Unknown symbol `%s` at line %d\n",
                err_tok.lexeme, err_tok.line_no
            );
            break;
        case 3  :
            fprintf(stderr, "*** ERROR 3: Unknown pattern `%s` at line %d\n",
                err_tok.lexeme, err_tok.line_no
            );
            break;
        case 4  :
            break;

        default:
            return;
    }
}


FILE* getStream(FILE *fp, FileBuffer *b, int k) {
    b->fp = fp;
    b->size_of_buffer = k + 1;

    b->buf1 = (char*)malloc(sizeof(char) * k);
    b->buf2 = (char*)malloc(sizeof(char) * k);

    reloadBuffer(b, 1);
    b->curBuf = 1;
    b->current = b->buf1;
    return b->fp;
}


tokenInfo getNextToken(
    FileBuffer *buf,
    int *line,
    char *lexeme
) {
    tokenInfo curr; curr.state_id = 0; curr.final = 0; curr.error = -1;
    int tmp, tmp2, cur_len = 0;
    char *backup_start = buf->current;

    int concatChar = 1, moveAhead = 1;
    // loop till it is a final tokenInfo
    // or a lexical error has occured

    while (curr.final == 0 && curr.error == -1) {
        char next_char = readChar(buf);

        if (next_char == '\0') {
            continue;
        }

        switch (curr.state_id) {
            // START tokenInfo
            case 0:
                switch (next_char) {
                    case '\t':
                    case ' ' :
                        concatChar = 0;
                        break;
                    case '[' :
                        curr.state_id = TK_SQL;
                        curr.final = 1;
                        break;
                    case ']' :
                        curr.state_id = TK_SQR;
                        curr.final = 1;
                        break;
                    case ';' :
                        curr.state_id = TK_SEM;
                        curr.final = 1;
                        break;
                    case ':' :
                        curr.state_id = TK_COLON;
                        curr.final = 1;
                        break;
                    case '.' :
                        curr.state_id = TK_DOT;
                        curr.final = 1;
                        break;
                    case '(' :
                        curr.state_id = TK_OP;
                        curr.final = 1;
                        break;
                    case ')' :
                        curr.state_id = TK_CL;
                        curr.final = 1;
                        break;
                    case '+' :
                        curr.state_id = TK_PLUS;
                        curr.final = 1;
                        break;
                    case '-' :
                        curr.state_id = TK_MINUS;
                        curr.final = 1;
                        break;
                    case '*' :
                        curr.state_id = TK_MUL;
                        curr.final = 1;
                        break;
                    case '/' :
                        curr.state_id = TK_DIV;
                        curr.final = 1;
                        break;
                    case '~' :
                        curr.state_id = TK_NOT;
                        curr.final = 1;
                        break;
                    case ',' :
                        curr.state_id = TK_COMMA;
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
                    case '$' :
                        curr.error = 100;
                        break;

                    // SIMPLE THINGS FINISH
                    case '\r':
                    case '\n':
                        (*line)++;
                        moveAhead = 1;
                        concatChar = 0;
                        break;
                    case EOF:
                        curr.state_id = 100;
                        curr.final = 1;
                        concatChar = 0;
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
                                        curr.error = 3;
                                };
                                break;
                            case 1:
                                curr.state_id = 37;
                                break;
                            default:
                                curr.error = 3;
                        }

                };
                break;

            case 13:
                switch (next_char) {
                    case '=' :
                        curr.state_id = TK_NE;
                        curr.final = 1;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 15:
                switch (next_char) {
                    case '=' :
                        curr.state_id = TK_EQ;
                        curr.final = 1;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 17:
                switch (next_char) {
                    case '=':
                        curr.state_id = TK_GE;
                        curr.final = 1;
                        break;
                    default:
                        if (isAllExcept('=', next_char)) {
                            curr.state_id = TK_GT;
                            curr.final = 1;
                            moveAhead = 0;
                            concatChar = 0;
                        } else {
                            curr.error = 3;
                        }
                };
                break;

            case 20:
                switch (next_char) {
                    case '=' :
                        curr.state_id = TK_LE;
                        curr.final = 1;
                        break;
                    case '-' :
                        curr.state_id = 22;
                        moveForward(buf);
                        lexeme[cur_len++] = next_char;

                        next_char = readChar(buf);
                        switch (next_char) {
                            case '-':
                                curr.state_id = 23;
                                lexeme[cur_len++] = next_char;
                                moveForward(buf);

                                next_char = readChar(buf);
                                switch (next_char) {
                                    case '-':
                                        curr.state_id = TK_ASSIGNOP;
                                        curr.final = 1;
                                        break;
                                    default:
                                        curr.error = 3;
                                }
                                break;
                            default:
                                curr.error = 3;
                        }
                        break;

                    default:
                        if (isAllExcept('-', next_char) && isAllExcept('=', next_char)) {
                            curr.state_id = TK_LT;
                            curr.final = 1;
                            moveAhead = 0;
                            concatChar = 0;
                        } else {
                            curr.error = 3;
                        }
                };
                break;

            case 26:
                switch (next_char) {
                    case '&' :
                        curr.state_id = 27;
                        lexeme[cur_len++] = next_char;
                        moveForward(buf);

                        next_char = readChar(buf);
                        switch (next_char) {
                            case '&':
                                curr.state_id = TK_AND;
                                curr.final = 1;
                                break;
                        }
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 29:
                tmp = isSmallAlpha(next_char);
                switch (tmp) {
                    case 1:
                        curr.state_id = 30;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 32:
                switch (next_char) {
                    case '@' :
                        curr.state_id = 33;
                        lexeme[cur_len++] = next_char;
                        moveForward(buf);

                        next_char = readChar(buf);
                        switch (next_char) {
                            case '@':
                                curr.state_id = TK_OR;
                                curr.final = 1;
                                break;
                            default :
                                curr.error = 3;
                        }
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 35:
                switch (next_char) {
                    case '\n' :
                    case '\r' :
                        curr.state_id = TK_COMMENT;
                        curr.final = 1;
                        moveAhead = 0;
                        concatChar = 0;
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
                        curr.state_id = TK_RECORDID;
                        curr.final = 1;
                        moveAhead = 0;
                        concatChar = 0;
                        break;
                    default:
                        curr.error = 3;

                };
                break;

            case 42:
                tmp = isalpha(next_char) > 0 ? 1 : 0;
                switch (tmp) {
                    case 0:
                        curr.error = 3;
                        break;
                    case 1:
                        curr.state_id = 43;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 43:
                tmp = isalpha(next_char) > 0 ? 1 : 0;
                switch (tmp) {
                    case 0:
                        tmp2 = isdigit(next_char) ? 1 : 0;
                        switch(tmp2) {
                            case 0:
                                curr.state_id = TK_FUNID;
                                curr.final = 1;
                                moveAhead = 0;
                                concatChar = 0;
                                break;
                            case 1:
                                curr.state_id = 44;
                                break;
                            default:
                                curr.error = 3;
                        }
                        break;
                    case 1:
                        curr.state_id = 43;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 44:
                tmp = isdigit(next_char) ? 1 : 0;
                switch(tmp) {
                    case 0:
                        curr.state_id = TK_FUNID;
                        curr.final = 1;
                        moveAhead = 0;
                        concatChar = 0;
                        break;
                    case 1:
                        curr.state_id = 44;
                        break;
                    default:
                        curr.state_id = TK_FUNID;
                        moveAhead = 0;
                        concatChar = 0;
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
                                curr.state_id = TK_NUM;
                                curr.final = 1;
                                moveAhead = 0;
                                concatChar = 0;
                                break;
                            default:
                                curr.error = 3;
                        };
                        break;
                    case 1:
                        curr.state_id = 37;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 38:
                tmp = isdigit(next_char) ? 1 : 0;
                switch (tmp) {
                    case 0:
                        curr.error = 3;
                        break;
                    case 1:
                        curr.state_id = 39;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 39:
                tmp = isdigit(next_char) ? 1 : 0;
                switch (tmp) {
                    case 0:
                        curr.error = 3;
                        break;
                    case 1:
                        curr.state_id = TK_RNUM;
                        curr.final = 1;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 50:
                tmp = isSmallAlpha(next_char);
                switch (tmp) {
                    case 0:
                        curr.state_id = TK_FIELDID;
                        curr.final = 1;
                        moveAhead = 0;
                        concatChar = 0;
                        break;
                    case 1:
                        curr.state_id = 50;
                        break;
                    default:
                        curr.error = 3;
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
                            curr.error = 3;
                        }
                        break;
                    case 1:
                        curr.state_id = 47;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 47:
                tmp = isBtoD(next_char);
                switch (tmp) {
                    case 0:
                        tmp2 = is2to7(next_char);
                        if (tmp2) {
                            curr.state_id = 48;
                        } else {
                            curr.state_id = TK_ID;
                            curr.final = 1;
                            moveAhead = 0;
                            concatChar = 0;
                        }
                        break;
                    case 1:
                        curr.state_id = 47;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            case 48:
                tmp = is2to7(next_char);
                switch (tmp) {
                    case 0:
                        curr.state_id = TK_ID;
                        curr.final = 1;
                        moveAhead = 0;
                        concatChar = 0;
                        break;
                    case 1:
                        curr.state_id = 47;
                        break;
                    default:
                        curr.error = 3;
                };
                break;

            default:
                curr.error = 3;
        }

        if (concatChar)
            lexeme[cur_len++] = next_char;

        if (moveAhead)
            moveForward(buf);

        // reset the flags
        moveAhead = 1;
        concatChar = 1;

        if (next_char == '$') {
            if (curr.state_id == 17
                || curr.state_id == 20
                || curr.state_id == 30
                || curr.state_id == 35
                || curr.state_id == 37
                || curr.state_id == 43
                || curr.state_id == 44
                || curr.state_id == 47
                || curr.state_id == 48
                || curr.state_id == 51
                || curr.state_id == 0
            ) {
                curr.final = 1;
                break;
            } else {
                printf("curr %d\n", curr.state_id);
                curr.error = 100;
                return curr;
            }
        }
    }


    // check for keywords
    int new_sid;
    if ((curr.state_id == 45 || curr.state_id == 51)) {

        int k = checkIfKeyword(t, lexeme, cur_len);
        if (k != -1) {
            curr.state_id = k;
            curr.error = -1;
            curr.final = 1;
        }
    }

    strcpy(curr.lexeme, lexeme);
    curr.line_no = *line;

    if ((curr.state_id == 49
        && cur_len > MAX_ID_LEN)
        || (curr.state_id == 45
        && cur_len > MAX_FUNID_LEN)
    ) {
        curr.error = 1;
    }

    return curr;

}


void lexicalAnalysis(FILE *fp, int k) {

    FileBuffer b;
    fp = getStream(fp, &b, k);

    int line = 1;
    tokenInfo a; a.error = -1;
    char lexeme[101];
    int i = 0;
    char *start;
    // read tokens and print them
    while (1) {
        start = b.current;
        a = getNextToken(&b, &line, lexeme);

        if (a.error != -1 && a.error != 100) {
            reportError(stderr, a.error, a);
            memset(lexeme, '\0', 100);
            continue;
        } else if (a.error == 100) {
            break;
        }

        printf("LINE %d --> `%s` : %s\n", a.line_no, a.lexeme, final_states[a.state_id]);
        memset(lexeme, '\0', 100);
    }

}
