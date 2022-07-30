package tcobs_test

import (
	"bytes"
	"io"
	"testing"

	"github.com/rokath/tcobs"
	"github.com/stretchr/testify/assert"
)

// TestSingleRead concatenates encoded testData with 0-delimiters and expects that Read returns decoded single packages.
func TestSingleRead(t *testing.T) {
	enc := createEncodedStream()
	p := tcobs.NewDecoder(bytes.NewReader(enc), testBufferSize, false)
	dec := make([]byte, testBufferSize)
	for _, x := range testData {
		n, e := p.Read(dec)
		assert.True(t, e == nil || e == io.EOF)
		assert.Equal(t, x.dec, dec[:n])
	}
}

// TestMultiRead concatenates encoded testData with 0-delimiters and expects that Read returns all packages decoded in one call.
func TestMultiRead(t *testing.T) {
	enc := createEncodedStream()
	p := tcobs.NewDecoder(bytes.NewReader(enc), testBufferSize, true)
	dec := make([]byte, testBufferSize)
	n, e := p.Read(dec)
	if e != nil {
		assert.True(t, e == io.EOF)
	}
	exp := createDecodedStream()
	assert.Equal(t, exp, dec[:n])
}

// TestMultiRead3Packets decodes 3 TCOBS packages with 0-delimiters and expects that Read returns all packages decoded in one call.
func TestMultiRead3Packets(t *testing.T) {
	enc := []byte{
		0,
		0xaa, 0xff, 0x02, 0,
		0xaa, 0xbb, 0xff, 0x03, 0,
		0,
		0x1, 0x2, 0x62, 0xff, 0x2, 0xff, 0x1, 0x54, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0xaa, 0,
	}
	p := tcobs.NewDecoder(bytes.NewReader(enc), testBufferSize, true)
	dec := make([]byte, testBufferSize)
	n, e := p.Read(dec)
	if e != nil {
		assert.True(t, e == io.EOF)
	}
	exp := []byte{0xaa, 0xff, 0xaa, 0xbb, 0xff, 0x1, 0x2, 0x0, 0x0, 0xff, 0x2, 0xff, 0x1, 0x0, 0x0, 0x0, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0x1, 0x1, 0x1, 0x1}
	assert.Equal(t, exp, dec[:n])
}

// TestMultiRead3PacketsWith1stAsError decodes 3 TCOBS packages with 0-delimiters and expects that Read returns last 2 packages decoded in one call, because 1st package is wrong.
func _TestMultiRead3PacketsWith1stAsError(t *testing.T) {
	enc := []byte{
		0,
		0xaa, 0xff, 0x02, 0, // right package
		//0xaa, 0xff, 0x99, 0, // wrong package
		0xaa, 0xbb, 0xff, 0x03, 0,
		0,
		0x1, 0x2, 0x62, 0xff, 0x2, 0xff, 0x1, 0x54, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0xaa, 0,
	}
	p := tcobs.NewDecoder(bytes.NewReader(enc), testBufferSize, true)
	dec := make([]byte, testBufferSize)
	n, e := p.Read(dec)
	//assert.True(t, e != nil && e != io.EOF) // expect an error
	// todo: the 2nd try should continue correct
	//n, e = p.Read(dec)
	assert.True(t, e == nil || e == io.EOF)
	exp := []byte{0xaa, 0xff, 0xaa, 0xbb, 0xff, 0x1, 0x2, 0x0, 0x0, 0xff, 0x2, 0xff, 0x1, 0x0, 0x0, 0x0, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0x1, 0x1, 0x1, 0x1}
	assert.Equal(t, exp, dec[:n])
}

// TestMultiRead3PacketsWithLastIncomplete decodes 3 TCOBS packages with 0-delimiters and expects that Read returns all packages decoded in two calls.
func _TestMultiRead3PacketsWithLastIncomplete(t *testing.T) {
	enc := []byte{
		0,
		0xaa, 0xff, 0x02, 0,
		0xaa, 0xbb, 0xff, 0x03, 0,
		0,
		0x1, 0x2, 0x62, 0xff, 0x2, 0xff, 0x1, 0x54, 0x2, 0x1, 0xff, 0x1, 0x1, 0x2, 0x2, 0x1, 0xff, 0x1, 0xaa, 0,
	}
	p := tcobs.NewDecoder(bytes.NewReader(enc), 16, true)
	dec := make([]byte, testBufferSize)
	n, e := p.Read(dec)
	// todo: successive read with small buffer should work
	assert.True(t, e == nil || e == io.EOF)
	exp := []byte{0x0, 0x0, 0x0, 0x0, 0x0, 0xaa, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x0, 0x0, 0x0, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0xff}
	assert.Equal(t, exp, dec[:n])

	n, e = p.Read(dec)
	//if e != nil {
	//	assert.True(t, e == io.EOF)
	//}
	exp = []byte{
		//0, 0, 0, 0, 0, 0xAA, 0xFF, 0xFF, 0xAA,
		//0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
		0, 0, 0, 0, 0, 0xFF,
	}
	assert.Equal(t, exp, dec[:n])
}
