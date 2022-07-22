/*! \file tcobs.h
\author thomas.hoehenleitner [at] seerose.net
*******************************************************************************/

#ifndef TCOBS_H_
#define TCOBS_H_

#include <stddef.h>
#include <stdint.h>

//! TCOBSEncode stuffs "length" bytes of data at the location pointed to by "input"
//! and writes the output to the location pointed to by "output".
//! A 0-delimiter is NOT added as last byte. Returns the number of bytes written to "output". 
//! Buffer overlapping is allowed, when input lays inside output with a sufficient offset.
//! The offset should be >= (length>>5) because in the worst case for each 31 bytes an additional sigil byte is inserted.
//! The provided output buffer size should be > length + (length>>5). This is a responsibility of the caller.
//! Remove the "restrict" qualifiers if compiling with a pre-C99 C dialect.
size_t TCOBSEncode( void * restrict output, const void * restrict input, size_t length);

//! TCOBSDecode decodes data ending at the location pointed to by "input" backwards 
//! and writes the output to the location pointed to by "output" with a maximum size of max.
//! Returns the number of bytes written to "output". Only max bytes are written. If the returned value is
//! > max, this is an error "output buffer too small". 
//! THIS IS IMPORTANT: The decoded data start at output+max-returned, because output is filled from the end.
//! Buffer overlapping is partially possible if output is behind input but data can get much longer.
//! Remove the "restrict" qualifiers if compiling with a pre-C99 C dialect.
size_t TCOBSDecode( void * restrict output, size_t max, const void * restrict input );

#ifdef __cplusplus
}
#endif

#endif // TCOBS_H_
