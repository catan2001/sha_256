#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void RotR(unsigned char *, unsigned int);
void ShR(unsigned char *, unsigned int);

#define H0 0x6a09e667
#define H1 0xbb67ae85
#define H2 0x3c6ef372
#define H3 0xa54ff53a
#define H4 0x510e527f
#define H5 0x9b05688c
#define H6 0x1f83d9ab
#define H7 0x5be0cd19

int main(void) {

    //TODO: implement when string has more than 128 char(multiple of 512)

    char *str = "AlokinCitac";
    unsigned char *ustr = str;
    unsigned long long int len = strlen(ustr); // length of string
    unsigned char *padded_msg = (unsigned char *)malloc(64 * sizeof(unsigned char));  
    unsigned char max_len = 64; // equates to 512bits

    unsigned char W[64][4] = {0}; //64 words of 32bits each...
    
    //initialization... {padding}
    if(len <= 64) {
        for(unsigned int i = 0; i < 64; ++i)
            padded_msg[i] = 0;
    }
    else if(len >=64 && len <= 128) {
        padded_msg = (unsigned char *)realloc(padded_msg, 128 * sizeof(unsigned char));
        for(unsigned int i = 0; i < 128; ++i)
            padded_msg[i] = 0;
        max_len = 128;
    }
    else
        printf("ERROR: length of string is too long! Maximum length is 128\n");

    printf("%s\n", padded_msg);
    strcpy(padded_msg, ustr);
    padded_msg[len] = (8 << 4); // append 1 to the end of the string     
    printf("%s\n", padded_msg);
    
    // last 64bits of padded_msg represent length len < 2^64
    // TODO: check correctness
    for(unsigned char i = 0; i < 8; ++i) {
        padded_msg[max_len - i] = ((0xff << i*8) & len) >> i*8;
        printf("padded_msg[%d] = %d\n", max_len-i, padded_msg[max_len-i]);
    }
    
    printf("len = %d \npadded_msg last = %d\n", len, padded_msg[max_len]);

    // the first 16 are obtained by splitting M in 32-bit blocks
    for(unsigned char i = 0; i < 16; ++i) 
        for(unsigned char j = 0; j < 4; ++j) {
            W[i][j] = padded_msg[i*4 + j];
            printf("W[%d][%d] = %c\n", i, j, W[i][j]);
        }

    unsigned char test[] = "abcd";
    printf("mjau");   
    RotR(test, 19);
    //printf("%x\n", test);
    unsigned char test2[] = "aaaa";
    ShR(test2, 10);
    unsigned char kk = 200;
    unsigned char kkk = 190;
    printf("%u\n", (unsigned char)(kk+kkk));
    //printf("%x", (unsigned char)(~0xa));
    
    /* 

    for(unsigned char i = 16; i < 64; ++i) {
        unsigned char sig0[4];
        unsigned char sig1[4];

        SigmaS0(W[i-15], sig0);
        SigmaS1(W[i-2], sig1);
        

    }
        

    */
    return 0;
}

void RotR(unsigned char *str, unsigned int num_shifts) {
    unsigned int max_len = strlen(str);
    unsigned char temp_last, shift_by;

    for(unsigned int i = 0; i < num_shifts; i += 8) {
        ((num_shifts-i) > 8) ? (shift_by = 8) : (shift_by = num_shifts%8);
        //printf("shift_by = %d\n", shift_by);
        temp_last = str[max_len-1];
        for(int j = max_len-1; j >= 0; --j) {
            //printf("str[%d] = %x\n", j, str[j]);
            str[j] = (str[j] >> shift_by); // 0000 1010 >> 3, 0000 0001
            if(j > 0)
                str[j] = str[j] | (str[j-1] << (8-shift_by)); // 0000 0001  | ((0001 0100 << 8-3) = )  
            else
                str[j] = str[j] | (temp_last << (8-shift_by));
            //printf("%x\n", str[j]);
        } 
    }
}

void ShR(unsigned char *str, unsigned int num_shifts) {
    unsigned int max_len = strlen(str);
    unsigned char shift_by;

    for(unsigned int i = 0; i < num_shifts; i += 8) {
        ((num_shifts-i) > 8) ? (shift_by = 8) : (shift_by = num_shifts%8);
        for(int j = max_len-1; j >= 0; --j) {
            //printf("str[%d] = %x\n", j, str[j]);
            str[j] = (str[j] >> shift_by); // 0000 1010 >> 3, 0000 0001
            if(j    > 0)
                str[j] = str[j] | (str[j-1] << (8-shift_by)); // 0000 0001  | ((0001 0100 << 8-3) = )  
            else 
                str[j] = str[j] >> shift_by;
            //printf("%x\n", str[j]);
        } 
    }
}

// TODO: test these tmrw
void Complement(unsigned char *str) {
    unsigned int len = strlen(str);
    for(int i = 0; i < len; ++i)
        str[i] = ~str[i];
}

void Xor(unsigned char *str1, unsigned char *str2, unsigned char *out) {
    if(strlen(str1) == strlen(str2)) {
        unsigned int len = strlen(str1);
        for(int i = 0; i < len; ++i)
            out[i] = str1[i] ^ str2[2];
    }
    else
        printf("ERROR: Xor, str1 and str2 must be of equal length!\n");
}

void Add32(unsigned char *str1, unsigned char *str2, unsigned char *out) {
    unsigned long long int num1 = 0, num2 = 0, sum = 0;
    for(int i = strlen(str1) - 1; i >= 0; --i)
        num1 = num1 | (unsigned int)str1[i] << i*8;
    for(int i = strlen(str2) - 1; i >= 0; --i)
        num2 = num2 | (unsigned int)str2[i] << i*8;
    sum = num1 + num2;
    
    for(int i = strlen(out) - 1; i >= 0; --i)
        out[i] = (sum >> ((strlen(out) - 1 - i)*8) & 0xff);
}

void SigmaL0(unsigned char *str1, unsigned char *out) {
    unsigned char str_c1[strlen(str1)] = {0};
    unsigned char str_c2[strlen(str1)] = {0};

    strcpy(str_c1, str1);
    strcpy(str_c2, str1);

    RotR(str_c1, 2);
    RotR(str_c2, 13);
    
    Xor(str_c1, str_c2, out);

    strcpy(str_c1, str1);
    RotR(str_c1, 22);
    Xor(str_c1, out, out);
}

void SigmaL1(unsigned char *str1, unsigned char *out) {
    unsigned char str_c1[strlen(str1)] = {0};
    unsigned char str_c2[strlen(str1)] = {0};

    strcpy(str_c1, str1);
    strcpy(str_c2, str1);

    RotR(str_c1, 6);
    RotR(str_c2, 11);
    
    Xor(str_c1, str_c2, out);

    strcpy(str_c1, str1);
    RotR(str_c1, 25);
    Xor(str_c1, out, out);
}

void SigmaS0(unsigned char *str1, unsigned char *out) {
    unsigned char str_c1[strlen(str1)] = {0};
    unsigned char str_c2[strlen(str1)] = {0};

    strcpy(str_c1, str1);
    strcpy(str_c2, str1);

    RotR(str_c1, 7);
    RotR(str_c2, 18 );
    
    Xor(str_c1, str_c2, out);

    strcpy(str_c1, str1);
    ShR(str_c1, 3);
    Xor(str_c1, out, out);
}

void SigmaS1(unsigned char *str1, unsigned char *out) {
    unsigned char str_c1[strlen(str1)] = {0};
    unsigned char str_c2[strlen(str1)] = {0};

    strcpy(str_c1, str1);
    strcpy(str_c2, str1);

    RotR(str_c1, 17);
    RotR(str_c2, 19 );
    
    Xor(str_c1, str_c2, out);

    strcpy(str_c1, str1);
    ShR(str_c1, 10);
    Xor(str_c1, out, out);
}

