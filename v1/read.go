package tcobsv1

import (
	"bytes"
	"errors"
	"io"
)

type decoder struct {
	r    io.Reader // upstream reader containing 0-delimited encoded frames
	iBuf []byte    // internal accumulation buffer for encoded bytes
	iCnt int       // number of valid bytes currently stored in iBuf
}

// NewDecoder creates a streaming decoder reading from r.
// size defines the internal encoded-frame buffer size.
// Each Read call returns at most one decoded package.
func NewDecoder(r io.Reader, size int) (p *decoder) {
	p = new(decoder)
	p.r = r
	p.iBuf = make([]byte, size)
	return
}

// Read returns one decoded package in buffer when a full 0-delimited frame is available.
// io.EOF is returned when the upstream reader has no more data and no pending bytes remain.
// If the internal encoded frame exceeds the configured size, that frame is dropped and an error is returned.
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
		return // upstream read error
	}
	if e == io.EOF && p.iCnt == 0 {
		return // end of stream with no pending bytes
	}
	before, after, found := bytes.Cut(p.iBuf[:p.iCnt], []byte{0})
	if !found {
		goto start
	}
	if dataInvalid {
		p.iCnt = copy(p.iBuf, after) // drop oversized frame and keep bytes after delimiter
		dataInvalid = false
		e = errors.New("encoded package is bigger than internal buffer, cannot read")
		return
	}
	n, e = Decode(buffer, before)
	p.iCnt = copy(p.iBuf, after)
	if e != nil && e != io.EOF {
		n = 0
		return
	}
	copy(buffer, buffer[len(buffer)-n:])
	return
}

// Alternate and less tested implementation using C-code.
//  // Read returns one decoded TCOBS package if available.
//  // If n == len(buffer), the returned package is maybe not complete
//  // and further Read calls are needed.
//  // io.EOF is returned when inner reader reached end of input stream.
//  func (p *decoder) Read(buffer []byte) (n int, e error) {
//  	if len(p.iBuf[p.iCnt:]) == 0 {
//  		e = errors.New("encoded package is bigger than internal buffer, cannot read")
//  		return
//  	}
//  	nCnt, e := p.r.Read(p.iBuf[p.iCnt:])
//  	p.iCnt += nCnt
//  	if e != nil && e != io.EOF {
//  		return
//  	}
//  	before, after, found := bytes.Cut(p.iBuf[:p.iCnt], []byte{0})
//  	if !found {
//  		return
//  	}
//  	n = CDecode(buffer, before)
//  	if n < -2000000 {
//  		e = fmt.Errorf("CDecode returned input buffer corrupted error from tcobsDecode.c line number %d)", -n-2000000)
//  		return
//  	} else if n < -1000000 {
//  		e = fmt.Errorf("CDecode returned output buffer too small error from tcobsDecode.c line number %d)", -n-1000000)
//  		return
//  	} else if n < 0 {
//  		e = fmt.Errorf("CDecode returned an error. Check tcobsDecode.c line number %d", -n)
//  		return
//  	} else {
//  		e = nil
//  	}
//  	copy(buffer, buffer[len(buffer)-n:])
//  	p.iCnt = copy(p.iBuf, after)
//  	return
//  }
