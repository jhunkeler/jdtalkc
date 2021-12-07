#include "jdtalk.h"

/**
 * Produce an output string containing various user-defined types of words
 *
 * a = adjective
 * d = adverb
 * n = noun
 * v = verb
 * x = any
 *
 * char *parts[1024]; // probably more than enough, right?
 * talkf(dict, "adnvx", &parts);
 *
 * @param dict pointer to dictionary array
 * @param fmt
 * @param parts
 * @return
 */
char *talkf(struct Dictionary *dict[], char *fmt, char **parts, size_t parts_max) {
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
            case 'x':
                word = dictionary_word(dict[WT_ANY], WT_ANY);
                break;
            case 'a':
                word = dictionary_word(dict[WT_ADJECTIVE], WT_ADJECTIVE);
                break;
            case 'd':
                word = dictionary_word(dict[WT_ADVERB], WT_ADVERB);
                break;
            case 'n':
                word = dictionary_word(dict[WT_NOUN], WT_NOUN);
                break;
            case 'v':
                word = dictionary_word(dict[WT_VERB], WT_VERB);
                break;
            default:
                fprintf(stderr, "INVALID FORMAT: %x\n", fmt[i]);
                break;
        }

        if (parts) {
            if (i < parts_max) {
                parts[i] = word;
            } else {
                // We reached the maximum number of parts. Stop processing.
                break;
            }
        }

        if (word) {
            strncat(buf, word, OUTPUT_SIZE_MAX);
            if (i < len - 1)
                strcat(buf, " ");
        }
    }
    return buf;
}

char *talk_salad(struct Dictionary *dict[], size_t limit, char **parts, size_t parts_max) {
    static char buf[OUTPUT_SIZE_MAX];
    buf[0] = '\0';
    for (size_t i = 0; i < limit; i++) {
        strncat(buf, talkf(dict, "x", parts, parts_max), OUTPUT_SIZE_MAX);
        if (i < limit - 1) {
            strcat(buf, " ");
        }
    }
    return buf;
}

char *talk_acronym(struct Dictionary *dict[], char *fmt, char *s, char **parts, size_t parts_max) {
    size_t s_len;
    static char buf[OUTPUT_SIZE_MAX];
    static char *local_parts[OUTPUT_PART_MAX];
    buf[0] = '\0';
    s_len = strlen(s);
    /*
    size_t format_len;
    char format[INPUT_SIZE_MAX];
    format[0] = '\0';
    if (fmt) {
        strcpy(format, fmt);
    } else {
        strcpy(format, "x");
    }

    format_len = strlen(fmt);
    if (format_len > s_len) {
        *(format + s_len) = '\0';
    }

    size_t x;
    x = 0;
     */
    for (size_t i = 0; i < s_len; i++) {
        char word[OUTPUT_SIZE_MAX];
        word[0] = '\0';
        while(1) {
            // Disable formatted output (again!)
            //char elem[2] = {0, 0};
            //elem[0] = format[x];
            strcpy(word, talkf(dict, "x", &local_parts[i], parts_max));
            if (*word == s[i]) {
                strncat(buf, word, OUTPUT_SIZE_MAX);
                if (i < s_len - 1) {
                    strcat(buf, " ");
                }
                break;
            }
        }
        if (parts) {
            if (i < parts_max) {
                //printf("parts[%zu]=%s\n", i, word);
                parts[i] = local_parts[i];
            } else {
                // We reached the maximum number of parts. Stop processing.
                break;
            }
        }
        /*
        if (x < format_len - 1) {
            x++;
        }
         */
    }
    return buf;
}

int acronym_safe(struct Dictionary *dict, const char *acronym, const char *pattern, const char *fmt) {
    size_t acronym_len;
    size_t fmt_len;
    size_t types_len;
    int pattern_valid;
    int format_valid;
    pattern_valid = 0;
    acronym_len = strlen(acronym);
    for (size_t i = 0; i < acronym_len; i++) {
        if (*pattern == acronym[i]) {
            pattern_valid = 1;
            break;
        }
    }

    format_valid = 1;
    if (fmt) {
        format_valid = 0;
        char *types;
        fmt_len = strlen(fmt);
        types = dictionary_word_formats(dict, pattern);
        types_len = strlen(types);

        for (size_t x = 0; x < types_len; x++) {
            if (format_valid) break;
            for (size_t i = 0; i < fmt_len; i++) {
                if (types[x] == fmt[i]) {
                    format_valid = 1;
                    break;
                }
            }

        }
    }

    return pattern_valid - format_valid == 0;
}

int format_safe(char *s) {
    size_t valid;
    const char *formatter = DEFAULT_FORMAT"x";

    valid = 0;
    for (size_t i = 0; i < strlen(formatter); i++) {
        for (size_t x = 0; x < strlen(s); x++) {
            if (s[x] == formatter[i]) {
                valid++;
            }
        }
    }
    return valid == strlen(s);
}