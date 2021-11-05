#include "jdtalk.h"

char *str_random_case(char *s) {
    size_t len;
    len = strlen(s);
    for (size_t i = 0; i < len; i++) {
        if ((rand() % 100) >= 50) {
            s[i] = (char)toupper(s[i]);
        }
    }
    return s;
}

char *str_hill_case(char *s) {
    size_t len;
    len = strlen(s);
    for (size_t i = 0; i < len; i++) {
        if (i % 2) {
            s[i] = (char)toupper(s[i]);
        }
    }
    return s;
}

char *str_leet(char *s) {
    size_t len;
    static char buf[OUTPUT_SIZE_MAX];
    memset(buf, '\0', sizeof(buf));
    len = strlen(s);
    for (size_t i = 0; i < len; i++) {
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

char *str_title_case(char *s) {
    size_t len;
    size_t i;

    i = 0;
    len = strlen(s);
    s[i] = (char) toupper(s[i]);
    for (; i < len; i++) {
        if (i < len - 1 && s[i] == ' ') {
            s[i + 1] = (char) toupper(s[i + 1]);
        }
    }
    return s;
}

char *str_randomize(char *s) {
    size_t len;
    char tmp = 0;
    len = strlen(s);
    for (size_t i = len - 1; i > 0; i--) {
        size_t from = random() % i + 1;
        tmp = s[from];
        s[from] = s[i];
        s[i] = tmp;
    }
    return s;
}

char *str_randomize_words(char *s) {
    char old[OUTPUT_SIZE_MAX];
    char buf[OUTPUT_SIZE_MAX];
    char *oldp;
    char *word;

    strcpy(old, s);
    oldp = old;
    memset(buf, '\0', sizeof(buf));
    size_t len = strlen(s);

    while ((word = strsep(&oldp, " ")) != NULL) {
        str_randomize(word);
        strcat(buf, word);
        strcat(buf, " ");
    }
    strncpy(s, buf, len);
    return s;
}

char *str_reverse(char *s) {
    size_t dest;
    char buf[1024];
    buf[0] = '\0';

    dest = 0;
    for (size_t src = strlen(s); src > 0; src--, dest++) {
        buf[dest] = s[src - 1];
    }
    buf[dest] = '\0';
    strcpy(s, buf);
    return s;
}
