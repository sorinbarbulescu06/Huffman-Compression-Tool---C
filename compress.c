#include "dec.h"

 int main(int argc, char *argv[])
 {
    int i;
    int file_count = 0;
    char *prefix = "to_compress/";

    //checking if a file name was inntroduced in the command
    if (argc == 1) {
        printf("Not enough files introduced\n");
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

    //setting freq for every file and memorising the data from each one
    for (i = 1; i < argc; ++i) {
        char *file_name;
        char *last_slash = strrchr(argv[i], '/');
        if (last_slash != NULL) {
            file_name = last_slash + 1;
        } else {
            file_name = argv[i];
        }

        strcpy(path, prefix);
        strcat(path, file_name);

        FILE *fin = fopen(path, "rb");
        if (fin != NULL) {
            files[file_count].name = file_name;
            files[file_count].file_len = update_freq(fin, freq);
            file_count++;
            fclose(fin);
        } else {
            printf("File: %s was not found in folder to_compress/\n", file_name);
        }
    }
    if (file_count == 0) {
        printf("No files match the search for the compression. Leaving...\n");
        goto error_header_print;
    }
    //heading the freq vector in the output file
    FILE *fout = fopen("archive.bin", "wb");
    if (fout == NULL) {
        goto error_header_print;
    }
    fwrite(freq, sizeof(int), NO_CHAR, fout);

    t_lit arr = make_sorted_array(freq);
    if(arr == NULL) {
        goto error_array;
    }
    free(freq);
    freq = NULL;

    t_node head_parent = (t_node) malloc(sizeof(d_node));
    if (head_parent == NULL) {
        goto error_head_parent;
    }
    t_node head_letter = (t_node) malloc(sizeof(d_node));
    if (head_letter == NULL) {
        goto error_head_letter;
    }
    head_parent->next = head_parent;
    head_parent->prev = head_parent;
    head_letter->next = head_letter;
    head_letter->prev = head_letter;

    int aux = make_list(head_letter, arr);
    if (aux == -1){
        goto error_letter_list;
    }
    if (aux == 0) {
        printf("0 bytes of memory cannot be compressed. Leaving...\n");
        goto free_zone;
    }
    t_tree_node root = make_heap(head_parent, head_letter);
    if (root == NULL) {
        goto error_make_heap;
    }
    //setting up the hashmap
    char **hashmap = (char **)calloc(NO_CHAR, sizeof(char *));
    if (hashmap == NULL) {
        goto error_hashmap;
    }
    int ok = 1;
    setup_hashmap(hashmap, root, 0, 0, &ok);
    if (ok == 0) {
        goto error_strings_hashmap;
    }

    //free memory
    for (i = 0; i < NO_CHAR; ++i) {
        free(hashmap[i]);
    }
    free(hashmap);
    free_tree(root);
free_zone:
    free(head_letter);
    free(head_parent);
    free(arr);
    fclose(fout);
    free(path);
    free(files);
    return 0;

    //err zone
error_strings_hashmap:
    for (i = 0; i < NO_CHAR; ++i) {
        free(hashmap[i]);
    }
    free(hashmap);

error_hashmap:
    free_tree(root);

error_make_heap:
    t_node aux_p = head_parent->next;
    while(aux_p != head_parent) {
        t_node temp = aux_p;
        aux_p = temp->next;
        free_tree(temp->node);
        free(temp);
    }

error_letter_list:
    t_node aux_free = head_letter->next;
    while(aux_free != head_letter) {
        t_node aux_aux = aux_free;
        aux_free = aux_aux->next;
        free(aux_aux->node);
        free(aux_aux);
    }

    free(head_letter);

error_head_letter:
    free(head_parent);

error_head_parent:
    free(arr);

error_array:
    fclose(fout);

error_header_print:
    free(path);

error_path:
    free(files);

error_files:
    free(freq);

error_freq:
    return -1;
 }