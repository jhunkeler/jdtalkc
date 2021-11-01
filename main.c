#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#define DICT_INITIAL_SIZE 1024
#define DICT_WORD_SIZE_MAX 255
#define INPUT_SIZE_MAX DICT_WORD_SIZE_MAX
#define OUTPUT_PART_MAX 255
#define OUTPUT_SIZE_MAX 1024

#define WT_ICASE 0x80
#define WT_ANY 0
#define WT_NOUN 1
#define WT_ADJECTIVE 2
#define WT_ADVERB 3
#define WT_VERB 4


int seed;

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

struct Dictionary *dictionary_new() {
    struct Dictionary *dict;
    dict = calloc(1, sizeof(*dict));
    dict->words = calloc(DICT_INITIAL_SIZE, sizeof(**dict->words));
    dict->nelem_alloc = DICT_INITIAL_SIZE;
    dict->nelem_inuse = 0;
    return dict;
}

void dictionary_append(struct Dictionary **dict, char *s, unsigned type) {
    if ((*dict)->nelem_inuse + 1 > (*dict)->nelem_alloc) {
        struct Word **tmp;
        (*dict)->nelem_alloc++;
        tmp = realloc((*dict)->words, (*dict)->nelem_alloc * sizeof(**(*dict)->words));
        if (!tmp) {
            perror("Unable to extend word list");
            exit(1);
        }
        (*dict)->words = tmp;
    }
    (*dict)->words[(*dict)->nelem_inuse] = calloc(1, sizeof(*(*dict)->words[0]));
    (*dict)->words[(*dict)->nelem_inuse]->word = strdup(s);
    (*dict)->words[(*dict)->nelem_inuse]->nchar = strlen(s) - 1;
    *((*dict)->words[(*dict)->nelem_inuse]->word + ((*dict)->words[(*dict)->nelem_inuse]->nchar)) = '\0';
    (*dict)->words[(*dict)->nelem_inuse]->type = type;
    (*dict)->nelem_inuse++;
}

int dictionary_read(FILE *fp, struct Dictionary **dict, unsigned type) {
    char buf[DICT_WORD_SIZE_MAX];
    char *bufp;
    bufp = buf;
    while ((fgets(bufp, sizeof(buf), fp) != NULL)) {
        if (errno) {
            return errno;
        }
        if (*bufp == '\0' || *bufp == '\n')
            continue;
        dictionary_append(&(*dict), bufp, type);
    }
    return 0;
}

struct Dictionary *dictionary_populate() {
    FILE *fp;
    struct Dictionary *dict;
    const char *files[] = {
            "nouns.txt",
            "adjectives.txt",
            "adverbs.txt",
            "verbs.txt",
            NULL,
    };

    const unsigned files_type[] = {
            WT_NOUN,
            WT_ADJECTIVE,
            WT_ADVERB,
            WT_VERB,
    };

    dict = dictionary_new();
    for (size_t i = 0; files[i] != NULL; i++) {
        char *datadir;
        char filename[PATH_MAX];
        filename[0] = '\0';
        datadir = getenv("JDTALK_DATA");

        if (!datadir) {
            fprintf(stderr, "JDTALK_DATA environment variable is not set\n");
            exit(1);
        }

        sprintf(filename, "%s/%s", datadir, files[i]);
        fp = fopen(filename, "r");
        if (!fp) {
            fprintf(stderr, "Unable to open dictionary: %s", filename);
            exit(1);
        }
        dictionary_read(fp, &dict, files_type[i]);
        fclose(fp);
    }
    return dict;
}

int dictionary_contains(struct Dictionary *dict, const char *s, unsigned type) {
    int result;
    unsigned icase;

    icase = type & 0x80;
    type &= 0x7f;
    result = 0;

    for (size_t i = 0; i < dict->nelem_inuse; i++) {
        if (*s != *dict->words[i]->word) {
            // Didn't start with first character in s
            continue;
        }

        if (type != WT_ANY && dict->words[i]->type != type) {
            // Incorrect type of word
            continue;
        }

        if (icase) {
            result = strcasecmp(dict->words[i]->word, s) == 0;
        } else {
            result = strcmp(dict->words[i]->word, s) == 0;
        }

        if (result) {
            break;
        }
    }
    return result;
}

char *dictionary_word(struct Dictionary *dict, unsigned type) {
    struct Word *word;
    while (1) {
        size_t index = rand() % dict->nelem_inuse;
        word = dict->words[index];
        if (word->type == type || type == WT_ANY) {
            return word->word;
        }
    }
}

void dictionary_free(struct Dictionary *dict) {
    for (size_t i = 0; i < dict->nelem_alloc; i++) {
        free(dict->words[i]);
    }
    free(dict->words);
}

char *str_random_case(char *s) {
    for (size_t i = 0; i < strlen(s); i++) {
        if ((rand() % 100) >= 50) {
            s[i] = (char)toupper(s[i]);
        }
    }
    return s;
}

char *str_hill_case(char *s) {
    for (size_t i = 0; i < strlen(s); i++) {
        if (i % 2) {
            s[i] = (char)toupper(s[i]);
        }
    }
    return s;
}

char *str_leet(char *s) {
    static char buf[OUTPUT_SIZE_MAX];
    memset(buf, '\0', sizeof(buf));
    for (size_t i = 0; i < strlen(s); i++) {
        switch (s[i]) {
            case 'a':
            case 'A':
                strcat(buf, "4");
                break;
            case 'b':
            case 'B':
                strcat(buf, "8");
                break;
            case 'c':
            case 'C':
                strcat(buf, "(");
                break;
            case 'd':
            case 'D':
                strcat(buf, ")");
                break;
            case 'e':
            case 'E':
                strcat(buf, "3");
                break;
            case 'f':
            case 'F':
                strcat(buf, "ƒ");
                break;
            case 'g':
            case 'G':
                strcat(buf, "6");
                break;
            case 'h':
            case 'H':
                strcat(buf, "#");
                break;
            case 'i':
            case 'I':
                strcat(buf, "!");
                break;
            case 'j':
            case 'J':
                strcat(buf, "]");
                break;
            case 'k':
            case 'K':
                strcat(buf, "X");
                break;
            case 'l':
            case 'L':
                strcat(buf, "1");
                break;
            case 'm':
            case 'M':
                strcat(buf, "|\\/|");
                break;
            case 'n':
            case 'N':
                strcat(buf, "|\\|");
                break;
            case 'o':
            case 'O':
                strcat(buf, "0");
                break;
            case 'p':
            case 'P':
                strcat(buf, "|*");
                break;
            case 'q':
            case 'Q':
                strcat(buf, "9");
                break;
            case 'r':
            case 'R':
                strcat(buf, "|2");
                break;
            case 's':
            case 'S':
                strcat(buf, "$");
                break;
            case 't':
            case 'T':
                strcat(buf, "7");
                break;
            case 'u':
            case 'U':
                strcat(buf, "|_|");
                break;
            case 'v':
            case 'V':
                strcat(buf, "\\/");
                break;
            case 'w':
            case 'W':
                strcat(buf, "\\/\\/");
                break;
            case 'x':
            case 'X':
                strcat(buf, "><");
                break;
            case 'y':
            case 'Y':
                strcat(buf, "¥");
                break;
            case 'z':
            case 'Z':
                strcat(buf, "2");
                break;
            default:
                buf[strlen(buf)] = s[i];
                break;
        }
    }
    return buf;
}

char *talkf(struct Dictionary *dict, const char *fmt, char **parts) {
    static char buf[OUTPUT_SIZE_MAX];
    buf[0] = '\0';

    if (!fmt || !strlen(fmt)) {
        return NULL;
    }

    for (size_t i = 0; i < strlen(fmt); i++) {
        char *word = NULL;
        switch (fmt[i]) {
            case 'a':
                word = dictionary_word(dict, WT_ADJECTIVE);
                break;
            case 'd':
                word = dictionary_word(dict, WT_ADVERB);
                break;
            case 'n':
                word = dictionary_word(dict, WT_NOUN);
                break;
            case 'v':
                word = dictionary_word(dict, WT_VERB);
                break;
            default:
                break;
        }
        parts[i] = word;
        strncat(buf, word, OUTPUT_SIZE_MAX);
        if (i < strlen(fmt) - 1)
            strcat(buf, " ");
    }
    return buf;
}

char *talk_salad(struct Dictionary *dict, int limit, char **parts) {
    static char buf[OUTPUT_SIZE_MAX];
    buf[0] = '\0';
    for (size_t i = 0; i < limit; i++) {
        char *word = NULL;
        word = dictionary_word(dict, WT_ANY);
        parts[i] = word;
        strncat(buf, word, OUTPUT_SIZE_MAX);
        if (i < limit - 1) {
            strcat(buf, " ");
        }
    }
    return buf;
}

char *talk_acronym(struct Dictionary *dict, char *s) {
    for (size_t i = 0; i < strlen(s); i++) {

    }
}

int main(int argc, char *argv[]) {
    FILE *fp;
    struct Dictionary *dict;
    char buf[OUTPUT_SIZE_MAX];
    char format[INPUT_SIZE_MAX];
    char pattern[INPUT_SIZE_MAX];
    char *part[OUTPUT_PART_MAX];
    int found;
    int do_pattern;
    int do_exact;
    int do_random_case;
    int do_hill_case;
    int do_leet;
    int do_salad;
    int do_benchmark;
    int salad_limit;
    int limit;
    float start_time;
    float end_time;
    float time_elapsed;

    srand(time(NULL));
    do_pattern = 0;
    do_exact = 0;
    do_random_case = 0;
    do_hill_case = 0;
    do_leet = 0;
    do_salad = 0;
    do_benchmark = 0;
    limit = 1;
    salad_limit = 10;
    strcpy(format, "andv");

    for (size_t i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            do_benchmark = 1;
        }
        if (strcmp(argv[i], "-c") == 0) {
            limit = (int) strtol(argv[i + 1], NULL, 10);
        }
        if (strcmp(argv[i], "-p") == 0) {
            do_pattern = 1;
            memset(pattern, '\0', sizeof(pattern));
            strcpy(pattern, argv[i + 1]);
        }
        if (strcmp(argv[i], "-e") == 0) {
            do_exact = 1;
        }
        if (strcmp(argv[i], "-r") == 0) {
            do_random_case = 1;
        }
        if (strcmp(argv[i], "-H") == 0) {
            do_hill_case = 1;
        }
        if (strcmp(argv[i], "-l") == 0) {
            do_leet = 1;
        }
        if (strcmp(argv[i], "-s") == 0) {
            do_salad = 1;
            salad_limit = (int) strtol(argv[i + 1], NULL, 10);
        }
        if (strcmp(argv[i], "-f") == 0) {
            strcpy(format, argv[i + 1]);
        }
    }

    dict = dictionary_populate();

    if (do_pattern && !dictionary_contains(dict, pattern, WT_ANY)) {
        fprintf(stderr, "Word not found in dictionary: %s\n", pattern);
        exit(1);
    }

    if (do_benchmark)
        start_time = (float)clock()/CLOCKS_PER_SEC;

    for (size_t i = 0; i < limit; i++) {
        LABEL_TRY_AGAIN:
        memset(part, 0, sizeof(part) / sizeof(*part));
        if (do_salad) {
            strcpy(buf, talk_salad(dict, salad_limit, part));
        } else {
            strcpy(buf, talkf(dict, format, part));
        }

        if (do_pattern) {
            found = 0;
            for (size_t z = 0; part[z] != NULL; z++) {
                if (!do_exact) {
                    if (strstr(buf, pattern)) {
                        found = 1;
                        break;
                    }
                } else {
                    if (strcmp(part[z], pattern) == 0) {
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                goto LABEL_TRY_AGAIN;
            }
        }

        if (do_random_case) {
            str_random_case(buf);
        } else if (do_hill_case) {
            str_hill_case(buf);
        } else if (do_leet) {
            strcpy(buf, str_leet(buf));
        }
        puts(buf);
    }

    if (do_benchmark) {
        end_time = (float)clock()/CLOCKS_PER_SEC;
        time_elapsed = end_time - start_time;
        fprintf(stderr, "benchmark: %fs\n", time_elapsed);
    }

    dictionary_free(dict);
    return 0;
}
