package tcobs_test

import (
	"testing"

	"github.com/rokath/tcobs"
	"github.com/tj/assert"
)

// TestDecoder tests the repeated usage of Decoder.
func TestDecoder(t *testing.T) {
	i := []byte{0xAA, 0xBB, 0xA2, 0, 4, 0xA1, 0, 6}
	before, after, err := tcobs.Decoder(i) // panic
	assert.True(t, err == nil)
	assert.Equal(t, i[:2], before)
	assert.Equal(t, i[4:], after)

	before, after, err = tcobs.Decoder(after)
	assert.True(t, err == nil)
	assert.Equal(t, i[4:5], before)
	assert.Equal(t, i[7:], after)

	before, after, err = tcobs.Decoder(after)
	assert.True(t, err == nil)
	var exp []uint8
	assert.Equal(t, exp, before)
	assert.Equal(t, i[7:], after)
}
