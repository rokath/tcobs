/*! \file tcobs2Encode.c
\author Thomas.Hoehenleitner [at] seerose.net
\details See ./TCOBS2Specification.md.
*******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "tcobs.h"
#include "tcobs2Internal.h"

static void writeZeroCount( uint8_t ** out, int * count, int * distance );
static void writeFullCount( uint8_t ** out, int * count, int * distance );
static void writeRepeatCount( uint8_t ** out, uint8_t aa, int * count, int * distance );

static void writeNoopSigil( uint8_t ** out, int * distance);
static void writeLastSigil( uint8_t ** out, int * distance);

static unsigned ntoCCQNZ( int num, uint8_t* buf );
static unsigned ntoCCQNF( int num, uint8_t* buf );
static unsigned ntoCCQNR( int num, uint8_t* buf );

size_t TCOBSEncode( void * restrict output, const void * restrict input, size_t length){
    uint8_t const * i = input; // read pointer
    uint8_t const * limit = input + length; // read limit
    uint8_t b_1; // previous byte
    uint8_t b; // current byte

    uint8_t * out = output; //!< out is the output write pointer.
    uint8_t distance = 0; //<! distance is the byte count to the next sigil in sigil chain.
    int zeroCount = 0; //!< zeroCount is the number of accumulated 00-bytes.
    int fullCount = 0; //!< fullCount is the number of accumulated FF-bytes.
    int reptCount = 0; //!< reptCount is the number of accumulated equal bytes.

    // comment syntax:
    //     Sigil bytes chaining is done with offset and not shown explicitly.
    //     All left from comma is already written to o and if, only partially shown.
    //     n is 0..., representing count number.
    //     zn, fn, rn right from comma is count in variables, if not shown, then 0.
    //     At any moment only one of the 3 counters can be different from 0.
    //     Zn, Fn, Rn, Nn are (written) sigil bytes.
    //     Between comma and dot are the 2 values b_1 and b.
    //     3 dots ... means it is unknown if bytes follow. 
    //     !00 is a byte != 00.
    //     !FF is a byte != FF.
    //     aa is not 00 and not FF and all aa in a row are equal.
    //     xx yy and zz are any bytes.
    //     Invalid b_1 and b are displayed as --.

    if( length >= 2 ){ // , -- --. xx yy ... (at least 2 bytes)
        b = *i++; // , -- xx, yy ...
        for(;;){ // , zn|fn|rn -- xx. yy ... (possibly one of the 3 counters could be > 0 here, but only one)
            b_1 = b; // , xx --. yy ...
            b = *i++; // , xx yy. ...

            if( i < limit ){ // , zn|fn|rn xx yy. zz ... (at least one more byte)  
                if( b_1 == b ){ // , zn|fn|rn xx xx. zz ... 
                    if( b == 0 ){ // , zn 00 00. zz ... 
                        ASSERT( (fullCount|reptCount) == 0 );
                        zeroCount++; // , zn -- 00. zz ... 
                        continue; 
                    }
                    if( b == 0xFF ){
                        ASSERT( (zeroCount|reptCount) == 0 );
                        fullCount++; // , fn -- FF. zz ... 
                        continue; 
                    }
                    if( b == b_1 ){
                        ASSERT( (fullCount|zeroCount) == 0 );
                        reptCount++; // , rn -- AA. zz ... 
                        continue; 
                    }                    
                }else{ // , zn|fn|rn xx yy. zz ... (xx != yy)
                    if( b_1 == 0 ) { // , zn 00 yy. zz ...
                        ASSERT( (fullCount|reptCount) == 0 );
                        zeroCount++; // , zn -- yy. zz ...
                        writeZeroCount(&out, &zeroCount, &distance); // , -- yy. zz ...
                        continue;
                    }
                    if( b_1 == 0xFF ) { // , fn FF yy. zz ...
                        ASSERT( (zeroCount|reptCount) == 0 );
                        fullCount++; // , fn -- yy. zz ...
                        writeFullCount(&out, &fullCount, &distance); // , -- yy. zz ...
                        continue;
                    }
                    // , rn AA yy. zz ...
                    *out++ = b_1; // AA, rn -- yy. zz ...
                    if( distance == 31 ){
                        writeNoopSigil( &out, &distance);
                    }
                    distance++;
                    if( reptCount ){ // AA, rn -- yy. zz ... (n>=1)
                        ASSERT( (fullCount|zeroCount) == 0 );
                        writeRepeatCount(&out, b_1, &reptCount, &distance); // AA, -- yy. zz ...
                    }
                    continue;
                }
            }else{ // last 2 bytes
                // , zn|fn|rn xx yy.
                if( b_1 == b ){ // , zn|fn|rn xx xx. 
                    if( b == 0 ){ // , zn 00 00. 
                        ASSERT( (fullCount|reptCount) == 0 ); 
                        zeroCount += 2; // , zn -- --. 
                        writeZeroCount(&out, &zeroCount, &distance);
                        return out - (uint8_t*)output;
                    }
                    if( b == 0xFF ){ // , zn FF FF. 
                        ASSERT( (zeroCount|reptCount) == 0 );
                        fullCount += 2; // , fn -- --. 
                        writeFullCount(&out, &fullCount, &distance);
                        return out - (uint8_t*)output;
                    }
                    if( b == b_1 ){ // , zn AA AA.
                        ASSERT( (fullCount|zeroCount) == 0 );
                        reptCount +=2; // , rn -- --.
                        writeRepeatCount(&out, b, &reptCount, &distance);
                        return out - (uint8_t*)output;
                    }                    
                }else{ // , zn|fn|rn xx yy. (xx != yy)
                    if( b_1 == 0 ) { // , zn 00 yy.
                        ASSERT( (fullCount|reptCount) == 0 );
                        zeroCount++; // , zn -- yy.
                        writeZeroCount(&out, &zeroCount, &distance); // , -- yy.
                        goto lastByte;
                    }
                    if( b_1 == 0xFF ) { // , fn FF yy.
                        ASSERT( (zeroCount|reptCount) == 0 );
                        fullCount++; // , fn -- yy.
                        writeFullCount(&out, &fullCount, &distance); // , -- yy.
                        goto lastByte;
                    }
                    // , rn AA yy.
                    *out++ = b_1; // AA, rn -- yy.
                    if( distance == 31 ){
                        writeNoopSigil( &out, &distance);
                    }
                    distance++;
                    if( reptCount ){ // AA, rn -- yy. (n>=1)
                        ASSERT( (fullCount|zeroCount) == 0 );
                        writeRepeatCount(&out, b_1, &reptCount, &distance); // AA, -- yy.
                        goto lastByte;
                    }
                }
            }
        }
    }
    if( length == 0 ){ // , -- --.
        return 0;
    }
    if( length == 1 ){ // , . xx
        b = *i; // , -- xx.
        // (no need) goto lastByte;
    }
lastByte: // , -- xx.
    if( b == 0 ){ // , -- 00.
        zeroCount++; // , zn -- --. (n=1)
        writeZeroCount(&out, &zeroCount, &distance);
        // (no need) writeLastSigil(&out, &distance);
        return out - (uint8_t*)output;
    }
    if( b == 0xFF ){ // , -- FF.
        fullCount++; // , fn -- --. (n=1)
        writeFullCount(&out, &fullCount, &distance); // could be F0 F0 F0 F0 F0 F0 ...
        writeLastSigil(&out, &distance);             // therefore this is needed
        return out - (uint8_t*)output;
    }
    *out++ = b;
    if( distance == 31 ){
        writeNoopSigil( &out, &distance);
    }
    distance++;
    writeLastSigil(&out, &distance);
    return out - (uint8_t*)output;
}

static void writeNoopSigil( uint8_t ** out, int * distance){
    ASSERT( *distance <= 31 );
    **out++ = N | *distance;
    *distance = 0;          
}

static void writeLastSigil( uint8_t ** out, int * distance){
    if( *distance ){
        writeNoopSigil( out, distance);
    }
}

static void writeZeroCount( uint8_t ** out, int * num, int * distance ){
    uint8_t ciphers[16];
    int ciphersCount = ntoCCQNZ( num, &ciphers );
    for( int i = 0; i < ciphersCount; i++ ){
        switch( ciphers[i] ){
            case Z0:
                ASSERT( *distance <= 31 );
                **out++ = Z0 | *distance;
                *distance = 0;
                continue;          
            case Z1:
                ASSERT( *distance <= 31 );
                **out++ = Z1 | *distance;
                *distance = 0;
                continue;
            case Z2:
                if( *distance > 15 ){
                    writeNoopSigil( &out, &distance);
                }
                **out++ = Z2 | *distance;
                *distance = 0;
                continue;
            case Z3:
                if( *distance > 15 ){
                    writeNoopSigil( &out, &distance);
                }
                **out++ = Z3 | *distance;
                *distance = 0;
                continue;
            default: ASSERT( 0 );
        }
    }
    *num = 0;
}

static void writeFullCount( uint8_t ** out, int * num, int * distance ){
    uint8_t ciphers[16];
    int ciphersCount = ntoCCQNF( num, &ciphers );
    for( int i = 0; i < ciphersCount; i++ ){
        switch( ciphers[i] ){
            case F0:
                **out++ = F0;
                if( distance == 31 ){
                    writeNoopSigil( &out, &distance);
                }
                *distance++; // maybe not needed in sigil byte neighborhood?
                continue;
            case F1:
                ASSERT( *distance <= 31 );
                **out++ = F1 | *distance;
                *distance = 0;
                continue;
            case F2:
                if( *distance > 15 ){
                     writeNoopSigil( &out, &distance);
                }
                **out++ = F2 | *distance;
                *distance = 0;
                continue;
            case F3:
                if( *distance > 14 ){
                     writeNoopSigil( &out, &distance);
                }
                **out++ = F3 | *distance;
                *distance = 0;
                continue;
            default: ASSERT( 0 );
        }
    }
    *num = 0;
}

static void writeRepeatCount( uint8_t ** out, uint8_t aa, int * num, int * distance ){
    if( num == 1 ){
        **out++ = aa;
        if( distance == 31 ){
            writeNoopSigil( &out, &distance);
        }
        *distance++;
    }else{
        uint8_t ciphers[16];
        int ciphersCount = ntoCCQNR( num, &ciphers );
        for( int i = 0; i < ciphersCount; i++ ){
            switch( ciphers[i] ){
                case R0:
                    ASSERT( distance <= 31 );
                    **out++ = R0 | *distance;
                    *distance = 0;
                    continue;
                case R1:
                    if( *distance > 15 ){
                        writeNoopSigil( &out, &distance);
                    }
                    **out++ = R1 | *distance;
                    *distance = 0;
                    continue;
                case R2:
                    if( *distance > 14 ){
                        writeNoopSigil( &out, &distance);
                    }
                    **out++ = R1 | (*distance + 1);
                    *distance = 0;
                    continue;
                default: ASSERT( 0 );
            }
        }
    }
    *num = 0;
}


//! ntoCCQNZ converts num into a CCQN cipher sequence coded as Z sigils to buf and returns count of ciphers.
static unsigned ntoCCQNZ( int num, uint8_t* buf ){
    ASSERT( num > 0 )
    if( num <= 4 ){
        static uint8_t* ciphers[4] = {
            Z0, Z1, Z2, Z3 // 1, 2, 3, 4
        };
        *buf = ciphers[num-1];
        return 1;
    }
    if( num <= 20){
        static uint8_t* ciphers[16][2] = {
            {Z0, Z0}, {Z0, Z1}, {Z0, Z2}, {Z0, Z3}, //  5,  6,  7,  8,
            {Z1, Z0}, {Z1, Z1}, {Z1, Z2}, {Z1, Z3}, //  9, 10, 11, 12,
            {Z2, Z0}, {Z2, Z1}, {Z2, Z2}, {Z2, Z3}, // 13, 14, 15, 16,
            {Z3, Z0}, {Z3, Z1}, {Z3, Z2}, {Z3, Z3}  // 17, 18, 19, 20,
        };
        buf[0] = ciphers[num-5][0];
        buf[1] = ciphers[num-5][1];
        return 2;        
    }
    ASSERT( num < 21 ) // todo: generic solution
}

//! ntoCCQNF converts num into a CCQN cipher sequence coded as F sigils to buf and returns count of ciphers.
static unsigned ntoCCQNF( int num, uint8_t* buf ){
    ASSERT( num > 0 )
    if( num <= 4 ){
        static uint8_t* ciphers[4] = {
            F0, F1, F2, F3 // 1, 2, 3, 4
        };
        *buf = ciphers[num-1];
        return 1;
    }
    if( num <= 20 ){
        static uint8_t* ciphers[16][2] = {
            {F0, F0}, {F0, F1}, {F0, F2}, {F0, F3}, //  5,  6,  7,  8,
            {F1, F0}, {F1, F1}, {F1, F2}, {F1, F3}, //  9, 10, 11, 12,
            {F2, F0}, {F2, F1}, {F2, F2}, {F2, F3}, // 13, 14, 15, 16,
            {F3, F0}, {F3, F1}, {F3, F2}, {F3, F3}  // 17, 18, 19, 20,
        };
        buf[0] = ciphers[num-5][0];
        buf[1] = ciphers[num-5][1];
        return 2;        
    }
    ASSERT( num < 21 ) // todo: generic solution
}

//! ntoCCTNR converts num into a CCTN cipher sequence coded as R sigils to buf and returns count of ciphers.
static unsigned ntoCCTNR( int num, uint8_t* buf ){
    ASSERT( num > 1 )
    if( num <= 4 ){
        static uint8_t* ciphers[4] = {
            R0, R1, R2 // 2, 3, 4
        };
        *buf = ciphers[num-1];
        return 1;
    }
    if( num <= 13 ){
        static uint8_t* ciphers[16][2] = {
            {R0, R0}, {R0, R1}, {R0, R2}, //  5,  6,  7,
            {R1, R0}, {R1, R1}, {R1, R2}, //  8,  9, 10,
            {R2, R0}, {R2, R1}, {R2, R2}, // 11, 12, 13,
        };
        buf[0] = ciphers[num-5][0];
        buf[1] = ciphers[num-5][1];
        return 2;        
    }
    ASSERT( num < 14 ) // todo: generic solution
}


/////////////////////////////////////////////////////////////////////////////

//  #define MAX_CIPHERS 16
//  static uint8_t ciphers[MAX_CIPHERS];

//  //! firstQuaternaryCipher computes first quaternary cipher of *pNumber and reduces *pNumber accordingly.
//  uint8_t firstQuaternaryCipher( int * pNumber ){
//      int cipher = *pNumber;
//      int power = 0;
//      int part = 1;
//      while( cipher > 3  ){
//          cipher >>= 2; // /= 4
//          power++;
//          part <<= 2; // *= 4
//      }
//      *pNumber -= cipher * part; 
//      return cipher;
//  }
//  
//  //! ntoq converts num into a quaternary cipher sequence to buf and returns count of ciphers.
//  unsigned ntoq( int num, uint8_t* buf ){
//      // todo: more effective implementation
//      int result = itoa( num, buf, MAX_CIPHERS, 4 );
//      ASSERT( result == 0 )
//      return strlen(buf);
//  }
//  
//  //! ntot converts num into a ternary cipher sequence to buf and returns count of ciphers.
//  unsigned ntot( int num, uint8_t* buf ){
//      // todo: more effective implementation
//      int result = itoa( num, buf, MAX_CIPHERS, 3 );
//      ASSERT( result == 0 )
//      return strlen(buf);
//  }
