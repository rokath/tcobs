package tcobs

import (
	"bytes"
	"io"
)

// todo: error handling

type decoder struct {
	r     io.Reader // inner reader
	iBuf  []byte    // input buffer
	iCnt  int       // valid data inside iBuf
	multi bool      // Read behavior selection
}

// NewDecoder creates a decoder instance and returns its address.
// r will be used as inner reader,
// size is used as initial size for the inner buffers and
// multi selects Read behavior.
// When multi is false, max one decoded TCOBS package is returned by Read.
func NewDecoder(r io.Reader, size int, multi bool) (p *decoder) {
	p = new(decoder)
	p.r = r
	p.iBuf = make([]byte, size)
	p.multi = multi
	return
}

// Read uses inner reader for encoded data, decodes them and returns the result.
func (p *decoder) Read(buffer []byte) (n int, e error) {
	if p.multi {
		return p.multiRead(buffer)
	} else {
		return p.singleRead(buffer)
	}
}

// singleRead returns one decoded COBS package if available.
func (p *decoder) singleRead(buffer []byte) (n int, e error) {
	e = p.get()
	if e != nil && e != io.EOF {
		return
	}
	before, after, found := bytes.Cut(p.iBuf[:p.iCnt], []byte{0})
	if !found {
		return
	}
	tmp := make([]byte, 4*len(before))
	n, e = Decode(tmp, before)
	copy(buffer, tmp[len(tmp)-n:])
	p.iCnt = copy(p.iBuf, after)
	return
}

// multiRead returns all decoded COBS package available as long no error occurs.
func (p *decoder) multiRead(buffer []byte) (n int, e error) {
	e = p.get()
	if e != nil && e != io.EOF {
		return
	}
	input := bytes.Split(p.iBuf[:p.iCnt], []byte{0})
	for _, x := range input {
		var cnt int
		tmp := make([]byte, 4*len(x))
		cnt, e = Decode(tmp, x)
		// todo: ERROR handling
		//  if e != nil {
		//  	if i == len(input) { // last element is maybe an incomplete package
		//  	p.rest = x // keep rest for the next Read
		//  	return
		//  	}else{ // probably a merge of 3 packets (data disruption)
		//  		fmt.Printf( "%w, data disruption?\n", e)
		//  	}
		//  }
		decoded := tmp[len(tmp)-cnt:]
		n += copy(buffer[n:], decoded)
	}
	p.iCnt = 0
	return
}

// get uses inner reader to read data into p.iBuf
func (p *decoder) get() error {
	nCnt, e := p.r.Read(p.iBuf[p.iCnt:])
	p.iCnt += nCnt
	return e
}
