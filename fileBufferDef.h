#ifndef FILE_BUFFER_DEF
#define FILE_BUFFER_DEF
#define MAX_BUFFER 4097

struct fileBuffer {
    char *buf1;
    char *buf2;
    int curBuf;
    char *current;
    FILE *fp;
    int size_of_buffer;
};

typedef struct fileBuffer FileBuffer;

#endif