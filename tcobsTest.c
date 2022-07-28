
#include <stdint.h>
#include <string.h>
#include "tcobs.h"
#include "tcobsInternal.h"

//! dataSet contains only representative data for debugging.
//! The more intensive testing is done from Go using CGO.
static uint8_t dataSet[] = {
  //len, data...

      0, // decoded
      0, // encoded

      1, 0xFF,
      1, F0,

     13, 0, 255, 0, 0, 2, 2, 1, 255, 1, 255, 255, 255, 0,
     10, Z0, F0, Z1|1, 2, 2, 1, F0, 1,           F2|5, Z0,

      2, 0x00, 0xFF,
      2, 0x20, 0xFF,

     10, 2, 2, 2, 0, 1,  0, 0xFF, 0xFF, 1, 2,
      9, 2, 129, 32, 1, 33,         F1, 1, 2, 2,

      7, 0xFF, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      5, F0, 0x09, 0x02, F0, F0, 
/*
      7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x02, 0xFF,
      5, F0, F0, 0x02, F0, 0x02, 
*/
      8, 1, 0, 1, 1, 1, 1, 255, 255,
      5, 1, Z0|1, 1, R1|1, F1,

      2, 0xAA, 0xFF,
      3, 0xAA, 0xFF, 0x02,

      5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      2, 0xFF, 0xFF, 

      2, 0xFF, 0x00,
      2, 0xFF, 0x21,

      2, 0xAA, 0x00,
      2, 0xAA, 0x21,

      1, 0,
      1, Z0,
      
      2, 0, 0,
      1, Z1,
      
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
      
      3, 0xaa, 0xaa, 0xaa,
      2, 0xaa, R0|1,
      
      4, 0xaa, 0xaa, 0xaa, 0xaa,
      2, 0xaa, R1|1,
      
      5, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      2, 0xaa, R2|1,
      
      6, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
      3, 0xaa, R0|1, R0,

      6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,    
      3, 0xFF, 0xFF, 0x20,

      5, 0xFF, 0xFF, 0xaa, 0xaa, 0xaa,
      3, F1,         0xaa, R0|1,

      6, 0xFF, 0xFF, 0xaa, 0xaa, 0, 0,
      4, F1,         0xaa, 0xaa, Z1|2,

      5, 0xFF, 0xFF, 0xaa, 0, 0,
      3, F1,         0xaa, Z1|1,

      4, 0xFF, 0xFF, 0, 0,
      2, F1,           Z1,

      8, 0xFF, 0xFF, 0, 0, 0xFF, 0xFF, 0, 0,
      4, F1,           Z1,   F1,         Z1,

      1, F1,
      2, 0xFF, 0xFF,
      
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

};

#define OMAX 100 //!< max expected output buffer size
static uint8_t obuf[OMAX]; //!< output buffer

void TCOBSEncodeTest( void ){
    uint8_t *limit = dataSet + sizeof(dataSet);
    uint8_t *dec = dataSet, *enc;
    int dlen;
    int olen, elen;

    do{
        dlen = *dec++;
        olen = TCOBSEncode( obuf, dec, dlen);

        enc  = dec + dlen;
        elen = *enc++;
        if( olen != elen ){
            for(;;);
        }
        for( int i = 0; i < olen; i++ ){
            if( obuf[i] != enc[i] ){ 
                for(;;){} 
            }
        }
        dec  = enc + elen;  
    }while( dec + dlen < limit );
}

void TCOBSDecodeTest( void ){
    uint8_t *limit = dataSet + sizeof(dataSet);
    uint8_t *output, *enc, *dec = dataSet;
    int dlen = 0;
    int olen, elen;


    do{
        enc  = dec + dlen;
        elen = *enc++;
        dec  = enc + elen;
        dlen = *dec++;
        olen = TCOBSDecode( obuf, OMAX, enc, elen);
        output = obuf + OMAX - olen;
        if( olen != elen ){ 
            for(;;){} 
        } 
        for( int i = 0; i < olen; i++ ){
            if( output[i] != enc[i] ){ 
                for(;;){} 
            } 
        }
    }while( dec + dlen < limit );
}
