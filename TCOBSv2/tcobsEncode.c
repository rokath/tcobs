/*! \file tcobsEncode.c
\author Thomas.Hoehenleitner [at] seerose.net
\details See ./TCOBSSpecification.md.
*******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "tcobs.h"
#include "tcobsInternal.h"

static int writeZeroCount( uint8_t ** out, int * count, int * distance );
static int writeFullCount( uint8_t ** out, int * count, int * distance );
static int writeRepeatCount( uint8_t ** out, uint8_t aa, int * count, int * distance );

static int writeNoopSigil( uint8_t ** out, int * distance);
static int writeLastSigil( uint8_t ** out, int * distance);

static int CCQNtoCCQNZ( uint8_t* ciphers, int count );
static int CCQNtoCCQNF( uint8_t* ciphers, int count );
static int CCTNtoCCTNR( uint8_t* ciphers, int count );

static int ntoq( int num, uint8_t* buf );
static int ntot( int num, uint8_t* buf );

static int ntoCCQNZ( int num, uint8_t* buf );
static int ntoCCQNF( int num, uint8_t* buf );
static int ntoCCTNR( int num, uint8_t* buf );

int TCOBSEncode( void * restrict output, const void * restrict input, size_t length){
    uint8_t const * i = input; // read pointer
    uint8_t const * limit = (uint8_t*)input + length; // read limit
    uint8_t b_1; // previous byte
    uint8_t b; // current byte
    int err = 0;
    uint8_t * out = output; //!< out is the output write pointer.
    int distance = 0; //<! distance is the byte count to the next sigil in sigil chain.
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
            if( err ){
                return err;
            }
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
                    //( b == b_1 )
                    ASSERT( (fullCount|zeroCount) == 0 );
                    reptCount++; // , rn -- AA. zz ... 
                    continue; 
                }else{ // , zn|fn|rn xx yy. zz ... (xx != yy)
                    if( b_1 == 0 ) { // , zn 00 yy. zz ...
                        ASSERT( (fullCount|reptCount) == 0 );
                        zeroCount++; // , zn -- yy. zz ...
                        err = writeZeroCount(&out, &zeroCount, &distance); // , -- yy. zz ...
                        continue;
                    }
                    if( b_1 == 0xFF ) { // , fn FF yy. zz ...
                        ASSERT( (zeroCount|reptCount) == 0 );
                        fullCount++; // , fn -- yy. zz ...
                        err = writeFullCount(&out, &fullCount, &distance); // , -- yy. zz ...
                        continue;
                    }
                    // , rn AA yy. zz ...
                    if( distance == 31 ){
                        err = writeNoopSigil( &out, &distance);
                        if( err ){
                            return err;
                        }
                    }
                    *out++ = b_1; // AA, rn -- yy. zz ...
                    distance++;
                    if( reptCount ){ // AA, rn -- yy. zz ... (n>=1)
                        ASSERT( (fullCount|zeroCount) == 0 );
                        err = writeRepeatCount(&out, b_1, &reptCount, &distance); // AA, -- yy. zz ...
                    }
                    continue;
                }
            }else{ // last 2 bytes
                // , zn|fn|rn xx yy.
                if( b_1 == b ){ // , zn|fn|rn xx xx. 
                    if( b == 0 ){ // , zn 00 00. 
                        ASSERT( (fullCount|reptCount) == 0 ); 
                        zeroCount += 2; // , zn -- --. 
                        err = writeZeroCount(&out, &zeroCount, &distance);
                        if( err ){
                            return err;
                        }
                        return out - (uint8_t*)output;
                    }
                    if( b == 0xFF ){ // , zn FF FF. 
                        ASSERT( (zeroCount|reptCount) == 0 );
                        fullCount += 2; // , fn -- --. 
                        err = writeFullCount(&out, &fullCount, &distance);
                        if( err ){
                            return err;
                        }
                        return out - (uint8_t*)output;
                    }
                    // , zn AA AA.
                    ASSERT( (fullCount|zeroCount) == 0 );
                    reptCount +=1; // , rn -- AA.
                    if( distance == 31 ){
                        err = writeNoopSigil( &out, &distance);
                        if( err ){
                            return err;
                        }
                    }
                    *out++ = b; // AA, rn -- --.
                    distance++;
                    err = writeRepeatCount(&out, b, &reptCount, &distance);
                    if( err ){
                        return err;
                    }
                    if( distance > 0 ){
                        err = writeNoopSigil( &out, &distance);
                        if( err ){
                            return err;
                        }
                    }
                    return out - (uint8_t*)output;
                }else{ // , zn|fn|rn xx yy. (xx != yy)
                    if( b_1 == 0 ) { // , zn 00 yy.
                        ASSERT( (fullCount|reptCount) == 0 );
                        zeroCount++; // , zn -- yy.
                        err = writeZeroCount(&out, &zeroCount, &distance); // , -- yy.
                        goto lastByte;
                    }
                    if( b_1 == 0xFF ) { // , fn FF yy.
                        ASSERT( (zeroCount|reptCount) == 0 );
                        err = fullCount++; // , fn -- yy.
                        err = writeFullCount(&out, &fullCount, &distance); // , -- yy.
                        goto lastByte;
                    }
                    // , rn AA yy.
                    if( distance == 31 ){
                        err = writeNoopSigil( &out, &distance);
                        if( err ){
                            return err;
                        }
                    }
                    *out++ = b_1; // AA, rn -- yy.
                    distance++;
                    if( reptCount ){ // AA, rn -- yy. (n>=1)
                        ASSERT( (fullCount|zeroCount) == 0 );
                        err = writeRepeatCount(&out, b_1, &reptCount, &distance); // AA, -- yy.
                    }
                    goto lastByte;
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
    if( err ){
        return err;
    }
    if( b == 0 ){ // , -- 00.
        zeroCount++; // , zn -- --. (n=1)
        err = writeZeroCount(&out, &zeroCount, &distance);
        if( err ){
            return err;
        }
        // (no need) writeLastSigil(&out, &distance);
        return out - (uint8_t*)output;
    }
    if( b == 0xFF ){ // , -- FF.
        fullCount++; // , fn -- --. (n=1)
        err = writeFullCount(&out, &fullCount, &distance); // could be F0
        if( err ){
            return err;
        }
        if( distance > 1 ){ // a single F0 at the end needs a terminating N sigil
            err = writeLastSigil(&out, &distance); // if preceeded by a non sigil byte
            if( err ){
                return err;
            }
        }
        return out - (uint8_t*)output;
    }
    if( distance == 31 ){
        err = writeNoopSigil( &out, &distance);
        if( err ){
            return err;
        }
    }
    *out++ = b;
    distance++;
    err = writeLastSigil(&out, &distance);
    if( err ){
        return err;
    }
    return out - (uint8_t*)output;
}

//! writeNoopSigil inserts a N-sigil carrying the *distance and sets *distance to 0.
static int writeNoopSigil( uint8_t ** out, int * distance){
    ASSERT( 0 < *distance && *distance < 32 );
    **out = N | *distance;
    *out += 1;
    *distance = 0;
    return 0;
}

//! writeLastSigil terminates encoded buffer by appending a sigil byte, if *distance is not 0.
static int writeLastSigil( uint8_t ** out, int * distance){
    if( *distance ){
        int err = writeNoopSigil( out, distance);
        if( err < 0 ){
            return err;
        }
    }
    return 0;
}

//! writeZeroCount writes *num 00-bytes encoded as Z-sigil sequence including *distance value.
static int writeZeroCount( uint8_t ** out, int * num, int * distance ){
    uint8_t ciphers[16];
    int ciphersCount = ntoCCQNZ( *num, ciphers );
    int err = 0;
    if( ciphersCount < 0 ){
        return ciphersCount; // err
    }
    ASSERT( ciphersCount != 0 )
    for( int i = 0; i < ciphersCount; i++ ){
        if( err < 0 ){
            return err;
        }
        switch( ciphers[i] ){
            case Z0:
                ASSERT( *distance <= 31 );
                **out = Z0 | *distance;
                *out += 1;
                *distance = 0;
                continue;
            case Z1:
                ASSERT( *distance <= 31 );
                **out = Z1 | *distance;
                *out += 1;
                *distance = 0;
                continue;
            case Z2:
                if( *distance > 15 ){
                    err = writeNoopSigil( out, distance);
                }
                **out = Z2 | *distance;
                *out += 1;
                *distance = 0;
                continue;
            case Z3:
                if( *distance > 15 ){
                    err = writeNoopSigil( out, distance);
                }
                **out = Z3 | *distance;
                *out += 1;
                *distance = 0;
                continue;
            default: ASSERT( 0 );
        }
    }
    *num = 0;
    return 0;
}

//! writeFullCount writes *num FF-bytes encoded as F-sigil sequence including *distance value.
static int writeFullCount( uint8_t ** out, int * num, int * distance ){
    uint8_t ciphers[16];
    int ciphersCount = ntoCCQNF( *num, ciphers );
    int err = 0;
    if( ciphersCount < 0 ){
        return ciphersCount; // err
    }
    ASSERT( ciphersCount != 0 )
    for( int i = 0; i < ciphersCount; i++ ){
        if( err < 0 ){
            return err;
        }
        switch( ciphers[i] ){
            case F0:
                if( ciphersCount == 1 ){ // a single F0 (==FF) can be treated as orinary byte
                    if( *distance == 31 ){
                        err = writeNoopSigil( out, distance);
                    }
                    **out = F0;
                    *out += 1;
                    *distance += 1;
                }else if( ciphersCount > 1 ){
                    if( i == 0 ){ // a first F0 cannot carry a distance
                        if( *distance > 0 ){
                            err = writeNoopSigil( out, distance);
                        }
                    }
                    **out = F0;
                    *out += 1;
                }
                continue;
            case F1:
                ASSERT( *distance <= 31 );
                **out = F1 | *distance;
                *out += 1;
                *distance = 0;
                continue;
            case F2:
                if( *distance > 15 ){
                     err = writeNoopSigil( out, distance);
                }
                **out = F2 | *distance;
                *out += 1;
                *distance = 0;
                continue;
            case F3:
                if( *distance > 14 ){
                     err = writeNoopSigil( out, distance);
                }
                **out = F3 | *distance;
                *out += 1;
                *distance = 0;
                continue;
            default: ASSERT( 0 );
        }
    }
    *num = 0;
    return 0;
}

//! writeRepeatCount writes *num aa-bytes encoded as aa followed by an R-sigil sequence including *distance value.
static int writeRepeatCount( uint8_t ** out, uint8_t aa, int * num, int * distance ){
    int err = 0;
    if( *num == 1 ){
        if( *distance == 31 ){
            err = writeNoopSigil( out, distance);
            if( err ){
                return err;
            }
        }
        **out = aa;
        *out += 1;
        *distance += 1;
    }else{
        uint8_t ciphers[16];
        int ciphersCount = ntoCCTNR( *num, ciphers );

        if( ciphersCount < 0 ){
            return ciphersCount; // err
        }
        ASSERT( ciphersCount != 0 )
        for( int i = 0; i < ciphersCount; i++ ){
            if( err ){
                return err;
            }
            switch( ciphers[i] ){
                case R0:
                    ASSERT( *distance <= 31 );
                    **out = R0 | *distance;
                    *out += 1;
                    *distance = 0;
                    continue;
                case R1:
                    if( *distance > 15 ){
                        err = writeNoopSigil( out, distance);
                    }
                    **out = R1 | *distance;
                    *out += 1;
                    *distance = 0;
                    continue;
                case R2:
                    if( *distance > 15 ){
                        err = writeNoopSigil( out, distance);
                    }
                    **out = R2 | *distance ;
                    *out += 1;
                    *distance = 0;
                    continue;
                default: ASSERT( 0 );
            }
        }
    }
    *num = 0;
    return 0;
}

//! CCQNgenericStartValue computes CCQN generic start value of n and returns it.
//! It also fills ciphersCount with the appropriate number of ciphers.
//! n->ciphersCount: 0->0, 1->1, 4->1, 5->2
//! n->genericStart: 0->0, 1->1, 4->1, 5->5 (5=4^0+4^)
static int CCQNgenericStartValue( int num, int * ciphersCount ){
    *ciphersCount = 0;
    int power = 1; // 4^0  // 0->0, 1->1, 4->1, 5->2
    int genericStart = 0;
    while( genericStart + power <= num ){
        genericStart += power; // 4^0 + 4^1 + 4^2 + ...
        power *= 4;
        *ciphersCount += 1;
    }
    return genericStart;
}

//! CCTNgenericStartValue computes CCQN generic start value of n and returns it.
//! It also fills ciphersCount with the appropriate number of ciphers.
//! n->ciphersCount: 0->0, 1->1, 4->1, 5->2
//! n->genericStart: 0->0, 1->1, 4->1, 5->5 (5=4^0+4^)
static int CCTNgenericStartValue( int num, int * ciphersCount ){
    *ciphersCount = 0;
    int power = 1; // 3^0  // 0->0, 1->1, 3->1, 4->2
    int genericStart = 1;
    while( genericStart + power <= num ){
        genericStart += power; // 1 + 3^0 + 3^1 + 3^2 + ...
        power *= 3;
        *ciphersCount += 1;
    }
    return genericStart;
}

//! ntoCCQNZ converts num into a CCQN cipher sequence coded as Z sigils to ciphers and returns count of ciphers.
static int ntoCCQNZ( int num, uint8_t* ciphers ){
    ASSERT( num > 0 ){
			int ciphersCount = 0;
			int gsv = CCQNgenericStartValue( num, &ciphersCount );
			int n = num - gsv;
			int qcount = ntoq( n, ciphers );
			memcpy(ciphers + ciphersCount - qcount, ciphers, qcount);
			memset(ciphers, 0, ciphersCount - qcount );
			return CCQNtoCCQNZ( ciphers, ciphersCount );
		}
}

//! ntoCCQNF converts num into a CCQN cipher sequence coded as F sigils to ciphers and returns count of ciphers.
static int ntoCCQNF( int num, uint8_t* ciphers ){
    ASSERT( num > 0 ){
			int ciphersCount = 0;
			int gsv = CCQNgenericStartValue( num, &ciphersCount );
			int n = num - gsv;
			int qcount = ntoq( n, ciphers );
			memcpy(ciphers + ciphersCount - qcount, ciphers, qcount);
			memset(ciphers, 0, ciphersCount - qcount );
			return CCQNtoCCQNF( ciphers, ciphersCount );
		}
}

//! ntoCCQNR converts num into a CCQN cipher sequence coded as F sigils to ciphers and returns count of ciphers.
static int ntoCCTNR( int num, uint8_t* ciphers ){
    ASSERT( num > 0 ){
			int ciphersCount = 0;
			int gsv = CCTNgenericStartValue( num, &ciphersCount );
			int n = num - gsv;
			int tcount = ntot( n, ciphers );
			uint8_t* dest = ciphers + ciphersCount - tcount;
			memmove(dest, ciphers, tcount);
			memset(ciphers, 0, ciphersCount - tcount );
			return CCTNtoCCTNR( ciphers, ciphersCount );
		}
}

//! CCQNtoCCQNZ converts ciphers in ciphers to Z-sigils.
static int CCQNtoCCQNZ( uint8_t* ciphers, int count ){
    for(int i = 0; i < count; i++ ){
        if( ciphers[i] == 0 ){
            ciphers[i] = Z0;
        }else if( ciphers[i] == 1 ){
            ciphers[i] = Z1;
        }else if( ciphers[i] == 2 ){
            ciphers[i] = Z2;
        }else if( ciphers[i] == 3 ){
            ciphers[i] = Z3;
        }else{
            return - __LINE__;
        }
    }
    return count;
}

//! CCQNtoCCQNF converts ciphers in ciphers to F-sigils.
static int CCQNtoCCQNF( uint8_t* ciphers, int count ){
    for(int i = 0; i < count; i++ ){
        if( ciphers[i] == 0 ){
            ciphers[i] = F0;
        }else if( ciphers[i] == 1 ){
            ciphers[i] = F1;
        }else if( ciphers[i] == 2 ){
            ciphers[i] = F2;
        }else if( ciphers[i] == 3 ){
            ciphers[i] = F3;
        }else{
            return - __LINE__;
        }
    }
    return count;
}

//! CCTNtoCCTNR converts ciphers in ciphers to Z-sigils.
static int CCTNtoCCTNR( uint8_t* ciphers, int count ){
    for(int i = 0; i < count; i++ ){
        if( ciphers[i] == 0 ){
            ciphers[i] = R0;
        }else if( ciphers[i] == 1 ){
            ciphers[i] = R1;
        }else if( ciphers[i] == 2 ){
            ciphers[i] = R2;
        }else{
            return - __LINE__;
        }
    }
    return count;
}

//! Function to swap two numbers
static void swap(char *x, char *y) {
    char t = *x; *x = *y; *y = t;
}
 
// Function to reverse `buffer[i…j]`
static char* reverse(char *buffer, int i, int j){
    while (i < j) {
        swap(&buffer[i++], &buffer[j--]);
    }
    return buffer;
}

//! ntoq converts n into a quaternary cipher sequence to buf and returns count of ciphers.
static int ntoq( int n, uint8_t* buffer ){
    int i = 0;
    while (n){
        int r = n & 3;  // n % 4;
        n     = n >> 2; // n / 4;
        buffer[i++] = r;
    }
    //  // if the number is 0
    //  if (i == 0) {
    //      buffer[i++] = 0;
    //  }
		reverse((char*)buffer, 0, i-1);
		return i;
}

//! ntot converts n into a ternary cipher sequence to buf and returns count of ciphers.
static int ntot(int n, uint8_t* buffer ){
    int i = 0;
    while (n){
        int r = n % 3;
        n     = n / 3;
        buffer[i++] = r;
    }
    //  // if the number is 0
    //  if (i == 0) {
    //      buffer[i++] = 0;
    //  }
    reverse((char*)buffer, 0, i - 1);
		return i;
}

/*
//! ntoCCQNZ converts num into a CCQN cipher sequence coded as Z sigils to buf and returns count of ciphers.
static int ntoCCQNZ( int num, uint8_t* buf ){
    ASSERT( num > 0 )
    if( num <= 4 ){
        static uint8_t ciphers[4] = {
            Z0, Z1, Z2, Z3 // 1, 2, 3, 4
        };
        *buf = ciphers[num-1];
        return 1;
    }
    if( num <= 20){
        static uint8_t ciphers[16][2] = {
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
    return 0;
}

//! ntoCCQNF converts num into a CCQN cipher sequence coded as F sigils to buf and returns count of ciphers.
static int ntoCCQNF( int num, uint8_t* buf ){
    ASSERT( num > 0 )
    if( num <= 4 ){
        static uint8_t ciphers[4] = {
            F0, F1, F2, F3 // 1, 2, 3, 4
        };
        *buf = ciphers[num-1];
        return 1;
    }
    if( num <= 20 ){
        static uint8_t ciphers[16][2] = {
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
    return 0;
}

//! ntoCCTNR converts num into a CCTN cipher sequence coded as R sigils to buf and returns count of ciphers.
static int ntoCCTNR( int num, uint8_t* buf ){
    ASSERT( num > 1 )
    if( num <= 4 ){
        static uint8_t ciphers[4] = {
            R0, R1, R2 // 2, 3, 4
        };
        *buf = ciphers[num-2];
        return 1;
    }
    if( num <= 13 ){
        static uint8_t ciphers[16][2] = {
            {R0, R0}, {R0, R1}, {R0, R2}, //  5,  6,  7,
            {R1, R0}, {R1, R1}, {R1, R2}, //  8,  9, 10,
            {R2, R0}, {R2, R1}, {R2, R2}, // 11, 12, 13,
        };
        buf[0] = ciphers[num-5][0];
        buf[1] = ciphers[num-5][1];
        return 2;        
    }
    if( num <= 40 ){
        static uint8_t ciphers[27][3] = {
            {R0, R0, R0}, {R0, R0, R1}, {R0, R0, R2}, //  14 +   0,  1,  2
            {R0, R1, R0}, {R0, R1, R1}, {R0, R1, R2}, //  14 +   3,  4,  5
            {R0, R2, R0}, {R0, R2, R1}, {R0, R2, R2}, //  14 +   6,  7,  8

            {R1, R0, R0}, {R1, R0, R1}, {R1, R0, R2}, //  14 +   9, 10, 11
            {R1, R1, R0}, {R1, R1, R1}, {R1, R1, R2}, //  14 +  12, 13, 14
            {R1, R2, R0}, {R1, R2, R1}, {R1, R2, R2}, //  14 +  15, 16, 17

            {R2, R0, R0}, {R2, R0, R1}, {R2, R0, R2}, //  14 +  18, 19, 20
            {R2, R1, R0}, {R2, R1, R1}, {R2, R1, R2}, //  14 +  21, 22, 23
            {R2, R2, R0}, {R2, R2, R1}, {R2, R2, R2}, //  14 +  24, 25, 26
        };
        buf[0] = ciphers[num-14][0];
        buf[1] = ciphers[num-14][1];
        buf[2] = ciphers[num-14][2];
        return 3;        
    }
    ASSERT( num < 41 ) // todo: generic solution
    return 0;
}
*/
