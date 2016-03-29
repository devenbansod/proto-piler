/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#include "common.h"
#include "fileBufferDef.h"

void reloadBuffer(FileBuffer *b, int buf_num) {
    if (buf_num == 1) {
        int len = fread(b->buf1, 1, b->size_of_buffer - 1, b->fp);
        if (len == 0) {
            b->buf1[0] ='\0';
        } else if (len == b->size_of_buffer - 1) {
            b->buf1[++len] ='\0';
        } else {
            b->buf1[len] = EOF;
        }
    } else {
        int len = fread(b->buf2, 1, b->size_of_buffer - 1, b->fp);
        if (len == 0) {
            b->buf2[0] ='\0';
        } else if (len == b->size_of_buffer - 1) {
            b->buf2[++len] ='\0';
        } else {
            b->buf2[len] = EOF;
        }
    }
}

void moveForward(FileBuffer *b) {
    if (b->current == (b->buf1) + b->size_of_buffer - 1) {
        reloadBuffer(b, 2);
        b->current = b->buf2;
        b->curBuf = 2;
    }
    else if (b->current == (b->buf2) + b->size_of_buffer - 1) {
        reloadBuffer(b, 1);
        b->current = b->buf1;
        b->curBuf = 1;
    } else {
        b->current += 1;
    }
}


char readChar(FileBuffer *b) {
    char ret_char = *(b->current);

    // if reached end of current buffer
    // move to appropriate new buffer
    if (ret_char == '\0') {
        moveForward(b);
        ret_char = *(b->current);
    }

    return ret_char;
}


int checkEndOfFile(FileBuffer *b) {
    return
    (
        (b->curBuf == 1 && b->current != ((b->buf1) + b->size_of_buffer))
        || (b->curBuf == 2 && b->current != ((b->buf2) + b->size_of_buffer))
    );
}