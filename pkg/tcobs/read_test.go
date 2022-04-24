package tcobs_test

import (
	"bytes"
	"io"
	"tcobs/pkg/tcobs"
	"testing"

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
