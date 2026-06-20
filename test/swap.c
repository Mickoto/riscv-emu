#include "stdlib.h"

void main() {
    char i = 'b';
    char j = 'a';
    i = i + j;
    j = i - j;
    i = i - j;
    putc(i);
    putc(j);
}
