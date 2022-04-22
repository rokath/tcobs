package tcobs

import (
	"bytes"
	"io"
)

// DO NOT USE code below - Work in progress !

//  // DecodeZ decodes a TCOBS frame `in` (until end or 0-delimiter) to `d` and returns as `n` the valid data length from the end in `d`.
//  // ATTENTION: d is filled from the end! decoded := d[len(d)-n:] is the final result.
//  // In `m` are returned the processed bytes inside `in`.
//  // In case of an error n is 0. n can be 0 without having an error.
//  // For details see TCOBSSpecification.md.
//  func DecodeZ(d, in []byte) (n, m int, e error) {
//  	return
//  }

type decoder struct {
	r        io.Reader // inner reader
	rest     []byte    // not processed incomplete buffer
	restSize int       // valid data inside rest
	bufSize  int       // internal buffer size
}

// NewReader creates a decoder instance and returns its address.
// r will be used as inner reader and size is used as initial size for the inner buffers.
func NewReader(r io.Reader, size int) *decoder {
	d := new(decoder)
	d.r = r
	d.rest = make([]byte, size)
	d.bufSize = size
	return d
}

func (p *decoder) Read(buffer []byte) (n int, e error) {
	buf := make([]byte, p.bufSize)     // internal stack allocation
	scratch := make([]byte, p.bufSize) // internal stack allocation
	rest := p.rest[:p.restSize]
	rCnt := copy(buf, rest)
	nCnt, e := p.r.Read(buf[rCnt:])
	if e != nil {
		p.restSize = copy(p.rest, buf[:rCnt+nCnt])
		return
	}
	input := bytes.Split(buf[:rCnt+nCnt], []byte{0})

	for _, x := range input {
		var cnt int
		cnt, e = Decode(scratch, x)
		//  if e != nil {
		//  	if i == len(input) { // last element is maybe an incomplete package
		//  	p.rest = x // keep rest for the next Read
		//  	return
		//  	}else{ // probably a merge of 3 packets (data disruption)
		//  		fmt.Printf( "%w, data disruption?\n", e)
		//  	}
		//  }
		decoded := scratch[len(scratch)-cnt:]
		n += copy(buffer[n:], decoded)
	}
	return
}
