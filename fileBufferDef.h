#ifndef FILE_BUFFER_DEF
#define FILE_BUFFER_DEF
#define MAX_BUFFER 4097

struct fileBuffer {
    char buf1[MAX_BUFFER];
    char buf2[MAX_BUFFER];
    int curBuf;
    char *current;
    int fd;
};

typedef struct fileBuffer FileBuffer;

#endif