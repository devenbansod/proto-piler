#include "common.h"
#include "keywordTrie.h"
#include "lexerDef.h"
#include "fileBuffer.h"


#define MAX_FUNID_LEN 30
#define MAX_ID_LEN 20

#define STR_CPY_STATE_NAME(X) strcpy(final_states[X], #X)

extern Trie *t;
char** final_states;

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
        || next_char == '_' || next_char == '$'
        || next_char == '\n' || next_char == ' '
        || next_char == '\t' || next_char == ','
        || next_char == '#'
    ) {
        return 1;
    } else {
        return -1;
    }

}

int checkIfInLanguage(char next_char) {
    if (isalpha(next_char) || isdigit(next_char) ? 1 : 0
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
        || next_char == '\t' || next_char == '\r'
        || next_char == '_' || next_char == '#'
        || next_char == ','
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
    switch (error_code) {
        case -1 :
        case 100:
        case 50 :
            return;

        case 1  :
            fprintf(stderr, "*** ERROR 1: Identifier at line : <%d> "
                "is longer than maximum prescribed length of 20 characters\n",
                err_tok.line_no
            );
            break;
        case 2  :
            break;
        case 3  :
            fprintf(stderr, "*** ERROR 3: Unknown pattern <%s> at line <%d>\n",
                err_tok.lexeme, err_tok.line_no
            );
            break;
        case 4  :
            break;

        default:
            printf("Some error!\n");
            return;
    }
}


Symbol getTermType(int state_id) {
    switch (state_id) {
        case 1 :
            return TK_SQL;
            break;
        case 2 :
            return TK_SQR;
            break;
        case 3 :
            return TK_SEM;
            break;
        case 4 :
            return TK_COLON;
            break;
        case 5 :
            return TK_DOT;
            break;
        case 6 :
            return TK_OP;
            break;
        case 7 :
            return TK_CL;
            break;
        case 8 :
            return TK_PLUS;
            break;
        case 9 :
            return TK_MINUS;
            break;
        case 10 :
            return TK_MUL;
            break;
        case 11 :
            return TK_DIV;
            break;
        case 12 :
            return TK_NOT;
            break;
        case 14 :
            return TK_NE;
            break;
        case 16 :
            return TK_EQ;
            break;
        case 18 :
            return TK_GE;
            break;
        case 19 :
            return TK_GT;
            break;
        case 21 :
            return TK_LE;
            break;
        case 24 :
            return TK_ASSIGNOP;
            break;
        case 25 :
            return TK_LT;
            break;
        case 28 :
            return TK_AND;
            break;
        case 31 :
            return TK_RECORDID;
            break;
        case 34 :
            return TK_OR;
            break;
        case 36 :
            return TK_COMMENT;
            break;
        case 40 :
            return TK_RNUM;
            break;
        case 41 :
            return TK_NUM;
            break;
        case 45 :
            return TK_FUNID;
            break;
        case 49 :
            return TK_ID;
            break;
        case 51 :
            return TK_FIELDID;
            break;
        case 52 :
            return TK_COMMA;
            break;
        case 61 :
            return TK_WITH;
            break;
        case 62 :
            return TK_PARAMETERS;
            break;
        case 63 :
            return TK_END;
            break;
        case 64 :
            return TK_WHILE;
            break;
        case 65 :
            return TK_INT;
            break;
        case 66 :
            return TK_REAL;
            break;
        case 67 :
            return TK_TYPE;
            break;
        case 68 :
            return TK_MAIN;
            break;
        case 69 :
            return TK_GLOBAL;
            break;
        case 70 :
            return TK_PARAMETER;
            break;
        case 71 :
            return TK_LIST;
            break;
        case 72 :
            return TK_INPUT;
            break;
        case 73 :
            return TK_OUTPUT;
            break;
        case 74 :
            return TK_ENDWHILE;
            break;
        case 75 :
            return TK_IF;
            break;
        case 76 :
            return TK_THEN;
            break;
        case 77 :
            return TK_ENDIF;
            break;
        case 78 :
            return TK_READ;
            break;
        case 79 :
            return TK_WRITE;
            break;
        case 80 :
            return TK_RETURN;
            break;
        case 81 :
            return TK_RECORD;
            break;
        case 82 :
            return TK_ENDRECORD;
            break;
        case 83 :
            return TK_ELSE;
            break;
        case 84 :
            return TK_CALL;
            break;
        default :
            return -1;
    }

    return -1;
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
    DFA_state curr; curr.state_id = 0; curr.final = 0; curr.error = -1;
    int tmp, tmp2, cur_len = 0;

    int concatChar = 1, moveAhead = 1;
    // loop till it is a final DFA_state
    // or a lexical error has occured

    tokenInfo ret_tok;

    while (curr.final == 0 && curr.error == -1 && curr.state_id != ERROR) {
        char next_char = readChar(buf);

        if (next_char == '\0') {
            if (checkEndOfFile(buf))
                curr.error = 100;
            continue;
        }

        if (checkIfInLanguage(next_char) == -1) {
            fprintf(stderr, "*** ERROR 2: Unrecognised symbol <%c> at line <%d>\n", next_char, *line);
        }


        switch (curr.state_id) {
            // START DFA_state
            case 0:
                switch (next_char) {
                    case '\r':
                    case '\t':
                    case ' ' :
                        concatChar = 0;
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
                        moveAhead = 1;
                        concatChar = 0;
                        break;
                    case EOF:
                        curr.state_id = 100;
                        curr.final = 1;
                        concatChar = 0;
                        break;

                    // If some stray symbol when in START state,
                    // It is UNKNOWN SYMBOL
                    default:
                        tmp = isdigit(next_char) ? 1 : 0;
                        switch (tmp) {
                            case 0:
                                tmp2 = isBtoD(next_char);
                                tmp = checkIfInLanguage(next_char);
                                tmp2 = (tmp == -1) ? -1 : tmp;
                                switch (tmp2) {
                                    case 0:
                                        curr.state_id = 50;
                                        break;
                                    case 1:
                                        curr.state_id = 46;
                                        break;
                                    default:
                                        curr.error = 2;
                                };
                                break;
                            case 1:
                                curr.state_id = 37;
                                break;
                            default:
                                curr.error = 2;
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
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            case 15:
                switch (next_char) {
                    case '=' :
                        curr.state_id = 16;
                        curr.final = 1;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
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
                            moveAhead = 0;
                            concatChar = 0;
                        } else if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
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
                                        curr.state_id = 24;
                                        curr.final = 1;
                                        break;
                                    default:
                                        if (checkIfInLanguage(next_char) == -1) {
                                            curr.error = 2;
                                        } else {
                                            curr.error = 3;
                                            concatChar = 0;
                                        }
                                }
                                break;
                            default:
                                if (checkIfInLanguage(next_char) == -1) {
                                    curr.error = 2;
                                } else {
                                    curr.error = 3;
                                    concatChar = 0;
                                }
                        }
                        break;

                    default:
                        if (isAllExcept('-', next_char) && isAllExcept('=', next_char)) {
                            curr.state_id = 25;
                            curr.final = 1;

                            moveAhead = 0;
                            concatChar = 0;
                        } else if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
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
                                curr.state_id = 28;
                                curr.final = 1;
                                break;
                            default:
                                concatChar = 0;
                        }
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            case 29:
                tmp = isSmallAlpha(next_char);
                switch (tmp) {
                    case 1:
                        curr.state_id = 30;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
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
                                curr.state_id = 34;
                                curr.final = 1;
                                break;
                            default :
                                curr.error = 3;
                                concatChar = 0;
                        }
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            case 35:
                switch (next_char) {
                    case '\n' :
                    case '\r' :
                        curr.state_id = 36;
                        curr.final = 1;

                        moveAhead = 0;
                        concatChar = 0;

                        break;
                    // default:
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

                        moveAhead = 0;
                        concatChar = 0;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }

                };
                break;

            case 42:
                tmp = isalpha(next_char) > 0 ? 1 : 0;
                switch (tmp) {
                    case 0:
                        curr.error = 3;
                        concatChar = 0;
                        break;
                    case 1:
                        curr.state_id = 43;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            case 43:
                tmp = isalpha(next_char) > 0 ? 1 : 0;
                switch (tmp) {
                    case 0:
                        tmp2 = isdigit(next_char) ? 1 : 0;
                        switch(tmp2) {
                            case 0:
                                curr.state_id = 45;
                                curr.final = 1;

                                moveAhead = 0;
                                concatChar = 0;
                                break;
                            case 1:
                                curr.state_id = 44;
                                break;
                            default:
                            if (checkIfInLanguage(next_char) == -1) {
                                curr.error = 2;
                            } else {
                                curr.error = 3;
                                concatChar = 0;
                            }
                        }
                        break;
                    case 1:
                        curr.state_id = 43;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            case 44:
                tmp = isdigit(next_char) ? 1 : 0;
                switch(tmp) {
                    case 0:
                        curr.state_id = 45;
                        curr.final = 1;

                        moveAhead = 0;
                        concatChar = 0;
                        break;
                    case 1:
                        curr.state_id = 44;
                        break;
                    default:
                        curr.state_id = 45;

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
                                curr.state_id = 41;
                                curr.final = 1;

                                moveAhead = 0;
                                concatChar = 0;
                                break;
                            default:
                                if (checkIfInLanguage(next_char) == -1) {
                                    curr.error = 2;
                                } else {
                                    curr.error = 3;
                                    concatChar = 0;
                                }
                        };
                        break;
                    case 1:
                        curr.state_id = 37;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            case 38:
                tmp = isdigit(next_char) ? 1 : 0;
                switch (tmp) {
                    case 0:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                        break;
                    case 1:
                        curr.state_id = 39;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            case 39:
                tmp = isdigit(next_char) ? 1 : 0;
                switch (tmp) {
                    case 0:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                        break;
                    case 1:
                        curr.state_id = 40;
                        curr.final = 1;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            case 50:
                tmp = isSmallAlpha(next_char);
                switch (tmp) {
                    case 0:
                        curr.state_id = 51;
                        curr.final = 1;

                        moveAhead = 0;
                        concatChar = 0;
                        break;
                    case 1:
                        curr.state_id = 50;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
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
                            if (checkIfInLanguage(next_char) == -1) {
                                curr.error = 2;
                            } else {
                                curr.error = 3;
                                concatChar = 0;
                            }
                        }
                        break;
                    case 1:
                        curr.state_id = 47;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
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
                            curr.state_id = 49;
                            curr.final = 1;

                            moveAhead = 0;
                            concatChar = 0;
                        }
                        break;
                    case 1:
                        curr.state_id = 47;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            case 48:
                tmp = is2to7(next_char);
                switch (tmp) {
                    case 0:
                        curr.state_id = 49;
                        curr.final = 1;

                        moveAhead = 0;
                        concatChar = 0;
                        break;
                    case 1:
                        curr.state_id = 47;
                        break;
                    default:
                        if (checkIfInLanguage(next_char) == -1) {
                            curr.error = 2;
                        } else {
                            curr.error = 3;
                            concatChar = 0;
                        }
                };
                break;

            default:
                if (checkIfInLanguage(next_char) == -1) {
                    curr.error = 2;
                } else {
                    curr.error = 3;
                    concatChar = 0;
                }
        }

        if (curr.error <= 0 && concatChar)
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

                strcpy(ret_tok.lexeme, lexeme);
                ret_tok.line_no = *line;
                ret_tok.error = curr.error;

                return ret_tok;
            }
        }
    }


    // check for keywords
    if ((curr.state_id == 45 || curr.state_id == 51)) {

        int k = checkIfKeyword(t, lexeme, cur_len);
        if (k != -1) {
            curr.state_id = k;
            curr.error = -1;
            curr.final = 1;
        }
    }

    if ((curr.state_id == 31
        && cur_len > MAX_ID_LEN)
        || (curr.state_id == 45
        && cur_len > MAX_FUNID_LEN)
        || (curr.state_id == 49
        && cur_len > MAX_ID_LEN)
        || (curr.state_id == 51
        && cur_len > MAX_ID_LEN)
    ) {
        int i;
        for(i = 20; i < cur_len; i++) {
            lexeme[i] = '\0';
        }
        curr.error = 1;
    }

    memset(ret_tok.lexeme, '\0', 100);
    strcpy(ret_tok.lexeme, lexeme);
    ret_tok.line_no = *line;
    ret_tok.error = curr.error;

    if (curr.final && ((curr.error < 0) || (curr.error == 1))) {
        ret_tok.symbol_type = getTermType(curr.state_id);
    } else {
        ret_tok.symbol_type = ERROR;
    }

    if (curr.error == 100) {
        ret_tok.symbol_type = EOI;
    }

    return ret_tok;

}


void lexicalAnalysis(FILE *fp, int k) {

    initStateNames(final_states);

    FileBuffer b;
    fp = getStream(fp, &b, k);

    int line = 1;
    tokenInfo a; a.error = -1;
    char lexeme[101];

    // read tokens and print them
    while (1) {
        a = getNextToken(&b, &line, lexeme);

        if (a.error == 1) {
            reportError(stderr, a.error, a);
            fprintf(stderr, "I cut short your Identifier on line %d\n", a.line_no);
            memset(lexeme, '\0', 100);
        } else if (a.symbol_type == ERROR) {
            reportError(stderr, a.error, a);
            memset(lexeme, '\0', 100);
            continue;
        } else if (a.symbol_type == EOI) {
            break;
        } else if (a.symbol_type == TK_COMMENT) {
            memset(lexeme, '\0', 100);
            continue;
        }

        int i = 0;
        char line_no_str[10];
        sprintf(line_no_str, "%d", a.line_no);

        printf("LINE <%d> ", a.line_no);
        for (i = strlen(line_no_str); i < 5; i++) {
            printf(" ");
        }

        printf("<%s>", final_states[a.symbol_type]);

        for (i = strlen(final_states[a.symbol_type]); i < 15; i++) {
            printf(" ");
        }

        printf(" : %s\n", a.lexeme);
        memset(lexeme, '\0', 100);
    }

    free(final_states);

}


void printCommentFreeSource(char *filename) {
    FILE *fp = fopen(filename, "r");

    char *line = NULL;
    size_t line_len = 0;
    ssize_t read_len;

    while ((read_len = getline(&line, &line_len, fp)) != -1) {
        int i = 0;
        while (i < read_len
            && (line[i] == ' '
            || line[i] == '\n'
            || line[i] == '\t'
            || line[i] == '\r'
            )
        ) {
            i++;
        }

        if (i < read_len && line[i] != '%') {
            printf("%s", line);
        }
    }

    free(line);
    fclose(fp);
}
