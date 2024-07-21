#include "sha256.h"

int sha_256(char *str, unsigned int *output_string){
    // TODO: implement later when string is larger than 64
    unsigned char *ustr = str;
    unsigned long long int len = strlen(ustr); // length of string 
    unsigned long long int bit_len = len*8;
    
    if(strlen(str) > 55 || (sizeof(output_string)) < 8) {
        return -1;
    }
    //unsigned int num_blocks = len/55 + 1; // 55 characters = 64(8*64 = 512) - 8(8*8 = 64 bits for length) - 1(1 bit for '1' to append)
    //unsigned char **padded_msg = (unsigned char**)malloc(bit_len * sizeof(unsigned char*));
    //for(unsigned int i = 0; i < bit_len; ++i)
    unsigned char *padded_msg = (unsigned char *)malloc(64 * sizeof(unsigned char));
    
    unsigned char max_len = 64; // equates to 512bits
    unsigned int M[16]; // 16x32 bits = 512bits
    unsigned int W[64]; // 64 words of 32bits each

    unsigned int H[2][8];
    
    unsigned int T1;
    unsigned int T2;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned int e;
    unsigned int f;
    unsigned int g;
    unsigned int h;
    
    //initialization... {padding}
    for(unsigned int i = 0; i < 64; ++i)
        padded_msg[i] = 0;

    strcpy(padded_msg, ustr);
    padded_msg[len] = (8 << 4); // append 1 to the end of the string     

    for(unsigned char i = 0; i < 14; ++i) {
        M[i] = ((padded_msg[i*4] << 8*3) | (padded_msg[i*4 +1] << 8*2) | (padded_msg[i*4 + 2] << 8) | padded_msg[i*4 + 3]);
    }
    M[14] = (bit_len >> 4*8) & 0xffffffff;
    M[15] = bit_len & 0xffffffff;
    
    //calculating first 16 W
    for(unsigned char i = 0; i < 16; ++i)
        W[i] = M[i];
    //calculating the rest 48 W
    for(unsigned char i = 16; i < 64; ++i)
        W[i] = (SigmaS1(W[i-2]) + W[i-7] + SigmaS0(W[i-15]) + W[i-16]);
    //init 0th H:
    for(unsigned char i = 0; i < 8; ++i) {
        H[0][i] = H0[i];
    }

    //HASH computation:
    a = H[0][0];
    b = H[0][1];
    c = H[0][2];
    d = H[0][3];
    e = H[0][4];
    f = H[0][5];
    g = H[0][6];
    h = H[0][7];
    for(unsigned char i = 0; i < 64; ++i){
        T1 = h + SigmaL1(e) + Ch(e, f, g) + K[i] + W[i];
        T2 = SigmaL0(a) + Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }
    H[1][0] = H[0][0] + a;
    H[1][1] = H[0][1] + b;
    H[1][2] = H[0][2] + c;
    H[1][3] = H[0][3] + d;
    H[1][4] = H[0][4] + e;
    H[1][5] = H[0][5] + f;
    H[1][6] = H[0][6] + g;
    H[1][7] = H[0][7] + h;
   
    output_string[0] = H[1][0];
    output_string[1] = H[1][1];
    output_string[2] = H[1][2];
    output_string[3] = H[1][3];
    output_string[4] = H[1][4];
    output_string[5] = H[1][5];
    output_string[6] = H[1][6];
    output_string[7] = H[1][7];

    //printf("HASH calculated: ");
    //for(unsigned char i = 0; i < 8; ++i)
    //    printf("%04x", H[1][i]);

    free(padded_msg);
    return 0;
}
