
#include <stdint.h>
#include <string.h>
#include "tcobs.h"
#include "tcobsInternal.h"

//! encodeDebugSet contains only representative data for debugging.
//! The more intensive testing is done from Go using CGO.
static uint8_t encodeDebugSet[] = {
  //len, data...
    
      5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      2, 0xFF, 0xFF, 

      2, 0xFF, 0x00,
      2, 0xFF, 0x21,

      1, 0xFF,
      1, 0xFF,
    
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

};


void TCOBSEncodeTest( void ){
    uint8_t* t = encodeDebugSet;
    uint8_t* limit = encodeDebugSet + sizeof(encodeDebugSet);
    unsigned ilen, olen;
    uint8_t* input;
    uint8_t output[40];

    while( t < limit ){
        ilen = *t++;
        input = t;
        memset( output, 0x55, sizeof(output) );
        olen = TCOBSEncode( output, input, ilen);
        t += ilen;
        if( olen != *t++ ){ 
            for(;;){} 
        } 
        for( int i = 0; i < olen; i++ ){
            if( output[i] != *t++ ){ 
                for(;;){} 
            } 
        }
    }
}
