#include <stddef.h>
#include "utils.h"

uint64_t strlen(const char *str) {
    uint64_t length = 0;
    if (str != NULL) {
        while(str[length] != '\0') {
            ++length;
        }
    }

    return length;
}

int strcmp(const char *a, const char *b) {
    uint64_t a_len = strlen(a);
    uint64_t b_len = strlen(b);

    if (a_len < b_len)
        return -1;
    if (a_len > b_len)
        return 1;
    for(uint64_t i = 0; i < a_len; ++i) {
        if (a[i] != b[i]) {
            if (a[i] < b[i]) {
                return -1;
            }
            return 1;
        }
    }

    return 0;
}

void strcpy(char *destination, const char *source) {
    uint64_t source_len = strlen(source);
    if (source != NULL) {
        for(uint64_t i = 0; i < source_len; ++i)
            destination[i] = source[i];
        destination[source_len] = '\0';
    }
}
