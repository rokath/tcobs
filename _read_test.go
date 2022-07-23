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
		assert.Nil(t, e)
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
		0, 0x60, 0x40, 0xAA, 0xC1, 0xAA, 0xA1, 0,
		0x80, 0x20, 0xC0, 0,
		0, 0x60, 0x40, 0xFF, 0xA1, 0,
	}
	p := tcobs.NewDecoder(bytes.NewReader(enc), testBufferSize, true)
	dec := make([]byte, testBufferSize)
	n, e := p.Read(dec)
	if e != nil {
		assert.True(t, e == io.EOF)
	}
	exp := []byte{
		0, 0, 0, 0, 0, 0xAA, 0xFF, 0xFF, 0xAA,
		0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
		0, 0, 0, 0, 0, 0xFF,
	}
	assert.Equal(t, exp, dec[:n])
}

// TestMultiRead3PacketsWith1stAsError decodes 3 TCOBS packages with 0-delimiters and expects that Read returns last 2 packages decoded in one call, because 1st package is wrong.
func TestMultiRead3PacketsWith1stAsError(t *testing.T) {
	enc := []byte{
		0, 0x60, 0x40, 0xAA, 1, 2, 3, 0xC1, 0xAA, 0xA1, 0,
		0x80, 0x20, 0xC0, 0,
		0, 0x60, 0x40, 0xFF, 0xA1, 0,
	}
	p := tcobs.NewDecoder(bytes.NewReader(enc), testBufferSize, true)
	dec := make([]byte, testBufferSize)
	n, e := p.Read(dec)
	assert.False(t, e == nil)
	assert.False(t, e == io.EOF)
	exp := []byte{
		//0, 0, 0, 0, 0, 0xAA, 0xFF, 0xFF, 0xAA,
		0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
		0, 0, 0, 0, 0, 0xFF,
	}
	assert.Equal(t, exp, dec[:n])
}

// TestMultiRead3PacketsWithLastIncomplete decodes 3 TCOBS packages with 0-delimiters and expects that Read returns all packages decoded in two calls.
func TestMultiRead3PacketsWithLastIncomplete(t *testing.T) {
	enc := []byte{
		0, 0x60, 0x40, 0xAA, 0xC1, 0xAA, 0xA1, 0,
		0x80, 0x20, 0xC0, 0,
		0, 0x60, 0x40, 0xFF, 0xA1, 0,
	}
	p := tcobs.NewDecoder(bytes.NewReader(enc), 16, true)
	dec := make([]byte, testBufferSize)
	n, e := p.Read(dec)
	assert.True(t, e == nil)
	exp := []byte{
		0, 0, 0, 0, 0, 0xAA, 0xFF, 0xFF, 0xAA,
		0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
		//0, 0, 0, 0, 0, 0xFF,
	}
	assert.Equal(t, exp, dec[:n])

	n, e = p.Read(dec)
	if e != nil {
		assert.True(t, e == io.EOF)
	}
	exp = []byte{
		//0, 0, 0, 0, 0, 0xAA, 0xFF, 0xFF, 0xAA,
		//0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
		0, 0, 0, 0, 0, 0xFF,
	}
	assert.Equal(t, exp, dec[:n])
}
