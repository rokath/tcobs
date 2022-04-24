package tcobs

import (
	"errors"
	"io"
)

// untested!!!

type encoder struct {
	w    io.Writer // inner writer
	iBuf []byte    // input buffer
	iCnt int       // iCnt is byte count on hold in iBuf
}

// NewEncoder creates an encoder instance and returns its address.
// w will be used as inner writer and size is used as initial size for the inner buffer.
func NewEncoder(w io.Writer, size int) (p *encoder) {
	p = new(encoder)
	p.w = w
	p.iBuf = make([]byte, size)
	return
}

// Write encodes buffer and writes the encoded content. Return cases:
// 0, errors.New("inner buffer not empty (needs Flush)"): nothing done
// len(buffer), nil: all done
// 0<n && n<len(buffer), nil: partially done (inner buffer size too small)
// 0, !nil: inner reader issue, nothing done
// 0<n && n<=len(buffer), errors.New("inner buffer not empty (needs Flush)"): partially or full done, needs Flush
func (p *encoder) Write(buffer []byte) (n int, e error) {
	if p.iCnt > 0 {
		e = errors.New("inner buffer not empty (needs Flush)")
		return
	}
	n = len(buffer)
	for max := n + 1 + ((n + 1) >> 5); max > len(p.iBuf); { // worst case: no compression possible
		n >>= 1
	}
	siz := CEncode(p.iBuf, buffer[:n])
	enc := append(p.iBuf[:siz], 0) // 0-delimiter
	m, e := p.w.Write(enc)
	if m == len(enc) { // all written
		return
	}
	if m == 0 { // could not write at all
		n = 0
		return
	}
	// some bytes could be written, so keep the leftovers for Flush
	p.iCnt = copy(p.iBuf, enc[m:])
	e = errors.New("inner buffer not empty (needs Flush)")
	return
}

// Flush tries to write inner buffer with the inner writer and returns nil when all data could be written.
func (p *encoder) Flush() error {
	n, e := p.w.Write(p.iBuf[:p.iCnt])
	p.iCnt -= n
	if p.iCnt == 0 {
		return e
	}
	return errors.New("inner buffer not empty yet")
}
