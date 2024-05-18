#include <stdint.h>
#include <stddef.h>
#include "header/stdlib/string.h"

void* memset(void *s, int c, size_t n) {
    uint8_t *buf = (uint8_t*) s;
    for (size_t i = 0; i < n; i++)
        buf[i] = (uint8_t) c;
    return s;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    for (size_t i = 0; i < n; i++)
        dstbuf[i] = srcbuf[i];
    return dstbuf;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *buf1 = (const uint8_t*) s1;
    const uint8_t *buf2 = (const uint8_t*) s2;
    for (size_t i = 0; i < n; i++) {
        if (buf1[i] < buf2[i])
            return -1;
        else if (buf1[i] > buf2[i])
            return 1;
    }

    return 0;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    if (dstbuf < srcbuf) {
        for (size_t i = 0; i < n; i++)
            dstbuf[i]   = srcbuf[i];
    } else {
        for (size_t i = n; i != 0; i--)
            dstbuf[i-1] = srcbuf[i-1];
    }

    return dest;
}

size_t strlen(char *str) {
    size_t i = 0;
    
    while (str[i] != '\0'){
        i++;
    }
    
    return i;
}

void clear(void* str, size_t n) {
    uint8_t* ptr = (uint8_t*) str;
    for (size_t i = 0; i < n; i++) {
        ptr[i] = 0x0;
    }
}


char *strchr_custom(const char *str, int character) {
    while (*str != '\0') {
        if (*str == character) {
            return (char *)str;
        }
        str++;
    }
    return NULL;
}

char *strtok(char *str, const char *delim) {
    static char *lasts;
    char *tok;

    // If str is not NULL, we are starting a new string
    if (str != NULL) {
        lasts = str;
    }

    // If lasts is NULL, there are no more tokens
    if (lasts == NULL) {
        return NULL;
    }

    // Skip leading delimiters
    while (*lasts && strchr_custom(delim, *lasts)) {
        lasts++;
    }

    // If we reached the end of the string, return NULL
    if (*lasts == '\0') {
        lasts = NULL;
        return NULL;
    }

    // This is the start of the token
    tok = lasts;

    // Find the end of the token
    while (*lasts && !strchr_custom(delim, *lasts)) {
        lasts++;
    }

    // If we reached a delimiter, replace it with '\0' and move the pointer forward
    if (*lasts) {
        *lasts = '\0';
        lasts++;
    } else {
        // If we reached the end of the string, set lasts to NULL
        lasts = NULL;
    }

    return tok;
}

int strparse(char *str, char command[12][128], char *delim)
{
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            command[i][j] = '\0';
        }
    }
    char *token;
    int i = 0;

    // Parse the string
    token = strtok(str, delim);
    while (token != NULL && i < 12) {

        memcpy(command[i], token, 127); // Limit to 127 to ensure null-termination
        command[i][127] = '\0'; // Ensure null-termination
        // Increment the count and move to the next token
        i++;
        token = strtok(NULL, delim);
    }

    return i;
};