#include "stdint.h"

uint32_t ceil_div(uint32_t n1, uint32_t n2){
    if(! n1 % n2 == 0){
        return (n1 / n2) + 1;
    }
    return n1 / n2;
}