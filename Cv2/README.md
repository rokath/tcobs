# TCOBSv2

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>

<!-- vscode-markdown-toc -->
- [TCOBSv2](#tcobsv2)
  - [1. TCOBSv2 Specification](#1-tcobsv2-specification)
  - [2. TCOBSv2 Framing Encoder and Decoder](#2-tcobsv2-framing-encoder-and-decoder)
    - [2.1. TCOBSv2 Encoding](#21-tcobsv2-encoding)
    - [2.2. TCOBSv2 Decoding](#22-tcobsv2-decoding)
    - [2.3. TCOBSv2 Testing](#23-tcobsv2-testing)
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

##  1. <a name='TCOBSv2Specification'></a>TCOBSv2 Specification

* See [../docs/TCOBSSpecification.md](../docs/TCOBSSpecification.md).

<p align="right">(<a href="#top">back to top</a>)</p>

##  2. <a name='TCOBSv2FramingEncoderandDecoder'></a>TCOBSv2 Framing Encoder and Decoder

###  2.1. <a name='TCOBSv2Encoding'></a>TCOBSv2 Encoding

* [x] `tcobs.h` and `tcobsEncode.c` with `tcobs.Internal.h` contain the encoder as **C**-code.
* [x] The encoding in **Go** is possible with `tcobs.go` using the encoder C-code with CGO.
* [x] The **Go** idiomatic usage is to use a `NewEncoder(w io.Writer, size int) (p *encoder)` and its *Reader* interface (see `read.go`)

###  2.2. <a name='TCOBSv2Decoding'></a>TCOBSv2 Decoding

* [x] `tcobs.h` and `tcobDecode.c` with `tcobs.Internal.h` contain the encoder as **C**-code.
* [x] The decoding in **Go** is possible with `tcobs.go` using the decoder C-code with CGO.
* [x] The **Go** idiomatic usage is to use a `NewDecoder(r io.Reader, size int, multi bool) (p *decoder)` and its *Writer* interface (see `write.go`)

###  2.3. <a name='TCOBSv2Testing'></a>TCOBSv2 Testing

* [x] `tcobs_test.go` contains test code. CGO is not supported inside test files but usable through **Go** functions.
  * Testing: `go test ./...`: ![../docs/ref/Test.PNG](../docs/ref/Test.PNG) executes the C-code with many test data.
* The test execution can take several seconds. If your computer is slow you can reduce the loop count in the test functions `TestEncodeDecode*`.
* The file `tcobsTest.c` contains also some test data but is intended only for debugging.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->

##  3. <a name='GettingStarted'></a>Getting Started

* Add [./tcobsEncode.c](./tcobsEncode.c) and/or [./tcobsDecode.c](./tcobsDecode.c)  to your embedded project and use function `TCOBSEncode` and/or `TCOBSDecode` to convert a buffer to/from TCOBS format.
* After transmitting one (or more) TCOBS package(s) transmit a 0-delimiter byte.
* Encoding and Decoding is currently implemented in **Go** using the C-code with CGO.
* Encoder and Decoder in other languages are implementable using the TCOBS specification and the given **C**- and **Go**-code.
* Contributions are appreciated.

###  3.1. <a name='Prerequisites'></a>Prerequisites

* Just a **C** compiler and, for testing, a **Go** installation.

###  3.2. <a name='Installation'></a>Installation

* To use TCOBSv2 with **Go** execute `go get github.com/rokath/tcobs/TCOBSv2`

###  3.3. <a name='UsageinGo'></a>Usage in Go

* For example usage check the tests.

####  3.3.1. <a name='Decoding'></a>Decoding

* The function `tcobs.CDecode` is usable standalone.
* Also it is possible to create a Decoder instance and use the `Read` method.

####  3.3.2. <a name='Encoding'></a>Encoding

* The function `tcobs.CEncode` is usable standalone.
* Also it is possible to create an Encoder instance and use the `Write` method.

<p align="right">(<a href="#top">back to top</a>)</p>
