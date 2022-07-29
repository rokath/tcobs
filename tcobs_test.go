// Package tcobs_test contains TCOBS whitebox tests.
package tcobs_test

import (
	"bytes"
	"fmt"
	"math/rand"
	"testing"

	"github.com/rokath/tcobs"
	"github.com/tj/assert"
)

var max int = 10 // 32768

// TestCEncodeDecode12 tests on generated random byte numbers 0xFF, 0x00, 0x01 and 0x02 for random length 0-32767.
func TestRandom12CEncodeDecode(t *testing.T) {
	datBuf := make([]byte, max)

	for i := 0; i < 10000; i++ {
		length := rand.Intn(max)
		for i := 0; i < length; i++ {
			b := uint8(rand.Intn(4)) - 1 // -1, 0, 1 2
			datBuf[i] = b
		}
		dat := datBuf[:length]
		checkSet(t, i, dat)
	}
}

// TestCEncodeDecode1 tests on generated random byte numbers 0xFF, 0x00 and 0x01 for random length 0-32767.
func TestEncodeDecode1(t *testing.T) {
	datBuf := make([]byte, max)
	for i := 0; i < 10000; i++ {
		length := rand.Intn(max)
		for i := 0; i < length; i++ {
			b := uint8(rand.Intn(3)) - 1 // -1, 0, 1
			datBuf[i] = b
		}
		dat := datBuf[:length]
		checkSet(t, i, dat)
	}
}

// TestCEncodeDecode256 tests on generated random byte numbers for random length 0-32767.
func TestCEncodeDecode256(t *testing.T) {
	datBuf := make([]byte, max)
	for i := 0; i < 10000; i++ {
		length := rand.Intn(max)
		for i := 0; i < length; i++ {
			b := uint8(rand.Intn(256)) // 0, ..., 0xFF
			datBuf[i] = b
		}
		dat := datBuf[:length]
		checkSet(t, i, dat)
	}
}

func checkSet(t *testing.T, index int, dat []byte) {
	encBuf := make([]byte, 2*len(dat)) // max 1 + (1+1/32)*len) ~= 1.04 * len
	decBuf := make([]byte, 2*len(dat)) // max 1 + (1+1/32)*len) ~= 1.04 * len
	fmt.Println(index, "dat:", len(dat), dat)

	en := tcobs.CEncode(encBuf, dat)
	assert.True(t, en >= 0)
	enc := encBuf[:en]
	fmt.Println(index, "enc:", en, enc)
	assert.False(t, bytes.Contains(enc, []byte{0}))

	dn := tcobs.CDecode(decBuf, enc)
	if dn < 0 {
		fmt.Println(dn)
	}
	if dn != len(dat) {
		fmt.Println(index, dn)
	}
	assert.True(t, dn == len(dat))
	dec := decBuf[len(decBuf)-dn:]
	fmt.Println(index, "dec:", dn, dec)
	assert.Equal(t, dat, dec)
}
