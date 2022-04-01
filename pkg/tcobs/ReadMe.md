# TCOBS Framing Encoder and Decoder

- [ ] Encoder and Decoder in other languages are easy to implement using the TCOBS specification and the given **C**- and **Go**-code.

## TCOBS Encoding

- [x] `tcobs.h` and `tcobs.c` contain the encoder as **C**-code.
- [x] The encoding in **Go** is possible with `tcobs.c` using CGO.

## TCOBS Decoding

- [x] `tcobs.go` contains the decoder as **Go**-code.
- [ ] The decoding in **C** is not implemented (yet).

## TCOBS Testing

- [x] `tcobs_test.go` contains test code. CGO is not supported inside test files.
  - Testing `go test ./...`
