#include "sha256.h"
#include <stdio.h>

int main(void) {
    // TODO: implement later when string is larger than 64
    char str[] = "AlokinCitac";
    unsigned int hash[8];
    if(sha_256(str, hash) == 0)
        for(unsigned char i = 0; i < 8; ++i)
            printf("%04x ", hash[i]);
    else
        printf("ERROR: could not compute hash\n");
    return 0;
}
