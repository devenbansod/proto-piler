#ifndef FILE_BUFFER
#define FILE_BUFFER
#define MAX_BUFFER 4097

struct fileBuffer {
    char buf1[MAX_BUFFER];
    char buf2[MAX_BUFFER];
    int curBuf;
    char *current;
    int fd;
};

typedef struct fileBuffer FileBuffer;

/*
 * Interface to FileBuffer
 *
 */
void initFileBuffer(FileBuffer *b, char *pathname);
void moveForward(FileBuffer *b);
char readChar(FileBuffer *b);

#endif