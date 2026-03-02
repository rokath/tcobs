// Package tcobsv2 implements TCOBS v2 as documented in ../docs/TCOBSv2Specification.md.
//
// The package provides:
//   - streaming helpers via NewEncoder and NewDecoder
//   - C wrappers via CEncode and CDecode for the reference C implementation
//
// The C wrappers are mainly used for compatibility and testing.
package tcobsv2
