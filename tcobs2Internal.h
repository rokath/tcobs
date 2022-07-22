/*! \file tcobs2Internal.h
\author Thomas.Hoehenleitner [at] seerose.net
\details See ./TCOBS2Specification.md. Only internal usage.
*******************************************************************************/

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
