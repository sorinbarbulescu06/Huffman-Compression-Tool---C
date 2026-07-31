#include "dec.h"

 int main(int argc, char *argv[])
 {
    int i;
    int file_count = 0;
    char prefix = "to_compress/";

    //checking if a file name was inntroduced in the command
    if (argc == 1) {
        printf("Not enough files introduced");
        return 1;    
    }

    //allocating and checking the freq vector
    int *freq = calloc(NO_CHAR, sizeof(int));
    if (freq == NULL) {
        goto error_freq;
    }

    //allocating enough space for every file
    t_file files = (t_file) malloc((argc - 1) * sizeof(d_file));
    if (files == NULL) {
        goto error_files;
    }

    //allocating the path vector
    char *path = malloc(MAX_PATH_LEN * sizeof(char));
    if (path == NULL) {
        goto error_path;
    }

    for (i = 1; i <= argc; ++i) {
        strcpy(path, prefix);
        strcat(path, argv[i]);
        FILE *fin = fopen(path, "rb");
        if (fin != NULL) {
            files[file_count].name = argv[i];
            update_freq(fin, freq);
            file_count++;
        }
    }


    //free memory
    free(path);
    free(files);
    free(freq);
    return 0;

    //err zone
    free(path);

error_path:
    free(files);

error_files:
    free(freq);

error_freq:
    return -1;
 }