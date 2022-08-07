package tcobs

import (
	"bytes"
	"errors"
	"fmt"
	"io"
)

type decoder struct {
	r    io.Reader // inner reader
	iBuf []byte    // input buffer
	iCnt int       // valid data inside iBuf
}

// NewDecoder creates a decoder instance and returns its address.
// r will be used as inner reader.
// size is used as size for the inner buffers.
// Max one decoded TCOBS package is returned by Read.
func NewDecoder(r io.Reader, size int) (p *decoder) {
	p = new(decoder)
	p.r = r
	p.iBuf = make([]byte, size)
	return
}

// Read returns one decoded TCOBS package if available.
// If n == len(buffer), the returned package is maybe not complete
// and further Read calls are needed.
// io.EOF is returned when inner reader reached end of input stream.
func (p *decoder) Read(buffer []byte) (n int, e error) {
	if len(p.iBuf[p.iCnt:]) == 0 {
		e = errors.New("encoded package is bigger than internal buffer, cannot read")
		return
	}
	nCnt, e := p.r.Read(p.iBuf[p.iCnt:])
	p.iCnt += nCnt
	if e != nil && e != io.EOF {
		return
	}
	before, after, found := bytes.Cut(p.iBuf[:p.iCnt], []byte{0})
	if !found {
		return
	}
	n = CDecode(buffer, before)
	p.iCnt = copy(p.iBuf, after)
	if n < -2000000 {
		e = fmt.Errorf("CDecode returned input buffer corrupted error from tcobsDecode.c line number %d)", -n-2000000)
		n = 0
		return
	} else if n < -1000000 {
		e = fmt.Errorf("CDecode returned output buffer too small error from tcobsDecode.c line number %d)", -n-1000000)
		n = 0
		return
	} else if n < 0 {
		e = fmt.Errorf("CDecode returned an error. Check tcobsDecode.c line number %d", -n)
		n = 0
		return
	} else {
		e = nil
	}
	copy(buffer, buffer[len(buffer)-n:])
	return
}
