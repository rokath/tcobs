// Package tcobs_test contains TCOBS whitebox tests.
package tcobs_test

import (
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

// TestCDecode checks if decoded testData lead to the matching encoded testData.
func TestCDecode(t *testing.T) {
	for _, k := range testData {
		dec := make([]byte, 40000)
		n := tcobs.CDecode(dec, k.enc)
		dec = dec[len(dec)-n:]
		assert.Equal(t, k.dec, dec)
	}
}
