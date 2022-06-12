#include "jdtalk.h"

static const char *usage_text = \
        "usage: %s [-h] [-befHlrtx] [-s salad_word_count] [-c line_limit] [-p pattern] [-a acronym]\n"
        "  -a str    Acronym mode\n"
        "  -b        Enable benchmark output\n"
        "  -c num    Output `num` lines\n"
        "  -j        Enable JSON output (requires -c)"
        "  -e        Exact match (use with -p)\n"
        "  -f str    Custom output format\n"
        "            (a=adjective, d=adverb, n=noun, v=verb)\n"
        "  -h        Show this usage statement\n"
        "  -H        Produce hill-cased strings (hIlL cAsE)\n"
        "  -l        Produce leet speak strings (1337 5|*34|<)\n"
        "  -p str    Search for `str` in output\n"
        "  -r        Produce random-case strings (raNdoM CasE)\n"
        "  -R        Produce reversed strings (sgnirts desrever)\n"
        "  -s num    Produce word salad (`num` words per line)\n"
        "  -S        Produce shuffled strings (fsfhleudf sntsrgi)\n"
        "  -t        Produce title-case strings (Title Case)\n"
        "  -x        Produce heart candy phrases\n"
        "\n";

/**
 * Print usage statement
 * @param name program name
 */
static void usage(char *name) {
    char *begin;
    char buf[PATH_MAX];
    buf[0] = '\0';

    // Get the basename of name
    strcpy(buf, name);
    begin = strrchr(buf, '/');
    if (begin && strlen(begin)) {
        memmove(begin, begin + 1, strlen(begin));
    } else {
        begin = buf;
    }

    printf(usage_text, begin);
}

/**
 * Validate s against possible arguments
 * @param possible short options
 * @param s input string to validate
 * @return 0=invalid, 1=valid
 */
static int argv_validate(const char *possible, char *s) {
    if (strlen(s) > 1) {
        // s is a short option (i.e. -c)
        for (size_t i = 0; i < strlen(possible); i++) {
            if (possible[i] == *(s + 1))
                // s is a valid short option
                return 1;
        }
    }
    // s is an invalid short option
    return 0;
}

#define ARG(X) strcmp(option, X) == 0
static const char *args_valid = "AabcefhHjlprRsStx";

int main(int argc, char *argv[]) {
    struct Dictionary *dict;
    char buf[OUTPUT_SIZE_MAX];
    char errbuf[OUTPUT_SIZE_MAX];
    char format[INPUT_SIZE_MAX];
    char pattern[INPUT_SIZE_MAX];
    char acronym[INPUT_SIZE_MAX];
    char *part[OUTPUT_PART_MAX];
    int found;
    int do_pattern;
    int do_exact;
    int do_acronym;
    int do_random_case;
    int do_hill_case;
    int do_leet;
    int do_salad;
    int do_benchmark;
    int do_title_case;
    int salad_limit;
    int do_shuffle;
    int do_reverse;
    int do_format;
    int do_heart;
    int do_json;
    size_t limit;
    size_t heart_limit;
    size_t heart_maxlen;
    float start_time;
    float end_time;
    float time_elapsed;

    do_pattern = 0;
    do_exact = 0;
    do_acronym = 0;
    do_random_case = 0;
    do_hill_case = 0;
    do_leet = 0;
    do_salad = 0;
    do_benchmark = 0;
    do_title_case = 0;
    do_shuffle = 0;
    do_reverse = 0;
    do_format = 0;
    do_heart = 0;
    do_json = 0;
    limit = 0;
    salad_limit = 10;
    heart_limit = 3;
    heart_maxlen = 5;

    strcpy(format, DEFAULT_FORMAT);
    pattern[0] = '\0';
    buf[0] = '\0';
    acronym[0] = '\0';

    srandom(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);

    for (int i = 1; i < argc; i++) {
        char *option;
        char *option_value;
        option = argv[i];
        option_value = argv[i + 1];

        if (!argv_validate(args_valid, option)) {
            fprintf(stderr, "Unknown argument: %s\n", option);
            usage(argv[0]);
            exit(1);
        }
        if (ARG("-h")) {
            usage(argv[0]);
            exit(0);
        }
        if (ARG("-b")) {
            do_benchmark = 1;
        }
        if (ARG("-j")) {
            do_json = 1;
        }
        if (ARG("-c")) {
            if (!option_value || *option_value == '-') {
                fprintf(stderr, "requires a positive integer option_value\n");
                exit(1);
            }

            limit = (int) strtol(option_value, NULL, 10);
            if (!limit) {
                limit = 1;
            }
            i++;
            continue;
        }
        if (ARG("-p")) {
            if (!option_value || *option_value == '-') {
                fprintf(stderr, "requires a dictionary word\n");
                exit(1);
            }
            do_pattern = 1;
            strcpy(pattern, option_value);
            i++;
            continue;
        }
        if (ARG("-e")) {
            do_exact = 1;
        }
        if (ARG("-r")) {
            do_random_case = 1;
        }
        if (ARG("-H")) {
            do_hill_case = 1;
        }
        if (ARG("-l")) {
            do_leet = 1;
        }
        if (ARG("-s")) {
            if (!option_value || *option_value == '-') {
                fprintf(stderr, "requires a positive integer option_value\n");
                exit(1);
            }

            do_salad = 1;
            salad_limit = (int) strtol(option_value, NULL, 10);
            if (!salad_limit) {
                salad_limit = 1;
            }
            i++;
            continue;
        }
        if (ARG("-x")) {
            do_heart = 1;
        }
        if (ARG("-f")) {
            do_format = 1;
            strcpy(format, option_value);
            i++;
            continue;
        }
        if (ARG("-a")) {
            if (!option_value) {
                fprintf(stderr, "requires a string\n");
                exit(1);
            }
            do_acronym = 1;
            do_title_case = 1;
            strcpy(acronym, option_value);
            i++;
            continue;
        }
        if (ARG("-t")) {
            do_title_case = 1;
        }
        if (ARG("-S")) {
            do_shuffle = 1;
        }
        if (ARG("-R")) {
            do_reverse = 1;
        }
    }

    dict = dictionary_populate();
    struct Dictionary *dicts[6] = {
        dict,             // ALL
        dictionary_of(&dict, WT_NOUN),
        dictionary_of(&dict, WT_ADJECTIVE),
        dictionary_of(&dict, WT_ADVERB),
        dictionary_of(&dict, WT_VERB),
        NULL,
    };

    if (do_json && limit) {
        JSON_BEGIN(stdout);
        JSON_LIST_BEGIN(stdout, "data");
    }

    if (do_pattern && !dictionary_contains(&dicts[1], pattern, WT_ANY)) {
        sprintf(errbuf, "Word not found in dictionary: %s", pattern);
        goto error_exit;
    }

    if (!format_safe(format)) {
        sprintf(errbuf, "Invalid format: %s", format);
        goto error_exit;
    }

    if ((do_pattern && do_acronym) && !acronym_safe(dict, acronym, pattern, do_format ? NULL: format)) {
        sprintf(errbuf, "Word will never appear in acronym, '%s': %s (format: %s)", acronym, pattern, format);
        goto error_exit;
    }

    if ((do_pattern && do_heart) && strlen(pattern) > heart_maxlen) {
        sprintf(errbuf, "Word is too long for heart mode: %s (%zu > %zu)", pattern, strlen(pattern), heart_maxlen);
        goto error_exit;
    }

    if (do_json && limit) {
        JSON_NEXT_LINE(stdout);
    }

    if (do_benchmark)
        start_time = (float)clock() / CLOCKS_PER_SEC;

    for (size_t i = 1; ; i++) {
        memset(part, 0, sizeof(part) / sizeof(*part) * sizeof(char *));

        if (do_salad) {
            strcpy(buf, talk_salad(dicts, salad_limit, part, OUTPUT_PART_MAX));
        } else if (do_heart) {
            strcpy(buf, talk_heart(dicts, heart_limit, heart_maxlen, part, OUTPUT_PART_MAX));
        } else if (do_acronym) {
            if (strcmp(format, DEFAULT_FORMAT) == 0) strcpy(format, "xxxx");
            strcpy(buf, talk_acronym(dicts, format, acronym, part, OUTPUT_PART_MAX));
        } else {
            strcpy(buf, talkf(dicts, format, part, OUTPUT_PART_MAX));
        }

        if (do_pattern) {
            found = 0;
            for (size_t z = 0; part[z] != NULL; z++) {
                if (do_exact) {
                    if (strcmp(part[z], pattern) == 0) {
                        found = 1;
                        break;
                    }
                } else {
                    if (strstr(buf, pattern)) {
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                i--;
                continue;
            }
        }

        if (do_random_case) {
            str_random_case(buf);
        } else if (do_hill_case) {
            str_hill_case(buf);
        } else if (do_leet) {
            strcpy(buf, str_leet(buf));
        } else if (do_title_case) {
            str_title_case(buf);
        } else if (do_shuffle) {
            str_randomize_words(buf);
        } else if (do_reverse) {
            str_reverse(buf);
        }

        if (do_json && limit) {
            JSON_LIST_APPEND(stdout, buf);
            if (i < limit)
                JSON_NEXT_ITEM(stdout);

        } else {
            puts(buf);
        }

        if (limit && i == limit) {
            break;
        }
    }

    if (do_json && limit) {
        JSON_NEXT_LINE(stdout);
        JSON_INDENT(stdout, 1);
        JSON_LIST_END(stdout);
        JSON_NEXT_ITEM(stdout);
        JSON_STRING(stdout, "error", "");
        JSON_NEXT_LINE(stdout);
        JSON_END(stdout);
    }

    if (do_benchmark) {
        end_time = (float) clock() / CLOCKS_PER_SEC;
        time_elapsed = end_time - start_time;
        fprintf(stderr, "benchmark: %fs\n", time_elapsed);
    }

    dictionary_free(dict);
    return 0;

    error_exit:
    if (do_json && limit) {
        JSON_NEXT_LINE(stdout);
        JSON_INDENT(stdout, 1);
        JSON_LIST_END(stdout);
        JSON_NEXT_ITEM(stdout);
        JSON_STRING(stdout, "error", errbuf);
        JSON_NEXT_LINE(stdout);
        JSON_END(stdout);
    } else {
        fprintf(stderr, "%s\n", errbuf);
    }
    exit(1);
}
