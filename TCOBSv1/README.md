# TCOBSv1

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>

<!-- vscode-markdown-toc -->
* 1. [TCOBSv1 Specification](#TCOBSv1Specification)
* 2. [TCOBS Framing Encoder and Decoder](#TCOBSFramingEncoderandDecoder)
	* 2.1. [TCOBS Encoding](#TCOBSEncoding)
	* 2.2. [TCOBS Decoding](#TCOBSDecoding)
	* 2.3. [TCOBS Testing](#TCOBSTesting)
* 3. [Getting Started](#GettingStarted)
	* 3.1. [Prerequisites](#Prerequisites)
	* 3.2. [Installation](#Installation)
	* 3.3. [Usage in Go](#UsageinGo)
		* 3.3.1. [Decoding](#Decoding)
		* 3.3.2. [Encoding](#Encoding)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc --><div id="top"></div>

  </ol>
</details>

<p align="right">(<a href="#top">back to top</a>)</p>

##  1. <a name='TCOBSv1Specification'></a>TCOBSv1 Specification

* See [./docs/TCOBSv1Specification.md](./docs/TCOBSv1Specification.md).

<p align="right">(<a href="#top">back to top</a>)</p>

##  2. <a name='TCOBSFramingEncoderandDecoder'></a>TCOBS Framing Encoder and Decoder

###  2.1. <a name='TCOBSEncoding'></a>TCOBS Encoding

* [x] `tcobs.h` and `tcobs.c` contain the encoder as **C**-code.
* [x] The encoding in **Go** is possible with `tcobs.go` using `tcobs.c` with CGO.
* [x] The **Go** idiomatic usage is to use a `NewEncoder(w io.Writer, size int) (p *encoder)` and its *Reader* interface (see `read.go`)

###  2.2. <a name='TCOBSDecoding'></a>TCOBS Decoding

* [x] `tcobs.go` contains the decoder as **Go**-code.
* [ ] The decoding in **C** is not implemented (yet).
* [x] The **Go** idiomatic usage is to use a `NewDecoder(r io.Reader, size int, multi bool) (p *decoder)` and its *Writer* interface (see `write.go`)

###  2.3. <a name='TCOBSTesting'></a>TCOBS Testing

- [x] `tcobs_test.go` contains test code. CGO is not supported inside test files but usable through **Go** functions. 
  - Testing: `go test ./...`: ![./docs/ref/Test.PNG](./docs/ref/Test.PNG)
- The test execution can take several seconds. If your computer is slow you can reduce the loop count in the test functions `TestEncodeDecode*`.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->

##  3. <a name='GettingStarted'></a>Getting Started

* Add [./tcobs.c](./tcobs.c) to your embedded project and use function `TCOBSEncode` to convert a buffer into TCOBS format.
* After transmitting one (or more) TCOBS package(s) transmit a 0-delimiter byte.
* Decoding is currently implemented in **Go**.
  * Encoder and Decoder in other languages are easy to implement using the TCOBS specification and the given **C**- and **Go**-code.
* Contributions are appreciated.

###  3.1. <a name='Prerequisites'></a>Prerequisites

* Just a **C** compiler and, for testing, a **Go** installation.

###  3.2. <a name='Installation'></a>Installation

* To use TCOBS with **Go** execute `go get github.com/rokath/tcobs`

###  3.3. <a name='UsageinGo'></a>Usage in Go

* For example usage check the tests.

####  3.3.1. <a name='Decoding'></a>Decoding

* The function `tcobs.Decode` is usable standalone.
* Also it is possible to create a Decoder instance and use the `Read` method.

####  3.3.2. <a name='Encoding'></a>Encoding

* The function `tcobs.CEncode` is usable standalone.
* Also it is possible to create an Encoder instance and use the `Write` method.

<p align="right">(<a href="#top">back to top</a>)</p>
