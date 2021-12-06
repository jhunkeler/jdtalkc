#include "jdtalk.h"

/**
 * Initializes a dictionary
 * @return Empty initialized Dictionary structure
 */
struct Dictionary *dictionary_new() {
    struct Dictionary *dict;

    dict = malloc(1 * sizeof(*dict));
    if (!dict) {
        perror("Unable to initialize new dictionary");
        exit(1);
    }
    dict->words = malloc(DICT_INITIAL_SIZE * sizeof(**dict->words));
    if (!dict->words) {
        perror("Unable to initialize array of dictionary words");
        exit(1);
    }
    dict->nelem_alloc = DICT_INITIAL_SIZE;
    dict->nelem_inuse = 0;
    return dict;
}

void dictionary_grow_as_needed(struct Dictionary **dict) {
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
}

void dictionary_alloc_word_record(struct Dictionary **dict) {
    (*dict)->words[(*dict)->nelem_inuse] = malloc(1 * sizeof(**(*dict)->words));
    if (!(*dict)->words[(*dict)->nelem_inuse]) {
        perror("Unable to allocate dictionary word list record");
        exit(1);
    }
}

void dictionary_new_word(struct Dictionary **dict, char *s, unsigned type) {
    (*dict)->words[(*dict)->nelem_inuse]->word = strdup(s);
    if (!(*dict)->words[(*dict)->nelem_inuse]->word) {
        perror("Unable to allocate dictionary word in list");
        exit(1);
    }
    (*dict)->words[(*dict)->nelem_inuse]->nchar = strlen(s) - 1;
    *((*dict)->words[(*dict)->nelem_inuse]->word + ((*dict)->words[(*dict)->nelem_inuse]->nchar)) = '\0';
    (*dict)->words[(*dict)->nelem_inuse]->type = type;
}

struct Dictionary *dictionary_of(struct Dictionary **src, unsigned type) {
    struct Dictionary *dest;
    dest = dictionary_new();
    dictionary_grow_as_needed(&dest);
    for (size_t i = 0, x = 0; i < (*src)->nelem_inuse; i++) {
        if ((*src)->words[i]->type != type)
            continue;
        dictionary_alloc_word_record(&dest);
        dest->words[x] = malloc(sizeof(**(*src)->words));
        dest->words[x] = (*src)->words[i];
        dest->nelem_inuse++;
        x++;
    }
    return dest;
}

/**
 * Add a word to the dictionary
 *
 * @param dict pointer to dictionary
 * @param s string to append to word list
 * @param type type of word (WT_NOUN, WT_VERB, WT_ADVERB, WT_ADJECTIVE)
 */
void dictionary_append(struct Dictionary **dict, char *s, unsigned type) {
    dictionary_grow_as_needed(dict);
    dictionary_alloc_word_record(dict);
    dictionary_new_word(dict, s, type);
    (*dict)->nelem_inuse++;
}

/**
 * Extract words from a file and append them to a Dictionary
 * structure with a specific type
 *
 * @param fp raw dictionary file handle
 * @param dict pointer to Dictionary structure
 * @param type type of words in raw dictionary file
 * @return 0=success or value of errno on failure
 */
int dictionary_read(FILE *fp, struct Dictionary **dict, unsigned type) {
    char *buf;

    buf = malloc(DICT_WORD_SIZE_MAX * sizeof(char));
    if (!buf) {
        perror("Unable to allocate buffer for dictionary_read");
        exit(1);
    }
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

/**
 * Consume all dictionary files
 *
 * @return fully populated dictionary of words
 */
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

    // Types of words expected by files[] array
    const unsigned files_type[] = {
            WT_NOUN,
            WT_ADJECTIVE,
            WT_ADVERB,
            WT_VERB,
    };

    // Initialize the dictionary
    dict = dictionary_new();

    // Consume each dictionary in files[]
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

        // Append the contents of file[i] to dictionary
        dictionary_read(fp, &dict, files_type[i]);
        fclose(fp);
    }
    return dict;
}

/**
 * Get the types of a word
 * @param dict pointer to dictionary
 * @param s dictionary word to search for
 * @return a string containing the word types (i.e. n,a,d,v)
 */
char *dictionary_word_formats(struct Dictionary *dict, const char *s) {
    static char buf[OUTPUT_SIZE_MAX];
    buf[0] = '\0';

    for (size_t i = 0; i < dict->nelem_inuse; i++) {
        if (strcmp(dict->words[i]->word, s) != 0) {
            continue;
        }
        switch (dict->words[i]->type) {
            case WT_NOUN:
                strcat(buf, "n");
                break;
            case WT_ADJECTIVE:
                strcat(buf, "a");
                break;
            case WT_ADVERB:
                strcat(buf, "d");
                break;
            case WT_VERB:
                strcat(buf, "v");
                break;
            default:
                strcat(buf, "x");
                break;
        }
    }
    if (!strlen(buf)) {
        return NULL;
    }
    return buf;
}

/**
 * Search dictionary for a word
 *
 * int result;
 *
 * // "beef" exists and is a verb
 * result = dictionary_contains(dict, "beef", WT_VERB);
 * // 1
 *
 * // "beef" exists and is a noun
 * result = dictionary_contains(dict, "beef", WT_NOUN);
 * // 1
 *
 * // "beef" is not an adjective
 * result = dictionary_contains(dict, "beef", WT_ADJECTIVE);
 * // 0
 *
 * // "Beef" does not exist as written (case sensitive search)
 * result = dictionary_contains(dict, "Beef", WT_NOUN);
 * // 0
 *
 * // "Beef" exists (case insensitive search) and is a noun
 * result = dictionary_contains(dict, "Beef", WT_NOUN | WT_ICASE);
 * // 1
 *
 * // "Beef" exists (case insensitive search), matching any type of word
 * result = dictionary_contains(dict, "Beef", WT_ANY | WT_ICASE);
 * // 1
 *
 * @param dict pointer to populated dictionary
 * @param s pointer to pattern string
 * @param type type of word (WT_NOUN, WT_VERB, WT_ADVERB, WT_ADJECTIVE) || (WT_ANY, WT_ICASE)
 * @return 0=not found, !0=found
 */
int dictionary_contains(struct Dictionary *dict, const char *s, unsigned type) {
    int result;
    unsigned icase;

    icase = type & WT_ICASE; // Determine case-sensitivity of the search function
    type &= 0x7f;  // Strip case-insensitive flag from type
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

/**
 * Produce a random word from the dictionary of type
 *
 * When type is WT_ANY, a random word irrespective of type will be produced
 *
 * @param dict pointer to dictionary
 * @param type type of word to produce
 * @return pointer to dictionary word
 */
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

/**
 * Free a dictionary
 * @param dict pointer to dictionary
 */
void dictionary_free(struct Dictionary *dict) {
    for (size_t i = 0; i < dict->nelem_inuse; i++) {
        free(dict->words[i]->word);
        free(dict->words[i]);
    }
    free(dict->words);
    free(dict);
}

