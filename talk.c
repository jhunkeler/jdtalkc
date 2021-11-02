#include "jdtalk.h"

char *talkf(struct Dictionary *dict, const char *fmt, char **parts) {
    static char buf[OUTPUT_SIZE_MAX];
    buf[0] = '\0';

    if (!fmt || !strlen(fmt)) {
        return NULL;
    }

    size_t len;
    len = strlen(fmt);
    for (size_t i = 0; i < len; i++) {
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

        if (parts)
            parts[i] = word;

        strncat(buf, word, OUTPUT_SIZE_MAX);

        if (i < len - 1)
            strcat(buf, " ");
    }
    return buf;
}

char *talk_salad(struct Dictionary *dict, size_t limit, char **parts) {
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

char *talk_acronym(struct Dictionary *dict, const char *fmt, char *s, char **parts) {
    size_t len;
    static char buf[OUTPUT_SIZE_MAX];
    buf[0] = '\0';

    len = strlen(s);
    for (size_t i = 0; i < strlen(s); i++) {
        char *word = NULL;
        while(1) {
            word = dictionary_word(dict, WT_ANY);
            if (*word == s[i]) {
                break;
            }
            /* TODO: Formatted acronyms are too slow. Need a better way.
            if (strlen(fmt) < strlen(s)) {
                return NULL;
            }

            char letter[2] = {'\0', '\0'};
            for (size_t f = 0; f < strlen(fmt); f++) {
                *letter = fmt[f];
                word = talkf(dict, letter, NULL);
                if (*word == s[i]) {
                    done = 1;
                    break;
                }
            }
             */
        }
        parts[i] = word;
        strncat(buf, word, OUTPUT_SIZE_MAX);
        if (i < len - 1) {
            strcat(buf, " ");
        }
    }
    return buf;
}

int acronym_safe(const char *acronym, const char *pattern) {
    size_t acronym_len;
    int pattern_valid;
    pattern_valid = 0;
    acronym_len = strlen(acronym);
    for (size_t i = 0; i < acronym_len; i++) {
        if (*pattern == acronym[i]) {
            pattern_valid = 1;
            break;
        }
    }

    return pattern_valid;
}

