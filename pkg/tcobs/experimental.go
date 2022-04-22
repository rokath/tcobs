package tcobs

import (
	"bytes"
	"io"
)

// DO NOT USE code below - Work in progress !

type decoder struct {
	r        io.Reader // inner reader
	rest     []byte    // not processed read data
	restSize int       // valid data inside rest
	iBuf     []byte    // input buffer
	sBuf     []byte    // scratch buffer, maybe not needed, id buffer is used (tests!)
	multi    bool      // Read behavior selection
}

// NewReader creates a decoder instance and returns its address.
// r will be used as inner reader and size is used as initial size for the inner buffers and multi select Read behavior.
// When multi is false, max one decoded TCOBS package is returned by Read.
func NewReader(r io.Reader, size int, multi bool) (p *decoder) {
	p = new(decoder)
	p.r = r
	p.rest = make([]byte, size)
	p.iBuf = make([]byte, size)
	p.sBuf = make([]byte, size)
	return
}

func (p *decoder) Read(buffer []byte) (n int, e error) {
	if p.multi {
		return p.multiRead(buffer)
	} else {
		return p.singleRead(buffer)
	}
}

// singleRead returns one decoded COBS package if available.
func (p *decoder) singleRead(buffer []byte) (n int, e error) {
	cnt, e := p.get()
	if e != nil {
		return
	}
	before, after, found := bytes.Cut(p.iBuf[:cnt], []byte{0})
	if !found {
		p.restSize = copy(p.rest, before)
		return
	}
	p.restSize = copy(p.rest, after)
	n, e = Decode(p.sBuf, before) // p.sBuf can be here buffer
	copy(buffer, p.sBuf[len(p.sBuf)-n:])
	return
}

// multiRead returns all decoded COBS package available.
func (p *decoder) multiRead(buffer []byte) (n int, e error) {
	cnt, e := p.get()
	if e != nil {
		return
	}
	input := bytes.Split(p.iBuf[:cnt], []byte{0})

	for _, x := range input {
		var cnt int
		cnt, e = Decode(p.sBuf, x)
		//  if e != nil {
		//  	if i == len(input) { // last element is maybe an incomplete package
		//  	p.rest = x // keep rest for the next Read
		//  	return
		//  	}else{ // probably a merge of 3 packets (data disruption)
		//  		fmt.Printf( "%w, data disruption?\n", e)
		//  	}
		//  }
		decoded := p.sBuf[len(p.sBuf)-cnt:]
		n += copy(buffer[n:], decoded)
	}
	return
}

// read uses inner reader to read data and returns as n the amount inside p.iBuf
func (p *decoder) get() (n int, e error) {
	rCnt := copy(p.iBuf, p.rest[:p.restSize])
	nCnt, e := p.r.Read(p.iBuf[rCnt:])
	if e != nil {
		p.restSize = copy(p.rest, p.iBuf[:rCnt+nCnt])
		return
	}
	n = rCnt + nCnt
	return
}

//  // DecodeZ decodes a TCOBS frame `in` (until end or 0-delimiter) to `d` and returns as `n` the valid data length from the end in `d`.
//  // ATTENTION: d is filled from the end! decoded := d[len(d)-n:] is the final result.
//  // In `m` are returned the processed bytes inside `in`.
//  // In case of an error n is 0. n can be 0 without having an error.
//  // For details see TCOBSSpecification.md.
//  func DecodeZ(d, in []byte) (n, m int, e error) {
//  	return
//  }
