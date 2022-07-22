/*! \file tcobs2.c
\author Thomas.Hoehenleitner [at] seerose.net
\details See ./TCOBS2Specification.md.
*******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "tcobs.h"

#define R2 0x00 //!< R2 is the sigil byte 0x0000oooo, a CCTN repeatCount cipher 2 with offsets 0-14 coded as 1-15.
#define Z3 0x10 //!< Z3 is the sigil byte 0x0001oooo, a CCQN zeroCount cipher 3 with offsets 0-15.
#define Z0 0x20 //!< Z0 is the sigil byte 0x001ooooo, a CCQN zeroCount cipher 0 with offsets 0-31.
#define R1 0x40 //!< R1 is the sigil byte 0x0100oooo, a CCTN repeatCount cipher 1 with offsets 0-15.
#define Z2 0x50 //!< Z2 is the sigil byte 0x0101oooo, a CCQN zeroCount cipher 2 with offsets 0-15.
#define Z1 0x60 //!< Z1 is the sigil byte 0x011ooooo, a CCQN zeroCount cipher 1 with offsets 0-31.
#define R0 0x80 //!< R0 is the sigil byte 0x100ooooo, a CCTN repeatCount cipher 0 with offsets 0-31.
#define N  0xA0 //!< N  is the sigil byte 0x101ooooo, without operation with offsets 0-31 to keep the sigil chain linked.
#define F1 0xC0 //!< F1 is the sigil byte 0x110ooooo, a CCQN fullCount cipher 1 with offsets 0-31.
#define F2 0xE0 //!< F2 is the sigil byte 0x1110oooo, a CCQN fullCount cipher 2 with offsets 0-15.
#define F3 0xF0 //!< F3 is the sigil byte 0x1111oooo, a CCQN fullCount cipher 3 with offsets 0-14.
#define F0 0xFF //!< F0 is the sigil byte 0x11111111, a CCQN fullCount cipher 0 with offset 0.

//! ASSERT checks for a true condition, otherwise stop.
#define ASSERT( condition ) do{ if( !(condition) ){ for(;;){} } }while(0);

static uint8_t * o; //!< o is the output write pointer.
static uint8_t distance = 0; //<! distance is the byte count to the next sigil in sigil chain.
static uint32_t zeroCount; //!< zeroCount is the number of accumulated 00-bytes.
static uint32_t fullCount; //!< fullCount is the number of accumulated FF-bytes.
static uint32_t repeatCount; //!< repeatCount is the number of accumulated equal bytes.
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

//! ntoCCQNZ converts num into a CCQN cipher sequence coded as Z sigils to buf and returns count of ciphers.
unsigned ntoCCQNZ( int num, uint8_t* buf ){
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
            { Z0, Z0 }, { Z0, Z1 }, { Z0, Z2}, { Z0, Z3 }, //  5,  6,  7,  8,
            { Z1, Z0 }, { Z1, Z1 }, { Z1, Z2}, { Z1, Z3 }, //  9, 10, 11, 12,
            { Z2, Z0 }, { Z2, Z1 }, { Z2, Z2}, { Z2, Z3 }, // 13, 14, 15, 16,
            { Z3, Z0 }, { Z3, Z1 }, { Z3, Z2}, { Z3, Z3 }  // 17, 18, 19, 20,
        };
        buf[0] = ciphers[num-5][0];
        buf[1] = ciphers[num-5][1];
        return 2;        
    }
    ASSERT( num < 21 ) // todo: generic solution
}

//! ntoCCQNF converts num into a CCQN cipher sequence coded as F sigils to buf and returns count of ciphers.
unsigned ntoCCQNF( int num, uint8_t* buf ){
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
            { F0, F0 }, { F0, F1 }, { F0, F2}, { F0, F3 }, //  5,  6,  7,  8,
            { F1, F0 }, { F1, F1 }, { F1, F2}, { F1, F3 }, //  9, 10, 11, 12,
            { F2, F0 }, { F2, F1 }, { F2, F2}, { F2, F3 }, // 13, 14, 15, 16,
            { F3, F0 }, { F3, F1 }, { F3, F2}, { F3, F3 }  // 17, 18, 19, 20,
        };
        buf[0] = ciphers[num-5][0];
        buf[1] = ciphers[num-5][1];
        return 2;        
    }
    ASSERT( num < 21 ) // todo: generic solution
}


//! ntoCCTNR converts num into a CCTN cipher sequence coded as R sigils to buf and returns count of ciphers.
unsigned ntoCCTNR( int num, uint8_t* buf ){
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
            { R0, R0 }, { R0, R1 }, { R0, R2}, //  5,  6,  7,
            { R1, R0 }, { R1, R1 }, { R1, R2}, //  8,  9, 10,
            { R2, R0 }, { R2, R1 }, { R2, R2}, // 11, 12, 13,
        };
        buf[0] = ciphers[num-5][0];
        buf[1] = ciphers[num-5][1];
        return 2;        
    }
    ASSERT( num < 14 ) // todo: generic solution
}


static void writeZeroCount( void ){
    

    zeroCount = 0;
}

static void writeFullCount( void ){

    fullCount = 0;
}

static void writeRepeatCount( uint8_t aa ){

    repeatCount = 0;
}


size_t TCOBSEncode( void * restrict output, const void * restrict input, size_t length){
    uint8_t const * i = input; // read pointer
    uint8_t const * limit = input + length; // read limit
    uint8_t b_1; // previous byte
    uint8_t b; // current byte
    zeroCount = 0;
    fullCount = 0;
    repeatCount = 0;
    o = output;

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
                        repeatCount++; // , rn -- AA. zz ... 
                        continue; 
                    }                    
                }else{ // , zn|fn|rn xx yy. zz ... (xx != yy)
                    if( b_1 == 0 ) { // , zn 00 yy. zz ...
                        ASSERT( (fullCount|reptCount) == 0 );
                        zeroCount++; // , zn -- yy. zz ...
                        writeZeroCount(); // , -- yy. zz ...
                        continue;
                    }
                    if( b_1 == 0xFF ) { // , fn FF yy. zz ...
                        ASSERT( (zeroCount|reptCount) == 0 );
                        fullCount++; // , fn -- yy. zz ...
                        writeFullCount(); // , -- yy. zz ...
                        continue;
                    }
                    // , rn AA yy. zz ...
                    *o++ = b_1; // AA, rn -- yy. zz ...
                    distance++;
                    if( repeatCount ){ // AA, rn -- yy. zz ... (n>=1)
                        ASSERT( (fullCount|zeroCount) == 0 );
                        writeRepeatCount(b_1); // AA, -- yy. zz ...
                    }
                    if( distance == 31 ){
                        writeNopSigil();
                    }
                    continue;
                }
            }else{ // last 2 bytes
                // , zn|fn|rn xx yy.
                if( b_1 == b ){ // , zn|fn|rn xx xx. 
                    if( b == 0 ){ // , zn 00 00. 
                        ASSERT( (fullCount|reptCount) == 0 ); 
                        zeroCount += 2; // , zn -- --. 
                        writeZeroCount();
                        return o - output;
                    }
                    if( b == 0xFF ){ // , zn FF FF. 
                        ASSERT( (zeroCount|reptCount) == 0 );
                        fullCount += 2; // , fn -- --. 
                        writeFullCount();
                        return o - output;
                    }
                    if( b == b_1 ){ // , zn AA AA.
                        ASSERT( (fullCount|zeroCount) == 0 );
                        repeatCount +=2; // , rn -- --.
                        writeRepeatCount(b);
                        return o - output;
                    }                    
                }else{ // , zn|fn|rn xx yy. (xx != yy)
                    if( b_1 == 0 ) { // , zn 00 yy.
                        ASSERT( (fullCount|reptCount) == 0 );
                        zeroCount++; // , zn -- yy.
                        writeZeroCount(); // , -- yy.
                        goto lastByte;
                    }
                    if( b_1 == 0xFF ) { // , fn FF yy.
                        ASSERT( (zeroCount|reptCount) == 0 );
                        fullCount++; // , fn -- yy.
                        writeFullCount(); // , -- yy.
                        goto lastByte;
                    }
                    // , rn AA yy.
                    *o++ = b_1; // AA, rn -- yy.
                    distance++;
                    if( repeatCount ){ // AA, rn -- yy. (n>=1)
                        ASSERT( (fullCount|zeroCount) == 0 );
                        writeRepeatCount(b_1); // AA, -- yy.
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
        //goto lastByte;
    }
lastByte: // , -- xx.
    if( b == 0 ){ // , -- 00.
        zeroCount++; // , zn -- --. (n=1)
        writeZeroCount();
        writeLastSigil();
        return o - output;
    }
    if( b == 0xFF ){ // , -- FF.
        fullCount++; // , fn -- --. (n=1)
        writeFullCount();
        writeLastSigil();
        return o - output;
    }
    *o++ = b;
    distance++;
    writeLastSigil();
    return o - output;
}

/////////////////////////////////////////////////////////////////////////////


size_t TCOBSDecode( void * restrict output, size_t max, const void * restrict input ){

}