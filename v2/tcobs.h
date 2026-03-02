/*! \file tcobs.h
\author thomas.hoehenleitner [at] seerose.net
\details See ./TCOBSv2Specification.md.
*******************************************************************************/

#ifndef TCOBS_H_
#define TCOBS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

//! TCOBSEncode transforms "length" bytes of data beginning at the location pointed to by "input" and writes the output to
//! the location pointed to by "output". It returns the number of bytes written to "output" or a negative value on error.
//! A 0-delimiter is NOT added as last byte, allowing concatenation of multiple TCOBS encoded buffers before 00-byte delimiting.
//! Buffer overlapping is allowed when input lies inside output with a sufficient offset.
//! In the worst case, one additional sigil byte is inserted for each 31 input bytes.
//! The overlap offset should be >= the next larger whole number of length/31.
//! The provided output buffer size should be >= length + next larger whole number of length/31.
//! This is the caller's responsibility and is not checked for efficiency reasons.
//! Remove the "restrict" qualifiers if compiling with a pre-C99 C dialect.
int TCOBSEncode( void * restrict output, const void * restrict input, size_t length);

//! TCOBSDecode decodes data ending at the location pointed to by "input" backwards (starting with the last byte)
//! and writes the output also backwards to the location pointed to by "output" with a maximum size of max.
//! Returns the number of bytes written to "output". Only max bytes are written. If the returned value is
//! > max, this is an error "output buffer too small". In case of an error, a negative value is returned.
//! THIS IS **IMPORTANT**: The decoded data start at output+max-returned, because output is filled from the end.
//! Buffer overlapping is partially possible if output limit is _behind_ input limit with sufficient distance,
//! but decoded data can get much longer. Remove the "restrict" qualifiers if compiling with a pre-C99 C dialect.
int TCOBSDecode( void * restrict output, size_t max, const void * restrict input, size_t length );

#define OUT_BUFFER_TOO_SMALL -1000000 //!< OUT_BUFFER_TOO_SMALL is TCOBSDecode return error code.
#define INPUT_DATA_CORRUPTED -2000000 //!< INPUT_DATA_CORRUPTED is TCOBSDecode return error code.

#ifdef __cplusplus
}
#endif

#endif // TCOBS_H_
