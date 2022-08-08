package tcobs_test

import (
	"bytes"
	"fmt"
	"io"
	"testing"

	"github.com/rokath/tcobs"
	"github.com/stretchr/testify/assert"
)

// TestSingleRead concatenates encoded testData with 0-delimiters and expects that Read returns decoded single packages.
func TestSingleRead(t *testing.T) {
	enc := createEncodedStream()
	p := tcobs.NewDecoder(bytes.NewReader(enc), testBufferSize)
	dec := make([]byte, testBufferSize)
	for _, x := range testData {
		n, e := p.Read(dec)
		assert.True(t, e == nil || e == io.EOF)
		assert.Equal(t, x.dec, dec[:n])
	}
}

// TestMultiReadPackets decodes 6 TCOBS packages with 0-delimiters and expects that Read returns 1 package decoded in one call.
// As long a package is returned the return value is nil. If no more decodable data return is io.EOF.
func TestMultiReadPackets(t *testing.T) {
	enc := []byte{
		0,
		0xaa, 0xff, 0x02, 0,
		0xaa, 0xbb, 0xff, 0x03, 0,
		0,
		0x1, 0x2, 0x62, 0xff, 0x2, 0xff, 0x1, 0x54, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0xaa, 0,
		0,
	}
	exp := [][]byte{
		{},
		{0xaa, 0xff},
		{0xaa, 0xbb, 0xff},
		{},
		{0x1, 0x2, 0x0, 0x0, 0xff, 0x2, 0xff, 0x1, 0x0, 0x0, 0x0, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0x1, 0x1, 0x1, 0x1},
		{},
	}
	expErr := []error{nil, nil, nil, nil, nil, nil, io.EOF}

	p := tcobs.NewDecoder(bytes.NewReader(enc), testBufferSize)
	dec := make([]byte, testBufferSize)
	for i := 0; i < 7; i++ {
		n, e := p.Read(dec)
		fmt.Println(e)
		assert.True(t, e == expErr[i])
		if e == nil {
			assert.Equal(t, exp[i], dec[:n])
		}
	}
}

func TestMultiReadPacketsWithOneErrorPackageInside(t *testing.T) {
	enc := []byte{
		0,
		0xaa, 0xff, 0x02, 0,
		0xaa, 0xbb, 0xff /*0x03*/, 0x99, 0, // error injected: 0x99 instead of 0x03
		0,
		0x1, 0x2, 0x62, 0xff, 0x2, 0xff, 0x1, 0x54, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0xaa, 0,
		0,
	}
	exp := [][]byte{
		{},
		{0xaa, 0xff},
		{}, //{0xaa, 0xbb, 0xff}, <-- this package gets lost
		{},
		{0x1, 0x2, 0x0, 0x0, 0xff, 0x2, 0xff, 0x1, 0x0, 0x0, 0x0, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0x1, 0x1, 0x1, 0x1},
		{},
	}

	p := tcobs.NewDecoder(bytes.NewReader(enc), testBufferSize)
	dec := make([]byte, testBufferSize)
	var ecnt int
	for i := range exp {
		n, e := p.Read(dec)
		if e != nil && e != io.EOF {
			ecnt++
		}
		assert.Equal(t, exp[i], dec[:n])
	}
	assert.Equal(t, 1, ecnt)
}

// When the internal buffer is smaller than the encoded buffer, no read is possible.
func TestTooSmallBufferRead0(t *testing.T) {
	enc := []byte{
		0x1, 0x2, 0x62, 0xff, 0x2, 0xff, 0x1, 0x54, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0xaa, 0,
		0xaa, 0xbb, 0x02, 0,
	}
	p := tcobs.NewDecoder(bytes.NewReader(enc), 15)
	dec := make([]byte, 100)

	// package does not fit in internal buffer and is ignored therefore
	n, e := p.Read(dec)
	assert.True(t, e != nil)
	assert.True(t, e != io.EOF)
	assert.True(t, n == 0)

	// next package is ok
	n, e = p.Read(dec)
	assert.Nil(t, e)
	assert.True(t, n == 2)
}

// When the provided buffer is smaller than the decoded buffer, no read is possible.
func TestTooSmallBufferRead1(t *testing.T) {
	enc := []byte{
		0x1, 0x2, 0x62, 0xff, 0x2, 0xff, 0x1, 0x54, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0xaa, 0,
		0xaa, 0xbb, 0x02, 0,
	}
	p := tcobs.NewDecoder(bytes.NewReader(enc), 150)
	dec := make([]byte, 10)

	// package does not fit in external buffer
	n, e := p.Read(dec)
	assert.True(t, e != nil)
	assert.True(t, e != io.EOF)
	assert.True(t, n < 1000000)

	// next package is ok
	n, e = p.Read(dec)
	assert.Nil(t, e)
	assert.True(t, n == 2)
}
