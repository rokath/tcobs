/*! \file tcobs2Decode.c
\author Thomas.Hoehenleitner [at] seerose.net
\details See ./TCOBS2Specification.md.
*******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "tcobs.h"
#include "tcobsInternal.h"

//! CHECK_OUTPUT_SPACE checks for n plus distance output space.
#define CHECK_OUTPUT_SPACE(n) // if( (uint8_t*)output - o < n + dc ){ return OUT_BUFFER_TOO_SMALL; }

//! CHECK_INPUT_SPACE checks for distance input space.
#define CHECK_INPUT_SPACE  if( (uint8_t*)input > i - dc ){ return INPUT_DATA_CORRUPTED; }

//! CHECK_SPACE checks for sufficient input and output space.
#define CHECK_SPACE( sigilDecodedCount) CHECK_INPUT_SPACE CHECK_OUTPUT_SPACE(sigilDecodedCount)

//! COPY_BYTES transfers distance bytes backwards from the input buffer end to the output buffer end.
#define COPY_BYTES do{ CHECK_SPACE(0) while( dc--){ *--o = *--i; } dc = 0; } while( 0 );

#define MAX_CIPHERS 24 //!< MAX_CIPHERS is max expected sigil bytes of one kind in a row.

static void writeZn( uint8_t ** out, uint8_t ** ss, int * ciphersCount);
static void writeFn( uint8_t ** out, uint8_t ** ss, int * ciphersCount);
static void writeRn( uint8_t ** out, uint8_t ** ss, int * ciphersCount, uint8_t repeatByte);

int TCOBSDecode( void * restrict output, size_t max, const void * restrict input, size_t length ){
    if( length == 0 ){
        return 0;
    }else{
        uint8_t sigil;
        int dc; // distance;
        uint8_t cc[MAX_CIPHERS];
        uint8_t * ss = cc + MAX_CIPHERS; // sigilSequence (without distance bits) = counted ciphers
        uint8_t * o = (uint8_t*)output + max; // output write pointer behind next value
        uint8_t const * i = (uint8_t*)input + length; // input read pointer behind next value
        int zc = 0; // Z sigils in a row
        int fc = 0; // F sigils in a row
        int rc = 0; // R Z sigils in a row
        uint8_t repeatByte;
        for(;;){
            if( i == input ){ // done
                ASSERT( zc|fc|rc|dc == 0 )
                return (uint8_t*)output + max - o;
            }
            ASSERT(zc<MAX_CIPHERS)
            ASSERT(fc<MAX_CIPHERS)
            ASSERT(rc<MAX_CIPHERS)
            sigil = *--i;
            if( sigil >> 7 ==  0 ){               // 0xxxxxxx    // 00 | R2 | Z3 | Z0 | R1 | Z2 | Z1
                if( sigil >> 6 ==  0 ){           // 00xxxxxx    // 00 | R2 | Z3 | Z0
                    if( sigil >> 5 ==  0 ){       // 000xxxxx    // 00 | R2 | Z3 
                        if( sigil >> 4 ==  0 ){   // 0000xxxx    // 00 | R2
                            if( sigil == 0 ){     // 00000000    // 00
                                return -1;                       // forbidden
                            }else{                // 0000nnnn    //      R2
                                dc = (sigil & 0x0F) - 1;
                                goto R2sigil;
                            }
                        }else{                    // 0001dddd    //           Z3
                            dc = sigil & 0x0F;
                            goto Z3sigil;
                        } 
                    }else{                        // 001ddddd    //                Z0
                        dc = sigil & 0x1F;
                        goto Z0sigil;
                    }
                }else{                            // 01xxxxxx    //                     R1 | Z2 | Z1
                    if( sigil >> 5 == 2 ){        // 010xxxxx    //                     R1 | Z2
                        if( sigil >> 4 == 4 ){    // 0100dddd    //                     R1
                            dc = sigil & 0xF;
                            goto R1sigil;
                        }else{                    // 0101dddd    //                          Z2
                            dc = sigil & 0xF;
                            goto Z2sigil;
                        }
                    }else{                        // 011ddddd    //                               Z1
                        dc = sigil & 0x1F;
                        goto Z1sigil;
                    }
                }
            }else{                                // 1xxxxxxx    // R0 | N | F1 | F2 | F3 | F0
                if( sigil >> 6 ==  2 ){           // 10xxxxxx    // R0 | N
                    if( sigil >> 5 ==  4 ){       // 100ddddd    // R0
                        dc = sigil & 0x1F;
                        goto R0sigil;
                    }else{                        // 101ddddd    //      N
                        dc = sigil & 0x1F;
                        goto Nsigil;
                     }   
                }else{                            // 11xxxxxx    //          F1 | F2 | F3 | F0
                    if( sigil >> 5 ==  6 ){       // 110ddddd    //          F1
                        dc = sigil & 0x1F;
                        goto F1sigil;
                    }else{                        // 111xxxxx    //               F2 | F3 | F0
                        if( sigil >> 4 ==  0xE ){ // 1110dddd    //               F2 
                            dc = sigil & 0xF;
                            goto F2sigil;
                        }else{                    // 1111xxxx    //                    F3 | F0
                            if( sigil == 0xFF ){  // 11111111    //                         F0
                                dc = 0;
                                goto F0sigil;
                            }else{                // 1111dddd    //                    F3
                                dc = sigil & 0xF;
                                goto F3sigil;
                            }
                        }
                    }
                }
            }
            // The following jump points are code to be executed inside the upper binary search, but "outsourced" here for better readability.
            // This example is for better decode understanding:
            // # state          #                      in buffer #                                   out buffer # state derived action to follow
            // # sg zc fc rc dc # ------------------------------ # -------------------------------------------- # -------------------------------------------------------------
            // #     0  0  0  0 # 22 33 Z0|2 Z0 77 88 99 Z0|3 F3 #                                              # start
            // # F3  0  0  0  0 # 22 33 Z0|2 Z0 77 88 99 Z0|3    #                                              # all 0 -> F3 stored && continue
            // # Z0  0  1  0  3 # 22 33 Z0|2 Z0 77 88 99         #                                              # Z0 && fc -> a change -> writeFn( &o, ss, fc );
            // |     0  0  0  3 # 22 33 Z0|2 Z0 77 88 99         #                                  FF FF FF FF # *--o = 0;
            // |     0  0  0  3 # 22 33 Z0|2 Z0 77 88 99         #                               00 FF FF FF FF # COPY_BYTES
            // |     0  0  0  0 # 22 33 Z0|2 Z0                  #                      77 88 99 00 FF FF FF FF # continue
            // # Z0  0  0  0  0 # 22 33 Z0|2                     #                      77 88 99 00 FF FF FF FF # all 0 -> Z0 stored && continue
            // # Z0  0  1  0  2 # 22 33                          #                      77 88 99 00 FF FF FF FF # Z0 & zc & dc -> a change -> writeZn( &o, ss, zc ); Z0Z0==5
            // |     0  0  0  2 # 22 33                          #       00 00 00 00 00 77 88 99 00 FF FF FF FF # COPY_BYTES
            // |     0  0  0  0 # 22 33                          # 22 33 00 00 00 00 00 77 88 99 00 FF FF FF FF # i == input -> done

            Z0sigil:
                if( fc|rc|dc == 0 && i != input ){
                    *--ss = Z0;
                    zc++;
                    continue;
                }
                if( fc ){
                    ASSERT( rc == 0 )
                    ASSERT( fc == cc + MAX_CIPHERS - ss )
                    writeFn( &o, &ss, &fc );
                }
                if( rc ){
                    ASSERT( fc == 0 )
                    ASSERT( rc == cc + MAX_CIPHERS - ss )
                    writeRn( &o, &ss, &rc, repeatByte );
                }
                *--ss = Z0;
                zc++;
                writeZn( &o, &ss, &zc );
                COPY_BYTES
                continue;
            Z1sigil:
                if( fc|rc|dc == 0){
                    *--ss = Z1;
                    zc++;
                    continue;
                }
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 2 )
                    *--o = 0;
                    *--o = 0;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            Z2sigil:
                if( fc|rc|dc == 0){
                    *--ss = Z2;
                    zc++;
                    continue;
                }
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 3 )
                    *--o = 0;
                    *--o = 0;
                    *--o = 0;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            Z3sigil:
                if( fc|rc|dc == 0){
                    *--ss = Z3;
                    zc++;
                    continue;
                }
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 4 )
                    *--o = 0;
                    *--o = 0;
                    *--o = 0;
                    *--o = 0;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            F0sigil:
                if( zc|rc|dc == 0){
                    *--ss = F0;
                    fc++;
                    continue;
                }
                if( i == input ){ // done
                    CHECK_SPACE( 1 );
                    *--o = 0xFF;
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            F1sigil:
                if( zc|rc|dc == 0){
                    *--ss = F1;
                    fc++;
                    continue;
                }
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 2 )
                    *--o = 0xFF;
                    *--o = 0xFF;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            F2sigil:
                if( zc|rc|dc == 0){
                    *--ss = F2;
                    fc++;
                    continue;
                }
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 3 )
                    *--o = 0xFF;
                    *--o = 0xFF;
                    *--o = 0xFF;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            F3sigil:
                if( zc|rc|dc == 0){
                    *--ss = F3;
                    fc++;
                    continue;
                }
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 4 )
                    *--o = 0xFF;
                    *--o = 0xFF;
                    *--o = 0xFF;
                    *--o = 0xFF;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            R0sigil:
                if( zc|fc|dc == 0){
                    *--ss = R0;
                    rc++;
                    continue;
                }
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 2 )
                    repeatByte = *--i;
                    i++;
                    *--o = repeatByte;
                    *--o = repeatByte;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            R1sigil:
                if( zc|fc|dc == 0){
                    *--ss = R1;
                    rc++;
                    continue;
                }
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 3 )
                    repeatByte = *--i;
                    i++;
                    *--o = repeatByte;
                    *--o = repeatByte;
                    *--o = repeatByte;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            R2sigil:
                if( zc|fc|dc == 0){
                    *--ss = R2;
                    rc++;
                    continue;
                }
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 4 )
                    repeatByte = *--i;
                    i++;
                    *--o = repeatByte;
                    *--o = repeatByte;
                    *--o = repeatByte;
                    *--o = repeatByte;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            Nsigil:
                if( dc > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 0 )
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
        }
    }
}

static void writeZn( uint8_t ** out, uint8_t ** ss, int * ciphersCount){
    ASSERT( *ciphersCount == 1 ) // todo
    *out -= 1;
    **out = 0;
    *ss += *ciphersCount;
    *ciphersCount = 0;
}

static void writeFn( uint8_t ** out, uint8_t ** ss, int * ciphersCount){
    ASSERT( *ciphersCount == 1 ) // todo
    *out -= 1;
    **out = 0xFF;
    *ss += *ciphersCount;
    *ciphersCount = 0;
}

static void writeRn( uint8_t ** out, uint8_t ** ss, int * ciphersCount, uint8_t repeatByte ){
    ASSERT( *ciphersCount == 1 ) // todo
    *out -= 1;
    **out = repeatByte;
    *ss += *ciphersCount;
    *ciphersCount = 0;
}
