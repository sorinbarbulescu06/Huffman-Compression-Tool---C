#include "dec.h"

//compress.c
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