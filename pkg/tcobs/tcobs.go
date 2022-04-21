// Package tcobs is also a helper for testing the target C-code.
// Each C function gets a Go wrapper which ist tested in appropriate test functions.
// For some reason inside the tcobs_test.go an 'import "C"' is not possible.
package tcobs

// #include <stdint.h>
// #include "tcobs.h"
// #cgo CFLAGS: -g -Wall -flto
import "C"
import (
	"bytes"
	"errors"
	"io"
	"unsafe"
)

type decoder struct {
	r   io.Reader // inner reader
	buf []byte    // inner buffer
	pos int       // inner position
}

// NewDecoder creates a decoder instance and returns its address.
// r will be used as inner reader and size is used as initial size for the inner buffer.
func NewDecoder(r io.Reader, size int) *decoder {
	d := new(decoder)
	d.r = r
	d.buf = make([]byte, size)
	return d
}

func (p *decoder) Read(buffer []byte) (n int, e error) {
	cnt, e := p.r.Read(p.buf[p.pos:])

	if e != nil {
		return
	}
	p.pos += cnt
	tcobs, found := nextPackage(p.buf[:p.pos])
	if !found {
		return 0, nil
	}
	p.buf = p.buf[len(tcobs)+1:]
	n, e = Decode(buffer, tcobs)
	return
}

// nextPackage returns in cobs the first buf bytes until the 0-delimiter not including it.
// If no 0-delimiter is found, found is false and len(cobs) is 0.
// If a 0 delimiter was found (could be the first byte) found is true
func nextPackage(buf []byte) (cobs []byte, found bool) {
	// todo
	return
}

/*
// DecodeZ decodes a TCOBS frame `in` (until end or 0-delimiter) to `d` and returns as `n` the valid data length from the end in `d`.
// ATTENTION: d is filled from the end! decoded := d[len(d)-n:] is the final result.
// In `m` are returned the processed bytes inside `in`.
// In case of an error n is 0. n can be 0 without having an error.
// For details see TCOBSSpecification.md.
func DecodeZ(d, in []byte) (n, m int, e error) {
	return
}
*/

// CEncode encodes `i` into `o` and returns number of bytes in `o`.
// For details see TCOBSSpecification.md.
// The CEncode implementation is done in C because the aimed use case is an embedded device running C.
// This function is mainly for testing.
func CEncode(o, i []byte) (n int) {
	if len(i) == 0 {
		return
	}
	in := unsafe.Pointer(&i[0])
	out := unsafe.Pointer(&o[0])
	n = int(C.TCOBSEncode(out, in, C.size_t(len(i))))
	return
}

const (
	N        = 0xA0 // noop sigil byte
	Z1       = 0x20 // 1-zero sigil byte
	Z2       = 0x40 // 2-zero sigil byte
	Z3       = 0x60 // 3-zero sigil byte
	F2       = 0xC0 // 2-FF sigil byte
	F3       = 0xE0 // 3-FF sigil byte
	F4       = 0x80 // 4-FF sigil byte
	R2       = 0x08 // 2-repeat sigil byte
	R3       = 0x10 // 3-repeat sigil byte
	R4       = 0x18 // 4-repeat sigil byte
	Reserved = 0x00 // reserved sigil byte
)

// sigilAndOffset interprets by as sigil byte with offset and returns sigil and offset.
// For details see TCOBSSpecification.md.
func sigilAndOffset(by uint8) (sigil, offset int) {
	b := int(by)
	sigil = b & 0xE0 // 0x11100000
	if sigil == 0 {
		sigil = b & 0xF8 // 0x11111000
		offset = 7 & b   // 0x00000111
		return
	}
	offset = 0x1F & b // 0x00011111
	return
}

// Decode decodes a TCOBS frame `in` (without 0-delimiter) to `d` and returns as `n` the valid data length from the end in `d`.
// ATTENTION: d is filled from the end! decoded := d[len(d)-n:] is the final result.
// In case of an error n is 0. n can be 0 without having an error.
// Framing with 0-delimiter to be done before is assumed, so no 0-checks performed.
// For details see TCOBSSpecification.md.
func Decode(d, in []byte) (n int, e error) {
	for {
		if len(in) == 0 {
			return
		}
		next := in[len(in)-1] // get next sigil byte (starting from the end)
		sigil, offset := sigilAndOffset(next)
		if offset+1 > len(in) {
			e = errors.New("sigil chain mismatch")
			return
		}
		in = in[:len(in)-1] // remove sigil byte from buffer
		switch sigil {
		case N:
			goto copyBytes

		case Z3:
			n++
			d[len(d)-n] = 0
			fallthrough
		case Z2:
			n++
			d[len(d)-n] = 0
			fallthrough
		case Z1:
			n++
			d[len(d)-n] = 0
			goto copyBytes

		case F4:
			n++
			d[len(d)-n] = 0xFF
			fallthrough
		case F3:
			n++
			d[len(d)-n] = 0xFF
			fallthrough
		case F2:
			n++
			d[len(d)-n] = 0xFF
			n++
			d[len(d)-n] = 0xFF
			goto copyBytes

		case R4:
			n++
			d[len(d)-n] = repeatByte(offset, in)
			fallthrough
		case R3:
			n++
			d[len(d)-n] = repeatByte(offset, in)
			fallthrough
		case R2:
			r := repeatByte(offset, in)
			n++
			d[len(d)-n] = r
			n++
			d[len(d)-n] = r

			goto copyBytes

		case Reserved:
			n = 0
			e = errors.New("reserved sigil not allowed")
			return
		}

	copyBytes:
		to := len(d) - n - offset
		from := len(in) - offset // sigil byte is already removed
		n += copy(d[to:], in[from:])
		in = in[:len(in)-offset] // remove copied bytes
		continue
	}
}

// Decoder expects in s a 0-delimited number of TCOBS packages. If a 0-delimiter is found inside s,
// it tries to decode all bytes until the 0-delimiter as TCOBS package and writes the result in decoded.
// If s does not contain any 0-delimiter, decoded is nil, after is s and e is nil.
// If a 0-delimiter was found, inside 'after' all data after the found delimiter.
// If the TCOBS decoding of the data until the 0-delimiter failed, decoded has len 0, and e is not nil.
// For details see TCOBSSpecification.md.
func Decoder(s []byte) (decoded, after []byte, e error) {
	tcobs, after, found := bytes.Cut(s, []byte{0})
	if !found {
		after = s
		return
	}
	decoded = make([]byte, 2*len(tcobs))
	n, e := Decode(decoded, tcobs)
	decoded = decoded[len(decoded)-n:]
	return
}

// repeatByte returns the value to repeat
func repeatByte(offset int, in []byte) byte {
	if offset == 0 { // left byte of Ri is a sigil byte (probably N)
		return in[len(in)-2] // a buffer cannot start with Ri
	} else {
		return in[len(in)-1]
	}
}
