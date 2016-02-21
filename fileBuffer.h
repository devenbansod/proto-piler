#ifndef FILE_BUFFER_INTERFACE
#define FILE_BUFFER_INTERFACE

/*
 * Interface to FileBuffer
 *
 */
void initFileBuffer(FileBuffer *b, char *pathname);
void moveForward(FileBuffer *b);
char readChar(FileBuffer *b);

#endif