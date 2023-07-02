#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "utilities.h"

/*
On succes, return a pointer to a new, null-termined, heap allocated string that is a stripped (leading and trailing whitespace) version of the 
passed in "len"-long string pointed to by "str". The length of the newly returned buffer is at most ("len" + 1) bytes,
though it is null-terminated. On failure, return NULL. The caller is responsible for freeing the memory allocated by
this function.
*/
char *
strip_whitespace(const char *str, const size_t len) {
    const char *start = str;
    while (isspace(*start)) {
        start++;
    }

    /* either passed-in string was empty or composed of just whitespace - return an empty string */
    if (*start == '\0') {
        char *buf = malloc(1 * sizeof(char));
        if (!buf) return NULL;
        buf[0] = '\0';
        return buf;
    }

    const char *end = str + len - 1;
    while (isspace(*end)) {
        end--;
    }

    size_t new_buflen = end - start + 1 + 1;
    char *buf = malloc(new_buflen * sizeof(char));
    if (!buf) return NULL;
    memcpy(buf, start, end - start + 1);
    buf[new_buflen - 1] = '\0';
    
    return buf;
}