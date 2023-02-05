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
  - [4. Roadmap](#4-roadmap)
  - [5. Contributing](#5-contributing)
  - [6. License](#6-license)
  - [7. Contact](#7-contact)
    - [7.1. Acknowledgments](#71-acknowledgments)

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

<!-- ROADMAP -->
##  4. <a name='Roadmap'></a>Roadmap

* [x] Add Changelog
* [x] Add back to top links
* [x] Add **Go** Reader & Writer interface
* [x] Add decode function in **C**
* [ ] Add Additional Templates w/ Examples

See the [open issues](https://github.com/rokath/tcobs/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTRIBUTING -->
##  5. <a name='Contributing'></a>Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

**Working on your first Pull Request?** You can learn how from this *free* series [How to Contribute to an Open Source Project on GitHub](https://kcd.im/pull-request) 

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- LICENSE -->
##  6. <a name='License'></a>License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTACT -->
##  7. <a name='Contact'></a>Contact

Thomas Höhenleitner - <!-- [@twitter_handle](https://twitter.com/twitter_handle) - --> th@seerose.net
Project Link: [https://github.com/rokath/tcobs](https://github.com/rokath/tcobs)

<!-- ACKNOWLEDGMENTS -->
###  7.1. <a name='Acknowledgments'></a>Acknowledgments

* [COBS](https://pypi.org/project/cobs/)
* [rCOBS](https://github.com/Dirbaio/rcobs)
* [rlercobs](https://docs.rs/kolben/0.0.3/kolben/rlercobs/index.html)

<!--
* [Choose an Open Source License](https://choosealicense.com)
* [GitHub Emoji Cheat Sheet](https://www.webpagefx.com/tools/emoji-cheat-sheet)
* [Malven's Flexbox Cheatsheet](https://flexbox.malven.co/)
* [Malven's Grid Cheatsheet](https://grid.malven.co/)
* [Img Shields](https://shields.io)
* [GitHub Pages](https://pages.github.com)
* [Font Awesome](https://fontawesome.com)
* [React Icons](https://react-icons.github.io/react-icons/search)
-->

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- MARKDOWN LINKS & IMAGES -->

<!--
https://www.markdownguide.org/basic-syntax/#reference-style-links -- >
[contributors-shield]: https://img.shields.io/github/contributors/othneildrew/Best-README-Template.svg?style=for-the-badge
[contributors-url]: https://github.com/othneildrew/Best-README-Template/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/othneildrew/Best-README-Template.svg?style=for-the-badge
[forks-url]: https://github.com/othneildrew/Best-README-Template/network/members
[stars-shield]: https://img.shields.io/github/stars/othneildrew/Best-README-Template.svg?style=for-the-badge
[stars-url]: https://github.com/othneildrew/Best-README-Template/stargazers
[issues-shield]: https://img.shields.io/github/issues/othneildrew/Best-README-Template.svg?style=for-the-badge
[issues-url]: https://github.com/othneildrew/Best-README-Template/issues
[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=for-the-badge
[license-url]: https://github.com/othneildrew/Best-README-Template/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/othneildrew
[product-screenshot]: images/screenshot.png
-->

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/rokath/tcobs">
    <img src="../docs/ref/COBSDataDisruption.svg" alt="Logo" width="800" height="80">
  </a>

<h3 align="center">TCOBS</h3>

  <p align="center">
    Common Object Byte Stuffing with optimized Run-Length Encoding 
    <br />
    <a href="https://github.com/rokath/tcobs/blob/master/docs"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/rokath/tcobs/blob/master">View Code</a>
    ·
    <a href="https://github.com/rokath/tcobs/issues">Report Bug</a>
    ·
    <a href="https://github.com/rokath/tcobs/issues">Request Feature</a>
  </p>
</div>
