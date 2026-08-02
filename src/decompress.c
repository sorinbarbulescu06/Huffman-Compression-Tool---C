#include "dec.h"

int main(int argc, char *argv[])
{
    char prefix[] = "to_decompress/";
    int file_count = 0;
    int i;
    //alocation zone
    char *path = malloc(MAX_PATH_LEN * sizeof(char));
    if (path == NULL) {
        goto error_path;
    }
    int *freq = malloc(NO_CHAR * sizeof(int));
    if (freq == NULL) {
        goto error_freq;
    }
    t_node head_parent = (t_node) malloc(sizeof(d_node));
    if (head_parent == NULL) {
        goto error_head_parent;
    }
    init_head(head_parent);
    t_node head_letter = (t_node) malloc(sizeof(d_node));
    if (head_letter == NULL) {
        goto error_head_letter;
    }
    init_head(head_letter);

    //case of inputing only ./decompress
    if (argc == 1) {
        printf("Not enough arguments. Leaving...\n");
        goto error_fin;
    }

    //opening the archive
    strcpy(path, prefix);
    strcat(path, argv[1]);
    FILE *fin = fopen(path, "rb");
    if (fin == NULL) {
        printf("Error at opening the file %s. Leaving...\n", argv[1]);
        goto error_fin;
    }

    char magic[6]; 
    magic[5] = '\0';
    fread(magic, sizeof(char), 5, fin);
    if (strcmp(magic, "SHCBS") != 0) {
        fprintf(stderr, "Error: the file %s is not an archive.Leaving...\n", argv[1]);
        fclose(fin);
        goto error_magic;
    }

    //building the Huffman tree
    fread(freq, sizeof(int), NO_CHAR, fin);

    t_lit arr = make_sorted_array(freq);
    if(arr == NULL) {
        goto error_array;
    }
    free(freq);
    freq = NULL;

    int aux = make_list(head_letter, arr);
    if (aux == -1) {
        goto error_letter_list;
    }

    t_tree_node root = make_heap(head_parent, head_letter);
    if (root == NULL) {
        goto error_make_heap;
    }

    fread(&file_count, sizeof(int), 1, fin);

    t_file files = (t_file) malloc(file_count * sizeof(d_file));
    if (files == NULL) {
        goto error_files;
    }
    for (i = 0; i < file_count; ++i) {
        char name_file[MAX_PATH_LEN];
        int cursor = 0;
        int ch;

        while (1) {
            ch = fgetc(fin);
    
            name_file[cursor] = (char)ch;
            cursor++;
    
            if (ch == '\0') {
                break;
            }
        }
        files[i].name = malloc((strlen(name_file) + 1) * sizeof(char));
        if(files[i].name == NULL) {
            goto error_file_names;
        }
        strcpy(files[i].name, name_file);
        fread(&ch, sizeof(int), 1, fin);
        files[i].file_len = ch;
    }
    int cnt = 0;
    unsigned char buf = 0;
    int buf_count = 0;
    while(cnt < file_count) {
        FILE *fout = NULL;
        int ok = 1;
        if (string_found(argc, argv, files[cnt].name)) {
            strcpy(path, "output/");
            strcat(path, files[cnt].name);
            fout = fopen(path, "wb");
            if (fout == NULL) {
                goto error_file_names;
            }
        }
        print_or_not(fin, fout, &buf, &buf_count, root, &ok, root);
        if (fout != NULL) {
            fclose(fout);
        }
        cnt++;
    }

    //free zone
    free_tree(root);
    for (i = 0; i < file_count; ++i) {
        free(files[i].name);
    }
    free(files);
    free(head_parent);
    free(head_letter);
    free(arr);
    free(path);
    fclose(fin);
    return 0;

    //err zone
error_file_names:
    for (int j = 0; j < i; ++j) {
        free(files[j].name);
    }
    free(files);

error_files:
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

error_array:
    free(arr);

error_magic:
    fclose(fin);

error_fin:
    free(head_letter);

error_head_letter:
    free(head_parent);

error_head_parent:
    if (freq != NULL) {
        free(freq);
    }

error_freq:
    free(path);

error_path:
    return -1;
}
