#include "jdtalk.h"

struct Dictionary *dictionary_new() {
    struct Dictionary *dict;
    dict = malloc(1 * sizeof(*dict));
    dict->words = malloc(DICT_INITIAL_SIZE * sizeof(**dict->words));
    dict->nelem_alloc = DICT_INITIAL_SIZE;
    dict->nelem_inuse = 0;
    return dict;
}

void dictionary_append(struct Dictionary **dict, char *s, unsigned type) {
    if ((*dict)->nelem_inuse + 1 > (*dict)->nelem_alloc) {
        struct Word **tmp;
        (*dict)->nelem_alloc += DICT_INITIAL_SIZE;
        tmp = realloc((*dict)->words, (*dict)->nelem_alloc * sizeof((*dict)->words));
        if (!tmp) {
            perror("Unable to extend word list");
            exit(1);
        }
        (*dict)->words = tmp;
    }
    (*dict)->words[(*dict)->nelem_inuse] = malloc(1 * sizeof(**(*dict)->words));
    (*dict)->words[(*dict)->nelem_inuse]->word = strdup(s);
    (*dict)->words[(*dict)->nelem_inuse]->nchar = strlen(s) - 1;
    *((*dict)->words[(*dict)->nelem_inuse]->word + ((*dict)->words[(*dict)->nelem_inuse]->nchar)) = '\0';
    (*dict)->words[(*dict)->nelem_inuse]->type = type;
    (*dict)->nelem_inuse++;
}

int dictionary_read(FILE *fp, struct Dictionary **dict, unsigned type) {
    char *buf;

    buf = malloc(DICT_WORD_SIZE_MAX * sizeof(char));
    while ((fgets(buf, DICT_WORD_SIZE_MAX - 1, fp) != NULL)) {
        if (errno) {
            return errno;
        }
        if (*buf == '\0' || *buf == '\n')
            continue;
        dictionary_append(&(*dict), buf, type);
    }
    free(buf);
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

    icase = type & WT_ICASE;
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
        size_t index = random() % dict->nelem_inuse;
        word = dict->words[index];
        if (word->type == type || type == WT_ANY) {
            return word->word;
        }
    }
}

void dictionary_free(struct Dictionary *dict) {
    for (size_t i = 0; i < dict->nelem_inuse; i++) {
        free(dict->words[i]->word);
        free(dict->words[i]);
    }
    free(dict->words);
    free(dict);
}

