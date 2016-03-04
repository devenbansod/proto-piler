/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef FILE_BUFFER_INTERFACE
#define FILE_BUFFER_INTERFACE

#include "fileBufferDef.h"

/*
 * Interface to FileBuffer
 *
 */
void moveForward(FileBuffer *b);
char readChar(FileBuffer *b);
void reloadBuffer(FileBuffer *b, int buf_num);
int checkEndOfFile(FileBuffer *b);

#endif