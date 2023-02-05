# TCOBSv1

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>

<!-- vscode-markdown-toc -->
- [TCOBSv1](#tcobsv1)
	- [1. TCOBSv1 Specification](#1-tcobsv1-specification)
	- [2. TCOBSv1 Framing Encoder and Decoder](#2-tcobsv1-framing-encoder-and-decoder)
		- [2.1. TCOBSv1 Encoding](#21-tcobsv1-encoding)
		- [2.2. TCOBSv1 Decoding](#22-tcobsv1-decoding)
		- [2.3. TCOBSv1 Testing](#23-tcobsv1-testing)
	- [3. Getting Started](#3-getting-started)
		- [3.1. Prerequisites](#31-prerequisites)
		- [3.2. Installation](#32-installation)
		- [3.3. Usage in Go](#33-usage-in-go)
			- [3.3.1. Decoding](#331-decoding)
			- [3.3.2. Encoding](#332-encoding)
<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc --><div id="top"></div>

  </ol>
</details>

<p align="right">(<a href="#top">back to top</a>)</p>

##  1. <a name='TCOBSv1Specification'></a>TCOBSv1 Specification

* See [../docs/TCOBSv1Specification.md](../docs/TCOBSv1Specification.md).

<p align="right">(<a href="#top">back to top</a>)</p>

##  2. <a name='TCOBSv1FramingEncoderandDecoder'></a>TCOBSv1 Framing Encoder and Decoder

###  2.1. <a name='TCOBSv1Encoding'></a>TCOBSv1 Encoding

* [x] `tcobs.h` and `tcobsEncode.c` contain the encoder as **C**-code.
* [x] The decoding in **Go** is possible with `tcobsCDecode.go` using `tcobsDecode.c` with CGO.
* [x] The decoding in **Go** is possible also with `tcobsDecode.go` and recommended.
* [x] The **Go** idiomatic usage is to use a `NewEncoder(w io.Writer, size int) (p *encoder)` and its *Reader* interface (see `read.go`)

###  2.2. <a name='TCOBSv1Decoding'></a>TCOBSv1 Decoding

* [x] `tcobsDecode.go` contains the decoder as **Go**-code.
* [x] `tcobs.h` and `tcobsDecode.c` contain the decoder as **C**-code.
* [x] The encoding in **Go** is possible with `tcobsCEncode.go` using `tcobsEncode.c` with CGO.
* [x] The **Go** idiomatic usage is to use a `NewDecoder(r io.Reader, size int, multi bool) (p *decoder)` and its *Writer* interface (see `write.go`)

###  2.3. <a name='TCOBSv1Testing'></a>TCOBSv1 Testing

- [x] `*_test.go` files contain test code. CGO is not supported inside test files but usable through **Go** functions. 
  - Testing: `go test ./...`: ![../docs/ref/Test.PNG](../docs/ref/Test.PNG)
- The test execution can take several seconds. If your computer is slow you can reduce the loop count in the test functions `TestEncodeDecode*`.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->

##  3. <a name='GettingStarted'></a>Getting Started

* Add [./tcobsEncode.c](./tcobsEncode.c) and/or [./tcobsDecode.c](./tcobsDecode.c) to your embedded project and use function `TCOBSEncode` and/or `TCOBSDecode`  to convert a buffer to/from TCOBS format.
* After transmitting one (or more) TCOBS package(s) transmit a 0-delimiter byte.
* Decoding is implemented also in **Go**.
  * Encoder and Decoder in other languages are easy to implement using the TCOBS specification and the given **C**- and **Go**-code.
* Contributions are appreciated.

###  3.1. <a name='Prerequisites'></a>Prerequisites

* Just a **C** compiler and, for testing, a **Go** installation.

###  3.2. <a name='Installation'></a>Installation

* To use TCOBSv1 with **Go** execute `go get github.com/rokath/tcobs/TCOBSv1`

###  3.3. <a name='UsageinGo'></a>Usage in Go

* For example usage check the tests.

####  3.3.1. <a name='Decoding'></a>Decoding

* The function `tcobs.Decode` is usable standalone.
* Also it is possible to create a Decoder instance and use the `Read` method.
* The function `tcobs.CDecode` is usable standalone also.

####  3.3.2. <a name='Encoding'></a>Encoding

* The function `tcobs.CEncode` is usable standalone.
* Also it is possible to create an Encoder instance and use the `Write` method.

<p align="right">(<a href="#top">back to top</a>)</p>
