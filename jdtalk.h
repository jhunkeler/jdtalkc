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

#define DEFAULT_FORMAT "andv"

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
unsigned dictionary_contains(struct Dictionary *dict[], const char *s, unsigned type);
char *dictionary_word(struct Dictionary *dict, unsigned type);
char *dictionary_word_formats(struct Dictionary *dict, const char *s);
struct Dictionary *dictionary_of(struct Dictionary **src, unsigned type);
void dictionary_free(struct Dictionary *dict);

char *str_random_case(char *s);
char *str_hill_case(char *s);
char *str_leet(char *s);
char *str_title_case(char *s);
char *str_randomize_words(char *s);
char *str_reverse(char *s);

char *talkf(struct Dictionary *dict[], char *fmt, char **parts, size_t parts_max);
char *talk_salad(struct Dictionary *dict[], size_t limit, char **parts, size_t parts_max);
char *talk_heart(struct Dictionary *dict[], size_t word_limit, size_t word_maxlen, char **parts, size_t parts_max);
char *talk_acronym(struct Dictionary *dict[], __attribute__((unused)) char *fmt, char *s, char **parts, size_t parts_max);
int acronym_safe(struct Dictionary *dict, const char *acronym, const char *pattern, const char *fmt);
int format_safe(char *s);

#endif //JDTALKC_JDTALK_H
