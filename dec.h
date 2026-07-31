#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NO_CHAR 257
#define MAX_PATH_LEN 100

//data type of the file structure
typedef struct fil{
    int file_len;
    char *name;
}d_file, *t_file;
