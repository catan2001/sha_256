#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//functions used:
#define RotR32(X, C) ((X >> C) | (X << (32-C))) // rotate right 32bit number for C steps
#define ShR32(X, C) (X >> C) // shift right 32bit number for C steps
#define Complement(X) (~X)
#define Xor32(X, Y) (X ^ Y)
#define Add32(X, Y) (X + Y)
#define SigmaL0(X) (RotR32(X, 2) ^ RotR32(X, 13) ^ RotR32(X, 22))
#define SigmaL1(X) (RotR32(X, 6) ^ RotR32(X, 11) ^ RotR32(X, 25))
#define SigmaS0(X) (RotR32(X, 7) ^ RotR32(X, 18) ^ ShR32(X, 3))
#define SigmaS1(X) (RotR32(X, 17) ^ RotR32(X, 19) ^ ShR32(X, 10))
#define Ch(X, Y, Z) ((X & Y) ^ (~X & Z))
#define Maj(X, Y, Z) ((X & Y) ^ (X & Z) ^ (Y & Z))

static const unsigned int H0[8] =   {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

static const unsigned int K[64] =   {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                                    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                                    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                                    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                                    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                                    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                                    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                                    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};


int main(void) {

    // TODO: implement later when string is larger than 64
    char *str = "AlokinCitac";
    unsigned char *ustr = str;
    unsigned long long int len = strlen(ustr); // length of string 
    unsigned long long int bit_len = len*8;
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
    
    //for(unsigned int block = 0; block < num_blocks; ++block) {
    //initialization... {padding}
    for(unsigned int i = 0; i < 64; ++i)
        padded_msg[i] = 0;

    //if(block == num_blocks-1)
    strcpy(padded_msg, ustr);
    //else
    //{
    //    for(unsigned char i = 0; i < 64; ++i)
    //        padded_msg[block][i] = ustr[i + block*64];
    //}   
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
        printf("%04x\n", H[0][i]);
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
    
    printf("HASH calculated: ");
    for(unsigned char i = 0; i < 8; ++i)
        printf("%04x", H[1][i]);

    printf("\n\n\n");
    //}
    //for(unsigned int i = 0; i < num_blocks; ++i)
    //  free(padded_msg[i]);
    free(padded_msg);
    return 0;
}
