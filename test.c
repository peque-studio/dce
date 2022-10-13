#include <stdio.h>

char s[] =
"H\307\300\001\000\000\000H\307\307\001\000\000\000H\2155\025\000\000\000H\307\302\r\000\000\000\017\005H\307\300<\000\000\000H\211\370\017\005"
;

int main() {
    for(int i = 0; i < sizeof(s); ++i)
        printf("%02x", (unsigned int)(unsigned char)s[i]);
    putc('\n', stdout);
}
