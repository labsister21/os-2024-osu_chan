#include "clear.h"

void clear_command(){
    syscall(8, 0, 0, 0);
}