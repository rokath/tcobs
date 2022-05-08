// Package tcobs_test contains TCOBS whitebox tests.
package tcobs_test

import (
	"bytes"
	"fmt"
	"math/rand"
	"tcobs/pkg/tcobs"
	"testing"

	"github.com/tj/assert"
)

// TestTCOBSEncode checks if decoded testData lead to the matching encoded testData.
func TestCEncode(t *testing.T) {
	for _, k := range testData {
		enc := make([]byte, 40000)
		n := tcobs.CEncode(enc, k.dec)
		enc = enc[:n]
		assert.Equal(t, k.enc, enc)
	}
}

// TestTCOBSDecode checks if encoded testData lead to the matching unencoded testData.
func TestDecode(t *testing.T) {
	b := make([]byte, 40000)
	for _, k := range testData {
		n, e := tcobs.Decode(b, k.enc) // func TCOBSDecode(d, in []byte) (n int, e error) {
		assert.True(t, e == nil)
		assert.True(t, n <= len(b))
		dec := b[len(b)-n:]
		assert.Equal(t, k.dec, dec)
	}
}

// TestCEncodeDecode12 tests on generated random byte numbers 0xFF, 0x00, 0x01 and 0x02 for random length 0-32767.
func TestCEncodeDecode12(t *testing.T) {
	max := 32768
	for i := 0; i < 10000; i++ {
		length := rand.Intn(max)
		datBuf := make([]byte, max)
		encBuf := make([]byte, 2*max) // max 1 + (1+1/32)*len) ~= 1.04 * len
		decBuf := make([]byte, 2*max) // max 1 + (1+1/32)*len) ~= 1.04 * len
		for i := 0; i < length; i++ {
			b := uint8(rand.Intn(4)) - 1 // -1, 0, 1 2
			datBuf[i] = b
		}
		dat := datBuf[:length]
		n := tcobs.CEncode(encBuf, dat)
		enc := encBuf[:n]
		assert.False(t, bytes.Contains(enc, []byte{0}))
		n, e := tcobs.Decode(decBuf, enc)
		assert.True(t, e == nil)
		dec := decBuf[len(decBuf)-n:]
		assert.Equal(t, dat, dec)
	}
}

// TestCEncodeDecode1 tests on generated random byte numbers 0xFF, 0x00 and 0x01 for random length 0-32767.
func TestEncodeDecode1(t *testing.T) {
	max := 32768
	for i := 0; i < 10000; i++ {
		length := rand.Intn(max)
		datBuf := make([]byte, max)
		encBuf := make([]byte, 2*max) // max 1 + (1+1/32)*len) ~= 1.04 * len
		decBuf := make([]byte, 2*max) // max 1 + (1+1/32)*len) ~= 1.04 * len
		for i := 0; i < length; i++ {
			b := uint8(rand.Intn(3)) - 1 // -1, 0, 1
			datBuf[i] = b
		}
		dat := datBuf[:length]
		fmt.Println()
		n := tcobs.CEncode(encBuf, dat)
		enc := encBuf[:n]
		assert.False(t, bytes.Contains(enc, []byte{0}))
		n, e := tcobs.Decode(decBuf, enc)
		assert.True(t, e == nil)
		dec := decBuf[len(decBuf)-n:]
		assert.Equal(t, dat, dec)
	}
}

// TestCEncodeDecode256 tests on generated random byte numbers for random length 0-32767.
func TestCEncodeDecode256(t *testing.T) {
	max := 32768
	for i := 0; i < 10000; i++ {
		length := rand.Intn(max)
		datBuf := make([]byte, max)
		encBuf := make([]byte, 2*max) // max 1 + (1+1/32)*len) ~= 1.04 * len
		decBuf := make([]byte, 2*max) // max 1 + (1+1/32)*len) ~= 1.04 * len
		for i := 0; i < length; i++ {
			b := uint8(rand.Intn(256)) // 0, ..., 0xFF
			datBuf[i] = b
		}
		dat := datBuf[:length]
		fmt.Println()
		n := tcobs.CEncode(encBuf, dat)
		enc := encBuf[:n]
		assert.False(t, bytes.Contains(enc, []byte{0}))
		n, e := tcobs.Decode(decBuf, enc)
		assert.True(t, e == nil)
		dec := decBuf[len(decBuf)-n:]
		assert.Equal(t, dat, dec)
	}
}
