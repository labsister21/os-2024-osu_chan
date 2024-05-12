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
        ptr[i] = '\0';
    }
}

void rstrip(char *string) {

    int length = strlen(string);

    int length_bruto = length - 1;
    for(int i = 0; i < length_bruto ; i++){
        if(string[i] == ' ' || string[i] == '\t' || string[i] == '\n'){
            string[i] = '\0';
        }
    }
}


void deleteSpace(char *string){   

    for(size_t k = 0 ; k < 2048 ;k++){
        if(string[k] == ' '){
                string[k] = '\0';
        }
    }

}


// int lenArgValidation(char *string){
//         int start_index;
//         int last_arg_index;

//         for(int i = 0; i < 2048; i++){
//             if(string[i] != '\0'){
//                 start_index = i;
//                 break;
//             }
//         }

//         for(int j = start_index; j < 2048; j++){
//             if(string[j] == '\0'){
//                 last_arg_index = j - 1;
//                 break;
//             }
//         }

//         if(((last_arg_index - start_index) + 1) > 8){
//             return 0;
//         } 

//         return 1;
// }