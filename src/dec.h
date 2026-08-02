#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NO_CHAR 257
#define MAX_PATH_LEN 200

//data type of the file structure
typedef struct fil{
    int file_len;
    char *name;
}d_file, *t_file;

//data type of the letter array
typedef struct lit{
    int count;
    int ch;
}d_lit, *t_lit;

//data type of the tree node
typedef struct tree{
    int sum;
    int ch;
    struct tree *lt;
    struct tree *rt;
}d_tree_node, *t_tree_node;

//data type of the parent and letter list node
typedef struct list{
    t_tree_node node;
    struct list *next;
    struct list *prev;
}d_node, *t_node;

//functions.c
void free_tree(t_tree_node root);
int update_freq(FILE *fin, int *freq);
t_lit make_sorted_array(int *freq);
int make_list(t_node head, t_lit arr);
t_tree_node make_heap(t_node head_parent, t_node head_letter);
void setup_hashmap(char *hashmap[], t_tree_node root, int number, int depth, int *ok);
void write_compressed(FILE *fin, FILE *fout, char *hashmap[], unsigned char *buf, int *buf_count);
void init_head(t_node head);
int string_found(int argc, char *argv[], char *s);
void print_or_not(FILE *fin, FILE *fout, unsigned char *buf, int *buf_count, t_tree_node root, int *ok, t_tree_node original_root);