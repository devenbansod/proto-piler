#include "common.h"
#include "fileBuffer.h"

void reloadBuffer(FileBuffer *b, int buf_num) {
	if (buf_num == 1) {
		int len = read(b->fd, b->buf1, MAX_BUFFER - 1);
		if (len == 0) {
			b->buf1[0] ='$';
		} else if (len == MAX_BUFFER - 1) {
			b->buf1[++len] ='\0';
		} else {
			b->buf1[len] = '$';
		}
	} else {
		int len = read(b->fd, b->buf2, MAX_BUFFER - 1);
		if (len == 0) {
			b->buf2[0] ='$';
		} else if (len == MAX_BUFFER - 1) {
			b->buf2[++len] ='\0';
		} else {
			b->buf2[len] = '$';
		}
	}
}

void initFileBuffer(FileBuffer *b, char *filename) {
	b->fd = open(filename, 0400);
	reloadBuffer(b, 1);
	b->curBuf = 1;
	b->current = b->buf1;
}

void moveForward(FileBuffer *b) {
	if (b->current == (b->buf1) + MAX_BUFFER - 1) {
		reloadBuffer(b, 2);
		b->current = b->buf2;
		b->curBuf = 2;
	}
	else if (b->current == (b->buf2) + MAX_BUFFER - 1) {
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
	}

	return ret_char;
}