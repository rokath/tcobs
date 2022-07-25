/*! \file tcobs2Decode.c
\author Thomas.Hoehenleitner [at] seerose.net
\details See ./TCOBS2Specification.md.
*******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include "tcobs.h"
#include "tcobsInternal.h"

//! CHECK_OUTPUT_SPACE checks for n plus distance output space.
#define CHECK_OUTPUT_SPACE(n) // if( (uint8_t*)output - o < n + distance ){ return OUT_BUFFER_TOO_SMALL; }

//! CHECK_INPUT_SPACE checks for distance input space.
#define CHECK_INPUT_SPACE  if( (uint8_t*)input > i - distance ){ return INPUT_DATA_CORRUPTED; }

//! CHECK_SPACE checks for sufficient input and output space.
#define CHECK_SPACE( sigilDecodedCount) CHECK_INPUT_SPACE CHECK_OUTPUT_SPACE(sigilDecodedCount)

//! COPY_BYTES transfers distance bytes backwards from the input buffer end to the output buffer end.
#define COPY_BYTES while( distance--){ *--o = *--i; }

int TCOBSDecode( void * restrict output, size_t max, const void * restrict input, size_t length ){
    if( length == 0 ){
        return 0;
    }else{
        uint8_t sigil;
        uint8_t * o = (uint8_t*)output + max; // output write pointer behind next value
        int distance;
        uint8_t const * i = (uint8_t*)input + length; // input read pointer behind next value
        uint8_t repeatByte;
        for(;;){
            if( i == input ){ // done
                return (uint8_t*)output + max - o;
            }
            sigil = *--i;
            if( sigil >> 7 ==  0 ){               // 0xxxxxxx    // 00 | R2 | Z3 | Z0 | R1 | Z2 | Z1
                if( sigil >> 6 ==  0 ){           // 00xxxxxx    // 00 | R2 | Z3 | Z0
                    if( sigil >> 5 ==  0 ){       // 000xxxxx    // 00 | R2 | Z3 
                        if( sigil >> 4 ==  0 ){   // 0000xxxx    // 00 | R2
                            if( sigil == 0 ){     // 00000000    // 00
                                return -1;                       // forbidden
                            }else{                // 0000nnnn    //      R2
                                distance = (sigil & 0x0F) - 1;
                                goto R2sigil;
                            }
                        }else{                    // 0001dddd    //           Z3
                            distance = sigil & 0x0F;
                            goto Z3sigil;
                        } 
                    }else{                        // 001ddddd    //                Z0
                        distance = sigil & 0x1F;
                        goto Z0sigil;
                    }
                }else{                            // 01xxxxxx    //                     R1 | Z2 | Z1
                    if( sigil >> 5 == 2 ){        // 010xxxxx    //                     R1 | Z2
                        if( sigil >> 4 == 4 ){    // 0100dddd    //                     R1
                            distance = sigil & 0xF;
                            goto R1sigil;
                        }else{                    // 0101dddd    //                          Z2
                            distance = sigil & 0xF;
                            goto Z2sigil;
                        }
                    }else{                        // 011ddddd    //                               Z1
                        distance = sigil & 0x1F;
                        goto Z1sigil;
                    }
                }
            }else{                                // 1xxxxxxx    // R0 | N | F1 | F2 | F3 | F0
                if( sigil >> 6 ==  2 ){           // 10xxxxxx    // R0 | N
                    if( sigil >> 5 ==  4 ){       // 100ddddd    // R0
                        distance = sigil & 0x1F;
                        goto R0sigil;
                    }else{                        // 101ddddd    //      N
                        distance = sigil & 0x1F;
                        goto Nsigil;
                     }   
                }else{                            // 11xxxxxx    //          F1 | F2 | F3 | F0
                    if( sigil >> 5 ==  6 ){       // 110ddddd    //          F1
                        distance = sigil & 0x1F;
                        goto F1sigil;
                    }else{                        // 111xxxxx    //               F2 | F3 | F0
                        if( sigil >> 4 ==  0xE ){ // 1110dddd    //               F2 
                            distance = sigil & 0xF;
                            goto F2sigil;
                        }else{                    // 1111xxxx    //                    F3 | F0
                            if( sigil == 0xFF ){  // 11111111    //                         F0
                                distance = 0;
                                goto F0sigil;
                            }else{                // 1111dddd    //                    F3
                                distance = sigil & 0xF;
                                goto F3sigil;
                            }
                        }
                    }
                }
            }
            Z0sigil:
                if( distance > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 1 )
                    *--o = 0;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            Z1sigil:
                if( distance > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 2 )
                    *--o = 0;
                    *--o = 0;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            Z2sigil:
                if( distance > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 3 )
                    *--o = 0;
                    *--o = 0;
                    *--o = 0;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            Z3sigil:
                if( distance > 0 || i == input ){ // no neighbor sigil or done
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
                if( i == input ){ // done
                    CHECK_SPACE( 1 );
                    *--o = 0xFF;
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            F1sigil:
                if( distance > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 2 )
                    *--o = 0xFF;
                    *--o = 0xFF;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            F2sigil:
                if( distance > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 3 )
                    *--o = 0xFF;
                    *--o = 0xFF;
                    *--o = 0xFF;
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
            F3sigil:
                if( distance > 0 || i == input ){ // no neighbor sigil or done
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
                if( distance > 0 || i == input ){ // no neighbor sigil or done
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
                if( distance > 0 || i == input ){ // no neighbor sigil or done
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
                if( distance > 0 || i == input ){ // no neighbor sigil or done
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
                if( distance > 0 || i == input ){ // no neighbor sigil or done
                    CHECK_SPACE( 0 )
                    COPY_BYTES
                    continue;
                }
                return INPUT_DATA_CORRUPTED; // todo
        }
    }
}
