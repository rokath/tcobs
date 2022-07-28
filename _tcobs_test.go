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
