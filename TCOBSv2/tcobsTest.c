
#include <stdint.h>
#include <string.h>
#include "tcobs.h"
#include "tcobsInternal.h"

//! dataSet contains only representative data for debugging.
//! The more intensive testing is done from Go using CGO.
static uint8_t dataSet[] = {
  //len, data...

// 00
      0, // decoded
      0, // encoded
// 01
      1, 0xFF,
      1, F0,

     13, 0, 255, 0, 0, 2, 2, 1, 255, 1, 255, 255, 255, 0,
     10, Z0, F0, Z1|1, 2, 2, 1, F0, 1,           F2|5, Z0,

      2, 0x00, 0xFF,
      2, 0x20, 0xFF,

     10, 2, 2, 2, 0, 1,  0, 0xFF, 0xFF, 1, 2,
      9, 2, 129, 32, 1, 33,         F1, 1, 2, 2,

      8, 1, 0, 1, 1, 1, 1, 255, 255,
      5, 1, Z0|1, 1, R1|1, F1,
// 05
      2, 0xFF, 0xFF,
      1, F1,

      2, 0xFF, 0x00,
      2, 0xFF, 0x21,

      2, 0xAA, 0x00,
      2, 0xAA, 0x21,

      1, 0,
      1, Z0,

      2, 0, 0,
      1, Z1,
// 0A
      3, 0, 0, 0,
      1, Z2,
      
      4, 0, 0, 0, 0,
      1, Z3,
      
      5, 0, 0, 0, 0, 0,
      2, Z0, Z0,
      
      1, 0xaa,
      2, 0xaa, N|1,
      
      2, 0xaa, 0xaa,
      3, 0xaa, 0xaa, N|2,
// 0F
      3, 0xaa, 0xaa, 0xaa,
      2, 0xaa, R0|1,
      
      2, 0xAA, 0xFF,
      3, 0xAA, 0xFF, 0x02,
      
      5, 0xFF, 0xFF, 0xaa, 0xaa, 0xaa,
      3, F1,         0xaa, R0|1,

      6, 0xFF, 0xFF, 0xaa, 0xaa, 0, 0,
      4, F1,         0xaa, 0xaa, Z1|2,

      5, 0xFF, 0xFF, 0xaa, 0, 0,
      3, F1,         0xaa, Z1|1,
// 20
      4, 0xFF, 0xFF, 0, 0,
      2, F1,           Z1,

      8, 0xFF, 0xFF, 0, 0, 0xFF, 0xFF, 0, 0,
      4, F1,           Z1,   F1,         Z1,

      3, 0xFF, 0xFF, 0xFF,
      1, F2,
      
      4, 0xFF, 0xFF, 0xFF, 0xFF,
      1, F3,

     16,  0xFF, 0xFF, 0xFF, 0xFF, 0x22, 0x33, 0xFF, 0xFF, 0x55, 0x66, 0x77, 0x88, 0x99, 0xFF, 0xFF, 0xFF,
     10,                      F3, 0x22, 0x33,       F1|2, 0x55, 0x66, 0x77, 0x88, 0x99,             F2|5,

     16,  0, 0, 0, 0, 0x22, 0x33, 0, 0, 0x55, 0x66, 0x77, 0x88, 0x99, 0, 0, 0,
     10,          Z3, 0x22, 0x33, Z1|2, 0x55, 0x66, 0x77, 0x88, 0x99,    Z2|5,

     10, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0,
     10, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, Z0|9,
      
     10,  0, 0x22, 0x33,    0, 0x55, 0x66, 0x77, 0x88, 0x99, 0,
     10, Z0, 0x22, 0x33, Z0|2, 0x55, 0x66, 0x77, 0x88, 0x99, Z0|5,
// 1D
      7, 0xFF, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      5, F0, 0x09, 0x02, F0, F0, 
// 1E
      7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x02, 0xFF,
      5, F0, F0, 0x02, F0, 0x02, 
// 1F
      5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      2, 0xFF, 0xFF, 

      6, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      3, 0xaa, R0|1, R0,
// 21
      6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,    
      3, 0xFF, 0xFF, 0x20,

      5, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      2, 0xaa, R2|1,

      4, 0xaa, 0xaa, 0xaa, 0xaa,
      2, 0xaa, R1|1,

      1, 15,
     //   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,N|31
      2, 15, N|1,

     30, 15,199,187,129,134, 57,172, 72,164,198,175,162,241, 88, 26,139,149, 37,226, 15,218,104,146,127, 43, 47,248, 54,247, 53,
     //   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
     31, 15,199,187,129,134, 57,172, 72,164,198,175,162,241, 88, 26,139,149, 37,226, 15,218,104,146,127, 43, 47,248, 54,247, 53, N|30,

     31, 15,199,187,129,134, 57,172, 72,164,198,175,162,241, 88, 26,139,149, 37,226, 15,218,104,146,127, 43, 47,248, 54,247, 53,120,
     //   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
     32, 15,199,187,129,134, 57,172, 72,164,198,175,162,241, 88, 26,139,149, 37,226, 15,218,104,146,127, 43, 47,248, 54,247, 53,120, N|31,

     32, 15,199,187,129,134, 57,172, 72,164,198,175,162,241, 88, 26,139,149, 37,226, 15,218,104,146,127, 43, 47,248, 54,247, 53,120,219,
     //   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,   0,
     34, 15,199,187,129,134, 57,172, 72,164,198,175,162,241, 88, 26,139,149, 37,226, 15,218,104,146,127, 43, 47,248, 54,247, 53,120,N|31,219,N|1
/*
      //0 dat: 131 [15 199 187 129 134 57 172 72 164 198 175 162 241 88 26 139 149 37 226 15 218 104 146 127 43 47 248 54 247 53 120 219 15 165 76 41 247 253 146 141 146 202 67 241 147 222 228 127 89 21 73 245 151 168 17 200 250 103 171 3 30 189 156 106 164 233 130 159 34 75 232 234 246 103 38 201 7 124 180 31 121 1 157 137 43 233 147 3 178 190 88 130 243 36 7 88 163 141 126 65 39 219 253 71 122 50 245 254 112 138 41 191 6 40 1 195 249 87 118 62 234 13 175 98 214 93 206 91 165 36 247]
      //0 enc: 136 [15 199 187 129 134 57 172 72 164 198 175 162 241 88 26 139 149 37 226 15 218 104 146 127 43 47 248 54 247 53 120 219 31 15 165 76 41 247 253 146 141 146 202 67 241 147 222 228 127 89 21 73 245 151 168 17 200 250 103 171 3 30 189 156 31 106 164 233 130 159 34 75 232 234 246 103 38 201 7 124 180 31 121 1 157 137 43 233 147 3 178 190 88 130 243 36 31 7 88 163 141 126 65 39 219 253 71 122 50 245 254 112 138 41 191 6 40 1 195 249 87 118 62 234 13 175 98 214 31 93 206 91 165 36 247 7]

      131, 15,199,187,129,134, 57,172, 72,164,198,175,162,241, 88, 26,139,149, 37,226, 15,218,104,146,127, 43, 47,248, 54,247, 53,120,219,     15,165, 76, 41,247,253,146,141,146,202, 67,241,147,222,228,127, 89, 21, 73,245,151,168, 17,200,250,103,171,  3, 30,189,156,        106,164,233,130,159, 34, 75,232,234,246,103, 38,201,  7,124,180, 31,121,  1,157,137, 43,233,147,  3,178,190, 88,130,243, 36,      7, 88,163,141,126, 65, 39,219,253, 71,122, 50,245,254,112,138, 41,191,  6, 40,  1,195,249, 87,118, 62,234, 13,175, 98,214,     93,206, 91,    165, 36,247,
      //    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,N|31  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,N|31  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,*29, 30, 31,N|31  0,  1,  2,  // encode error: at pos 29 dc is 31!
      136, 15,199,187,129,134, 57,172, 72,164,198,175,162,241, 88, 26,139,149, 37,226, 15,218,104,146,127, 43, 47,248, 54,247, 53,120,219, 31, 15,165, 76, 41,247,253,146,141,146,202, 67,241,147,222,228,127, 89, 21, 73,245,151,168, 17,200,250,103,171,  3, 30,189,156, 31,    106,164,233,130,159, 34, 75,232,234,246,103, 38,201,  7,124,180, 31,121,  1,157,137, 43,233,147,  3,178,190, 88,130,243, 36, 31,  7, 88,163,141,126, 65, 39,219,253, 71,122, 50,245,254,112,138, 41,191,  6, 40,  1,195,249, 87,118, 62,234, 13,175, 98,214, 31, 93,206, 91,165, 36,247,  7,
      //           BB,  5,  4,  3,  2,  1,  0, C6, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 2F, 18, 17, 16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 93,  7,  6,  5,  4,  3,  2,  1,  0, A8,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 6A,  1,  0, 82,  6,  5,  4,  3,  2,  1,  0, 26, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, DB, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, D6,  6,  5,  4,  3,  2,  1,  0, 07, 
      //        Z3|11,                       F1|6,                                                          Z0|15,                                                                          R0|19,                               R2|8,                                      Z1|10,       R0|2,                           Z0|7,                                                                                                          F1|27,                                                                                      F1|22,                            N|7,
*/




};

#define OMAX 4000 //!< max expected output buffer size
static uint8_t obuf[OMAX]; //!< output buffer
int k;

void TCOBSEncodeTest( void ){
    uint8_t *limit = dataSet + sizeof(dataSet);
    uint8_t *enc, *dec = dataSet;
    int olen, dlen, elen;
    k = 0;
    do{
        dlen = *dec++;
        enc  = dec + dlen;
        elen = *enc++;
        olen = TCOBSEncode( obuf, dec, dlen );
        
        if( olen != elen ){
            for(;;);
        }
        for( int i = 0; i < olen; i++ ){
            if( obuf[i] != enc[i] ){ 
                for(;;){} 
            }
        }
        dec  = enc + elen;
        k++;
    }while( dec < limit );
}

void TCOBSDecodeTest( void ){
    uint8_t *limit = dataSet + sizeof(dataSet);
    uint8_t *enc, *dec = dataSet;
    int olen, dlen, elen;
    uint8_t *out;
    k = 0;
    do{
        dlen = *dec++;
        enc  = dec + dlen;
        elen = *enc++;
        olen = TCOBSDecode( obuf, OMAX, enc, elen );
        
        if( olen != dlen ){ 
            for(;;){} 
        }
        out = obuf + OMAX - olen;
        for( int i = 0; i < olen; i++ ){
            if( out[i] != dec[i] ){ 
                for(;;){} 
            } 
        }
        dec  = enc + elen;
        k++;
    }while( dec < limit );
}