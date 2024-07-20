#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void RotR(unsigned char *, unsigned char, unsigned int);
void ShR(unsigned char *, unsigned char, unsigned int);
void Complement(unsigned char *, unsigned char);
void Xor(unsigned char *, unsigned char *, unsigned char, unsigned char *);
void Add32(unsigned char *, unsigned char *, unsigned char, unsigned char *);
void SigmaL0(unsigned char *, unsigned char, unsigned char *);
void SigmaL1(unsigned char *, unsigned char, unsigned char *);
void SigmaS0(unsigned char *, unsigned char, unsigned char *);
void SigmaS1(unsigned char *, unsigned char, unsigned char *);
void Ch(unsigned char *, unsigned char *, unsigned char *, unsigned char, unsigned char *);
void Maj(unsigned char *, unsigned char *, unsigned char *, unsigned char, unsigned char *);

static const unsigned int H0[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

static const unsigned int Ks[64] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
                                0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
                                0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
                                0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
                                0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
                                0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
                                0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
                                0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};


int main(void) {

    //TODO: implement when string has more than 128 char(multiple of 512)

    char *str = "aa";
    unsigned char *ustr = str;
    unsigned long long int len = strlen(ustr); // length of string
    unsigned char *padded_msg = (unsigned char *)malloc(64 * sizeof(unsigned char));  
    unsigned char max_len = 64; // equates to 512bits

    unsigned char W[64][4] = {0}; //64 words of 32bits each...
    unsigned char H[2][8][4];
    unsigned char K[64][4];
    
    unsigned char T1[4];
    unsigned char T2[4];
    unsigned char a[4];
    unsigned char b[4];
    unsigned char c[4];
    unsigned char d[4];
    unsigned char e[4];
    unsigned char f[4];
    unsigned char g[4];
    unsigned char h[4];
    
    for(unsigned char i = 0; i < 8; ++i){
        for(char j = 4-1; j >=0; --j) {
            H[0][i][j] = (H0[i] >> (3-j)*8)&0xff;
        }
    }

    for(unsigned char i = 0; i < 64; ++i){
        for(char j = 4-1; j >=0; --j) {
            K[i][j] = (Ks[i] >> (3-j)*8)&0xff;
        }
    }

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

    // construct the 64 blocks Wi from M
    // the first 16 are obtained by splitting M in 32-bit blocks
    for(unsigned char i = 0; i < 16; ++i) 
        for(unsigned char j = 0; j < 4; ++j) {
            W[i][j] = padded_msg[i*4 + j];
            printf("W[%d][%d] = %c\n", i, j, W[i][j]);
        }
    // the leftover 48 blocks
    unsigned char temp_w1[4];
    unsigned char temp_w2[4];
    unsigned char temp1[4];
    unsigned char temp2[4];
    unsigned char sum[4];
    for(unsigned char i = 16; i < 64; ++i) {
        for(unsigned char j = 0; j < 4; ++j) {
            temp_w1[j] = W[i-2][j];
            temp_w2[j] = W[i-15][j];
        }
        SigmaS1(temp_w1, 4, temp1); 
        SigmaS0(temp_w2, 4, temp2);
        Add32(temp1, temp2, 4, sum);

        for(unsigned char j = 0; j < 4; ++j) {
            temp_w1[j] = W[i-7][j];
            temp_w2[j] = W[i-16][j];
        }
        Add32(sum, temp_w1, 4, sum);
        Add32(sum, temp_w2, 4, sum);
        for(unsigned char j = 0; j < 4; ++j)
            W[i][j] = sum[j];
    }
    
    for(unsigned char i = 0; i < 4; ++i) {
        a[i] = H[0][0][i];
        b[i] = H[0][1][i];
        c[i] = H[0][2][i];
        d[i] = H[0][3][i];
        e[i] = H[0][4][i];
        f[i] = H[0][5][i];
        g[i] = H[0][6][i];
        h[i] = H[0][7][i];
    }

    for(unsigned char i = 0; i < 64; ++i) {
        // calculating T1
        for(unsigned char j = 0; j < 4; ++j) {
            temp_w1[j] = W[i][j];
            temp1[j] = K[i][j];
        }        
        Add32(temp_w1, temp1, 4, sum);
        Ch(e, f, g, 4, temp2);
        Add32(sum, temp2, 4, sum);
        Add32(sum, h, 4, sum);
        SigmaL1(e, 4, temp2);
        Add32(sum, temp2, 4, T1);

        //calculating T2
        SigmaL0(a, 4, temp1);
        Maj(a, b, c, 4, temp2);
        Add32(temp1, temp2, 4, T2);

        //rest
        for(unsigned char j = 0; j < 4; ++j) {
            h[j] = g[j];
            g[j] = f[j];
            f[j] = e[j];
        }
        Add32(d, T1, 4, e);
        
        for(unsigned char j = 0; j < 4; ++j) {
            d[j] = c[j];
            c[j] = b[j];
            b[j] = a[j];
        }
        Add32(T1, T2, 4, a);

        // H1, H2
        for(unsigned char j = 0; j < 4; ++j) {
            temp1[j] = H[0][0][j];
            temp2[j] = H[0][1][j];
        }
        Add32(temp1, a, 4, temp_w1);
        Add32(temp2, b, 4, temp_w2);
        for(unsigned char j = 0; j < 4; ++j) {
            H[1][0][j] = temp_w1[j]; 
            H[1][1][j] = temp_w2[j];
        }

        // H3, H4
        for(unsigned char j = 0; j < 4; ++j) {
            temp1[j] = H[0][2][j];
            temp2[j] = H[0][3][j];
        }
        Add32(temp1, c, 4, temp_w1);
        Add32(temp2, d, 4, temp_w2);
        for(unsigned char j = 0; j < 4; ++j) {
            H[1][2][j] = temp_w1[j];
            H[1][3][j] = temp_w2[j];
        }

        // H5, H6
        for(unsigned char j = 0; j < 4; ++j) {
            temp1[j] = H[0][4][j];
            temp2[j] = H[0][5][j];
        }
        Add32(temp1, e, 4, temp_w1);
        Add32(temp2, f, 4, temp_w2);
        for(unsigned char j = 0; j < 4; ++j) {
            H[1][4][j] = temp_w1[j];
            H[1][5][j] = temp_w2[j];
        }

        // H7, H8
        for(unsigned char j = 0; j < 4; ++j) {
            temp1[j] = H[0][6][j];
            temp2[j] = H[0][7][j];
        }
        Add32(temp1, g, 4, temp_w1);
        Add32(temp2, h, 4, temp_w2);
        for(unsigned char j = 0; j < 4; ++j) {
            H[1][6][j] = temp_w1[j];
            H[1][7][j] = temp_w2[j];
        }

    }

    for(unsigned char i = 0; i < 8; ++i) {
        for(unsigned char j = 0; j < 4; ++j){
            printf("%x", H[1][i][j]);
        }
        printf(" ");
    }

    free(padded_msg);
    return 0;
}

void RotR(unsigned char *str, unsigned char max_len, unsigned int num_shifts) {
    unsigned char temp_last, shift_by;
    //printf("correct\n");
    //printf("max_len: %d\n", max_len);
    for(unsigned int i = 0; i < num_shifts; i += 8) {
        ((num_shifts-i) > 8) ? (shift_by = 8) : (shift_by = num_shifts%8);
        //printf("shift_by = %d\n", shift_by);
        temp_last = str[max_len-1];
        //printf("correct\n");

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

void ShR(unsigned char *str, unsigned char max_len, unsigned int num_shifts) {
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

void Complement(unsigned char *str, unsigned char len) {
    for(int i = 0; i < len; ++i)
        str[i] = ~str[i];
}

void Xor(unsigned char *str1, unsigned char *str2, unsigned char len, unsigned char *out) {
        for(int i = 0; i < len; ++i) 
            out[i] = str1[i] ^ str2[i];
}

void Add32(unsigned char *str1, unsigned char *str2, unsigned char len, unsigned char *out) {
    unsigned long long int num1 = 0, num2 = 0, sum = 0;
    for(int i = len - 1; i >= 0; --i)
        num1 = num1 | (unsigned int)str1[i] << i*8;
    for(int i = len - 1; i >= 0; --i)
        num2 = num2 | (unsigned int)str2[i] << i*8;
    sum = num1 + num2;
    for(int i = len - 1; i >= 0; --i)
        out[i] = (sum >> ((len - 1 - i)*8) & 0xff);
}

// TODO: test these tmrw
void SigmaL0(unsigned char *str1, unsigned char len, unsigned char *out) {
    unsigned char *str_c1 = (unsigned char* )malloc(len * sizeof(unsigned char));
    unsigned char *str_c2 = (unsigned char* )malloc(len * sizeof(unsigned char));
    
    for(unsigned char i = 0; i < len; ++i) {
        str_c1[i] = str1[i];
        str_c2[i] = str1[i];
    }

    RotR(str_c1, len, 2);
    RotR(str_c2, len, 13);
    
    Xor(str_c1, str_c2, len, out);

    //strcpy(str_c1, str1);
    for(unsigned char i = 0; i < len; ++i) {
        str_c1[i] = str1[i];
    }
    
    RotR(str_c1, len, 22);
    Xor(str_c1, out, len, out);

    free(str_c1);
    free(str_c2);
}

void SigmaL1(unsigned char *str1, unsigned char len, unsigned char *out) {
    unsigned char *str_c1 = (unsigned char* )malloc(len * sizeof(unsigned char));
    unsigned char *str_c2 = (unsigned char* )malloc(len * sizeof(unsigned char));

    //strcpy(str_c1, str1);
    //strcpy(str_c2, str1);
    for(unsigned char i = 0; i < len; ++i) {
        str_c1[i] = str1[i];
        str_c2[i] = str1[i];
    }

    RotR(str_c1, len, 6);
    RotR(str_c2, len, 11);
    
    Xor(str_c1, str_c2, len, out);

    //strcpy(str_c1, str1);
    for(unsigned char i = 0; i < len; ++i) {
        str_c1[i] = str1[i];
    }

    RotR(str_c1, len, 25);
    Xor(str_c1, out, len, out);
    
    free(str_c1);
    free(str_c2);
}

void SigmaS0(unsigned char *str1, unsigned char len, unsigned char *out) {
    unsigned char *str_c1 = (unsigned char* )malloc(len * sizeof(unsigned char));
    unsigned char *str_c2 = (unsigned char* )malloc(len * sizeof(unsigned char));

    //strcpy(str_c1, str1);
    //strcpy(str_c2, str1);
    for(unsigned char i = 0; i < len; ++i) {
        str_c1[i] = str1[i];
        str_c2[i] = str1[i];
    }

    RotR(str_c1, len, 7);
    RotR(str_c2, len, 18);
    
    Xor(str_c1, str_c2, len, out);

    //strcpy(str_c1, str1);
    for(unsigned char i = 0; i < len; ++i) {
        str_c1[i] = str1[i];
    }


    ShR(str_c1, len, 3);
    Xor(str_c1, out, len, out);

    free(str_c1);
    free(str_c2);
}

void SigmaS1(unsigned char *str1, unsigned char len, unsigned char *out) {
    unsigned char *str_c1 = (unsigned char* )malloc(len * sizeof(unsigned char));
    unsigned char *str_c2 = (unsigned char* )malloc(len * sizeof(unsigned char));

    //strcpy(str_c1, str1);
    //strcpy(str_c2, str1);
    for(unsigned char i = 0; i < len; ++i) {
        str_c1[i] = str1[i];
        str_c2[i] = str1[i];
    }

    //printf("%c\n", str_c1[0]);
    //printf("%c\n", str_c2[1]);
    //printf("entered sigma s1!\n");
    RotR(str_c1, len, 17);
    RotR(str_c2, len, 19 );
    //printf("entered!\n");
   
    Xor(str_c1, str_c2, len, out);
    //printf("enteredd!\n");

    //strcpy(str_c1, str1);
    for(unsigned char i = 0; i < len; ++i) {
        str_c1[i] = str1[i];
    }

    ShR(str_c1, len, 10);
    Xor(str_c1, out, len, out);

    free(str_c1);
    free(str_c2);
}

void Ch(unsigned char *X, unsigned char *Y, unsigned char *Z, unsigned char len, unsigned char *out) {
    unsigned char *X_c = (unsigned char *)malloc(len * sizeof(unsigned char));
    unsigned char *sum1 = (unsigned char *)malloc(len * sizeof(unsigned char));
    unsigned char *sum2 = (unsigned char *)malloc(len * sizeof(unsigned char));

    //strcpy(X_c, X);
    for(unsigned char i = 0; i < len; ++i) {
        X_c[i] = X[i];
    }

    for(int i = 0; i < len; ++i) {
        sum1[i] = X[i] & Y[i];
        Complement(X_c, len);
        sum2[i] = X_c[i] & Z[i];
    }
    Add32(sum1, sum2, len, out);
    
    free(X_c);
    free(sum1);
    free(sum2);
}


void Maj(unsigned char *X, unsigned char *Y, unsigned char *Z, unsigned char len, unsigned char *out) {
    unsigned char *sum1 = (unsigned char *)malloc(len * sizeof(unsigned char));
    unsigned char *sum2 = (unsigned char *)malloc(len * sizeof(unsigned char));
    unsigned char *sum3 = (unsigned char *)malloc(len * sizeof(unsigned char));

    for(int i = 0; i < len; ++i) {
        sum1[i] = X[i] & Y[i];
        sum2[i] = X[i] & Z[i];
        sum3[i] = Y[i] & Z[i];
    }
    Add32(sum1, sum2, len, out);
    Add32(sum3, out, len, out);

    free(sum1);
    free(sum2);
    free(sum3);
}
