package tcobsv2

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
// io.EOF is returned when inner reader reached end of input stream.
// The inner buffer must be able to hold the whole TCOBS package, otherwise it is dropped
// and an error is returned.
func (p *decoder) Read(buffer []byte) (n int, e error) {
	var dataInvalid bool
start:
	if len(p.iBuf[p.iCnt:]) == 0 {
		p.iCnt = 0 // drop data
		dataInvalid = true
	}
	nCnt, e := p.r.Read(p.iBuf[p.iCnt:])
	p.iCnt += nCnt
	if e != nil && e != io.EOF {
		return // internal Read error
	}
	if e == io.EOF && p.iCnt == 0 {
		return // no more data
	}
	before, after, found := bytes.Cut(p.iBuf[:p.iCnt], []byte{0})
	if !found {
		goto start
	}
	if dataInvalid {
		p.iCnt = copy(p.iBuf, after) // remove data
		dataInvalid = false
		e = errors.New("encoded package is bigger than internal buffer, cannot read")
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
