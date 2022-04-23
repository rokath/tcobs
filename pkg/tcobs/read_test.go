package tcobs_test

import (
	"bytes"
	"io"
	"tcobs/pkg/tcobs"
	"testing"

	"github.com/stretchr/testify/assert"
)

const testBufferSize = 64 * 1024

// TestSingleRead concatenates encoded testData with 0-delimiters and expects that Read returns decoded single packages.
func TestSingleRead(t *testing.T) {
	enc := createEncodedSteam()
	p := tcobs.NewReader(bytes.NewReader(enc), testBufferSize, false)
	dec := make([]byte, testBufferSize)
	for _, x := range testData {
		n, e := p.Read(dec)
		assert.Nil(t, e)
		PrintAsGoCode(x.dec)
		PrintAsGoCode(dec[:n])
		assert.Equal(t, x.dec, dec[:n])
	}
}

// TestMultiRead concatenates encoded testData with 0-delimiters and expects that Read returns all packages decoded in one call.
func TestMultiRead(t *testing.T) {
	enc := createEncodedSteam()
	p := tcobs.NewReader(bytes.NewReader(enc), testBufferSize, true)
	dec := make([]byte, testBufferSize)
	n, e := p.Read(dec)
	if e != nil {
		assert.True(t, e == io.EOF)
	}
	exp := createDecodedSteam()
	assert.Equal(t, exp, dec[:n])
}

func createEncodedSteam() []byte {
	enc := make([]byte, 0, testBufferSize)
	for _, k := range testData {
		enc = append(enc, k.enc...)
		enc = append(enc, 0)
	}
	return enc
}

func createDecodedSteam() []byte {
	dec := make([]byte, 0, testBufferSize)
	for _, k := range testData {
		dec = append(dec, k.dec...)
	}
	return dec
}
