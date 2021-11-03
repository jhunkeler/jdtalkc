#ifndef JDTALKC_JDTALK_H
#define JDTALKC_JDTALK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#define DICT_INITIAL_SIZE 65535
#define DICT_WORD_SIZE_MAX 255
#define INPUT_SIZE_MAX 255
#define OUTPUT_PART_MAX 255
#define OUTPUT_SIZE_MAX 1024

#define WT_ICASE 0x80
#define WT_ANY 0
#define WT_NOUN 1
#define WT_ADJECTIVE 2
#define WT_ADVERB 3
#define WT_VERB 4

struct Word {
    char *word;
    unsigned type;
    size_t nchar;
};

struct Dictionary {
    struct Word **words;
    size_t nelem_alloc;
    size_t nelem_inuse;
};

struct Dictionary *dictionary_new();
void dictionary_append(struct Dictionary **dict, char *s, unsigned type);
int dictionary_read(FILE *fp, struct Dictionary **dict, unsigned type);
struct Dictionary *dictionary_populate();
int dictionary_contains(struct Dictionary *dict, const char *s, unsigned type);
char *dictionary_word(struct Dictionary *dict, unsigned type);
void dictionary_free(struct Dictionary *dict);

char *str_random_case(char *s);
char *str_hill_case(char *s);
char *str_leet(char *s);
char *str_title_case(char *s);

char *talkf(struct Dictionary *dict, const char *fmt, char **parts);
char *talk_salad(struct Dictionary *dict, size_t limit, char **parts);
char *talk_acronym(struct Dictionary *dict, const char *fmt, char *s, char **parts);
int acronym_safe(const char *acronym, const char *pattern);

#endif //JDTALKC_JDTALK_H
