// Package tcobsv1 implements TCOBS v1 as documented in ../docs/TCOBSv1Specification.md.
//
// The package provides:
//   - streaming helpers via NewEncoder and NewDecoder
//   - direct decode in Go via Decode
//   - C wrappers via CEncode and CDecode for the reference C implementation
//
// The C wrappers are mainly used for compatibility and testing.
package tcobsv1
