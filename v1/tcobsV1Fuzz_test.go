package tcobsv1_test

import (
	"testing"

	tcobsv1 "github.com/rokath/tcobs/v1"

	"github.com/stretchr/testify/assert"
)

// FuzzCC tests the TCOBSv1 C-code encoding and decoding
//
// To run this test:  go test -run FuzzCC github.com/rokath/tcobs/v1 -fuzz=FuzzCC
// See also
// https://stackoverflow.com/questions/71584005/how-to-run-multi-fuzz-test-cases-wirtten-in-one-source-file-with-go1-18
func FuzzCC(f *testing.F) {
	for _, tc := range testData {
		f.Add(tc.dec)
	}
	f.Fuzz(func(t *testing.T, dec []byte) {
		act := make([]byte, 1000000)
		n := tcobsv1.CEncode(act, dec)
		actual := act[:n]

		decoded := make([]byte, 1000000)
		n = tcobsv1.CDecode(decoded, actual)
		actual = decoded[len(decoded)-n:]
		assert.Equal(t, dec, actual)
	})
}

// FuzzCGo tests the TCOBSv1 C-code encoding with Go decoding
// To run this test:  go test -run FuzzCGo github.com/rokath/tcobs/v1 -fuzz=FuzzCGo
// See also
// https://stackoverflow.com/questions/71584005/how-to-run-multi-fuzz-test-cases-wirtten-in-one-source-file-with-go1-18
func FuzzCGo(f *testing.F) {
	for _, tc := range testData {
		f.Add(tc.dec)
	}
	f.Fuzz(func(t *testing.T, dec []byte) {
		act := make([]byte, 1000000)
		n := tcobsv1.CEncode(act, dec)
		actual := act[:n]

		decoded := make([]byte, 1000000)
		n, err := tcobsv1.Decode(decoded, actual)
		assert.Nil(t, err)
		actual = decoded[len(decoded)-n:]
		assert.Equal(t, dec, actual)
	})
}
