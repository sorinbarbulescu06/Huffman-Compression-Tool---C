#include "dec.h"

//local
void put_in_file(FILE *fout, char *string, unsigned char *buf, int *buf_count)
{
    int cursor = 0;
    int full_len = strlen(string);
    while (cursor < full_len) {
        while(*buf_count < 8 && cursor < full_len) {
            *buf = *buf << 1;
            if (string[cursor] == '1') { //pushing 1 in buffer, else nothing
                *buf += 1;
            }
            *buf_count = *buf_count + 1;
            cursor++;
        }
        if (*buf_count == 8) {
            fwrite(buf, sizeof(unsigned char), 1, fout);
            *buf = 0;
            *buf_count = 0;
        }
    }
}

char* make_binary(int num, int depth) {
    int i;
    //edge case
    if (depth == 0) {
        depth = 1;
    }

    char *bin_str = (char *) malloc((depth + 1) * sizeof(char));
    if (bin_str == NULL) {
        return NULL;
    }

    //building the string from back to front
    bin_str[depth] = '\0';
    for (i = 1; i < depth + 1; ++i) {
        bin_str[depth - i] = (num & 1) + '0';
        num >>= 1;
    }

    return bin_str;
}

int comp(const void *a, const void *b)
{
    t_lit el1 = (t_lit)a;
    t_lit el2 = (t_lit)b;
    return(el2->count - el1->count);
}

int check_empty(t_node head) // 1 = empty, 0 = not empty
{
    if (head->next == head && head->prev == head)
        return 1;
    return 0;
}

t_tree_node peak(t_node head) // returns the sum of app
{
    return head->prev->node;
}

void push(t_node parent, t_node head)
{
    parent->next = head->next;
    parent->prev = head;
    head->next = parent;
    parent->next->prev = parent;
}

void pop(t_node head)
{
    t_node aux = head->prev;
    head->prev = aux->prev;
    aux->prev->next = head;
    free(aux);
}

//compress.c

void free_tree(t_tree_node root) {
    if (root == NULL) {
        return;
    }
    free_tree(root->lt);
    free_tree(root->rt);
    free(root);
}

int update_freq(FILE *fin, int *freq)
{
    int no_characters = 0;
    int ch = fgetc(fin);
    while(ch != EOF) {
        freq[ch]++;
        no_characters++;
        ch = fgetc(fin);
    }
    freq[NO_CHAR - 1]++;
    return no_characters;
}

//compress.c
t_lit make_sorted_array(int *freq)
{
    int i;
    //allocating the new array
    t_lit arr = (t_lit) malloc(NO_CHAR * sizeof(d_lit));
    if (arr == NULL) {
        return NULL;
    }

    for (i = 0; i < NO_CHAR; ++i) {
        arr[i].ch = i;
        arr[i].count = freq[i];
    }

    qsort(arr, NO_CHAR, sizeof(d_lit), comp);
    return arr;
}

int make_list(t_node head, t_lit arr) // -1 = aloc err, 0 = 0 letters, 1 = ok
{
    int cursor = NO_CHAR - 1;
    while (arr[cursor].count == 0 && cursor > 0) {
        cursor--;
    }
    if (cursor == 0)
        return 0;
    while(cursor >= 0) {
        t_tree_node leaf = (t_tree_node) malloc(sizeof(d_tree_node));
        if (leaf == NULL) {
            return -1;
        }
        leaf->ch = arr[cursor].ch;
        leaf->lt = NULL;
        leaf->rt = NULL;
        leaf->sum = arr[cursor].count;
        t_node t = (t_node) malloc(sizeof(d_node));
        if(t == NULL) {
            free(leaf);
            return -1;
        }
        t->node = leaf;
        push(t, head);
        cursor--;
    }
    return 1;
}

//compress.c
t_tree_node make_heap(t_node head_parent, t_node head_letter) // -1 = not enough chars to compress, null = error
{
    while (!(check_empty(head_letter) == 1 && head_parent->next->next == head_parent)) {
        t_tree_node rk, lk;
        
        //setting up rk and lk
        if(check_empty(head_parent) == 1) { //case of putting only leafs
            lk = peak(head_letter);
            pop(head_letter);
            rk = peak(head_letter);
            pop(head_letter);
        } else if (check_empty(head_letter) == 1) { //case of puttin only parents
            lk = peak(head_parent);
            pop(head_parent);
            rk = peak(head_parent);
            pop(head_parent);
        } else {
            t_tree_node aux_letter = peak(head_letter);
            t_tree_node aux_parent = peak(head_parent);
            if (aux_letter->sum > aux_parent->sum) { // least one is on parents
                lk = aux_parent;
                pop(head_parent);
            } else { // last one is on letters
                lk = aux_letter;
                pop(head_letter);
            }
            if (check_empty(head_parent) == 1) { //second leas one is surely on letters
                rk = peak(head_letter);
                pop(head_letter);
            } else if (check_empty(head_letter) == 1){ // second least one is surely on parents 
                rk = peak(head_parent);
                pop(head_parent);
            } else {
                aux_letter = peak(head_letter);
                aux_parent = peak(head_parent);
                if (aux_letter->sum > aux_parent->sum) { //least one is on parents
                    rk = aux_parent;
                    pop(head_parent);
                } else { //last one is on letters
                    rk = aux_letter;
                    pop(head_letter);
                }
            }
        }
        t_tree_node parent = (t_tree_node) malloc(sizeof(d_tree_node));
        if(parent == NULL) {
            free_tree(lk);
            free_tree(rk);
            return NULL;
        }

        parent->ch = 0;
        parent->lt = lk;
        parent->rt = rk;
        parent->sum = lk->sum + rk->sum;

        t_node new = (t_node) malloc(sizeof(d_node));
        if (new == NULL) {
            free_tree(parent);
            return NULL;
        }
        new->node = parent;
        push(new, head_parent);
    }
    t_tree_node final = head_parent->next->node;
    pop(head_parent);
    return final;
}

void setup_hashmap(char *hashmap[], t_tree_node root, int number, int depth, int *ok) // 0 success, 0 aloc err
{
    if (*ok == 1) {
        if (root->lt == NULL && root->rt == NULL) {
            hashmap[root->ch] = make_binary(number, depth);
            if (hashmap[root->ch] == NULL) {
                *ok = 0;
            }
        } else {
            if (root->lt != NULL && *ok == 1) {
                setup_hashmap(hashmap, root->lt, (number << 1) + 1, depth + 1, ok);
            }
            if (root->rt != NULL && *ok == 1) {
                setup_hashmap(hashmap, root->rt, number << 1, depth + 1, ok);
            }
        }
    }
}

void write_compressed(FILE *fin, FILE *fout, char *hashmap[], unsigned char *buf, int *buf_count)
{
    int ch = fgetc(fin);
    while (ch != EOF) {
        put_in_file(fout, hashmap[ch], buf, buf_count);
        ch = fgetc(fin);
    }
    put_in_file(fout, hashmap[NO_CHAR - 1], buf, buf_count);
}
