// Package tcobs is also a helper for testing the target C-code.
// Each C function gets a Go wrapper which ist tested in appropriate test functions.
// For some reason inside the tcobs_test.go an 'import "C"' is not possible.
// It is possible to use solo the functions tcobs.Decode or tcobs.CEncode.
// For encoding create a Encoder and use the Write method as shown in TestWrite.
// For decoding create a Decoder and use the Read method as shown in TestSingleRead and TestMultiRead.
package tcobs
