#include "jdtalk.h"

static const char *usage_text = \
        "usage: %s [-h] [-befHlr] [-s salad_word_count] [-c line_limit] [-p pattern] [-a acronym]\n"
        "  -a str    Acronym mode\n"
        "  -b        Enable benchmark output\n"
        "  -c num    Output `num` lines\n"
        "  -e        Exact match (use with -p)\n"
        "  -f str    Custom output format\n"
        "            (a=adjective, d=adverb, n=noun, v=verb)\n"
        "  -h        Show this usage statement\n"
        "  -H        Produce hill-cased strings (hIlL cAsE)\n"
        "  -l        Produce leet speak strings (1337 5|*34|<)\n"
        "  -r        Produce random-case strings (raNdoM CasE)\n"
        "  -s num    Produce word salad (`num` words per line)\n"
        "  -t        Produce title-case strings (Title Case)\n"
        "\n";

static void usage(char *name) {
    char *begin;
    char buf[PATH_MAX];
    buf[0] = '\0';

    strcpy(buf, name);
    begin = strrchr(buf, '/');
    if (begin && strlen(begin)) {
        printf("%s\n", begin);
        memmove(begin, begin + 1, strlen(begin));
        printf("%s\n", begin);
    } else {
        begin = buf;
    }
    printf(usage_text, begin);
}

int main(int argc, char *argv[]) {
    struct Dictionary *dict;
    char buf[OUTPUT_SIZE_MAX];
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
    size_t limit;
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
    limit = 0;
    salad_limit = 10;

    strcpy(format, "andv");
    pattern[0] = '\0';
    buf[0] = '\0';
    acronym[0] = '\0';

    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            exit(0);
        }
        if (strcmp(argv[i], "-b") == 0) {
            do_benchmark = 1;
        }
        if (strcmp(argv[i], "-c") == 0) {
            limit = (int) strtol(argv[i + 1], NULL, 10);
        }
        if (strcmp(argv[i], "-p") == 0) {
            do_pattern = 1;
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
        if (strcmp(argv[i], "-a") == 0) {
            do_acronym = 1;
            do_title_case = 1;
            strcpy(acronym, argv[i + 1]);
        }
        if (strcmp(argv[i], "-t") == 0) {
            do_title_case = 1;
        }
    }

    dict = dictionary_populate();

    if (do_pattern && !dictionary_contains(dict, pattern, WT_ANY)) {
        fprintf(stderr, "Word not found in dictionary: %s\n", pattern);
        exit(1);
    }

    if ((do_pattern && do_acronym) && !acronym_safe(acronym, pattern)) {
        fprintf(stderr, "Word will never appear in acronym, '%s': %s\n", acronym, pattern);
        exit(1);
    }

    if (do_benchmark)
        start_time = (float)clock() / CLOCKS_PER_SEC;

    for (size_t i = 1; ; i++) {
        LABEL_TRY_AGAIN:
        memset(part, 0, sizeof(part) / sizeof(*part) * sizeof(char *));

        if (do_salad) {
            strcpy(buf, talk_salad(dict, salad_limit, part));
        } else if (do_acronym) {
            strcpy(buf, talk_acronym(dict, format, acronym, part));
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
        } else if (do_title_case) {
            str_title_case(buf);
        }
        puts(buf);

        if (limit && i == limit) {
            break;
        }
    }

    if (do_benchmark) {
        end_time = (float) clock() / CLOCKS_PER_SEC;
        time_elapsed = end_time - start_time;
        fprintf(stderr, "benchmark: %fs\n", time_elapsed);
    }

    dictionary_free(dict);
    return 0;
}
