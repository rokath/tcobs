# TCOBS v1 & v2

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>

<!-- vscode-markdown-toc -->
* 1. [ATTENTION](#ATTENTION)
* 2. [About The project](#AboutTheproject)
* 3. [ Preface](#Preface)
	* 3.1. [ Why not in 2 steps?](#Whynotin2steps)
* 4. [COBS Data Disruption](#COBSDataDisruption)
* 5. [TCOBS Specification](#TCOBSSpecification)
	* 5.1. [TCOBSv1 Specification](#TCOBSv1Specification)
	* 5.2. [TCOBSv2 Specification](#TCOBSv2Specification)
* 6. [TCOBS Framing Encoder and Decoder](#TCOBSFramingEncoderandDecoder)
	* 6.1. [TCOBS Encoding](#TCOBSEncoding)
	* 6.2. [TCOBS Decoding](#TCOBSDecoding)
	* 6.3. [TCOBS Testing](#TCOBSTesting)
* 7. [Getting Started](#GettingStarted)
	* 7.1. [Prerequisites](#Prerequisites)
	* 7.2. [Installation](#Installation)
	* 7.3. [Usage in Go](#UsageinGo)
		* 7.3.1. [Decoding](#Decoding)
		* 7.3.2. [Encoding](#Encoding)
* 8. [Roadmap](#Roadmap)
* 9. [Contributing](#Contributing)
* 10. [License](#License)
* 11. [Contact](#Contact)
	* 11.1. [Acknowledgments](#Acknowledgments)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc --><div id="top"></div>

  </ol>
</details>

<!--
![GitHub Workflow Status](https://img.shields.io/github/workflow/status/rokath/tcobs/goreleaser)
![GitHub All Releases](https://img.shields.io/github/downloads/rokath/tcobs/total)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/rokath/tcobs)
![GitHub commits since latest release](https://img.shields.io/github/commits-since/rokath/tcobs/latest)
-->

![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/rokath/tcobs)
![GitHub watchers](https://img.shields.io/github/watchers/rokath/tcobs?label=watch)
[![Go Report Card](https://goreportcard.com/badge/github.com/rokath/tcobs)](https://goreportcard.com/report/github.com/rokath/tcobs)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![test](https://github.com/shogo82148/actions-goveralls/workflows/test/badge.svg?branch=main)](https://coveralls.io/github/rokath/tcobs)
[![Coverage Status](https://coveralls.io/repos/github/rokath/tcobs/badge.svg?branch=master)](https://coveralls.io/github/rokath/tcobs?branch=master)
![GitHub issues](https://img.shields.io/github/issues/rokath/tcobs)

##  1. <a name='ATTENTION'></a>ATTENTION

* The current v1 code is stable and ready to use.
* The current v2 code is stable and ready to use for packages not containing 14 or more equal bytes in a row.
  * This will change very soon.

<!-- ABOUT THE PROJECT -->
##  2. <a name='AboutTheproject'></a>About The project

![./TCOBSv1/docs/ref/COBSDataDisruption.svg](./TCOBSv1/docs/ref/COBSDataDisruption.svg)

* TCOBS is a variant of [COBS](https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing) combined with real-time [RLE](https://en.wikipedia.org/wiki/Run-length_encoding) data compression especially for short messages containing integers.
* The **consistent overhead** with TCOBS is 1 byte for each starting 31 bytes in the worst case, when no compression is possible. (*Example: A 1000 bytes buffer can be encoded with max 33 additional bytes.*) This is more compared to the original COBS with +1 byte for each starting 254 bytes but if the data contain integer numbers, as communication packets often do, the encoded data will be statistically shorter with TCOBS compared to the legacy COBS.

<p align="right">(<a href="#top">back to top</a>)</p>

##  3. <a name='Preface'></a> Preface

* TCOBS was originally developed as an optional [*Trice*](https://github.com/rokath/trice) part and that's the **T** is standing for. It aims to reduce the binary [trice](https://github.com/rokath/trice) data together with framing in one step.
  * T symbols also the joining of the 2 orthogonal tasks compression and framing.
  * Additionally the usage of ternary and quaternary numbers is reflected in the letter T.
* TCOBSv2 is a better approach for TCOBSv1, suited also when long sequences of equal characters occur in the data stream.
  * The TCOBSv1 code is simpler and therefore smaller. The compression is probably not that good as with TCOBS v2.
* About the data is assumed, that 00-bytes and FF-bytes occur a bit more often than other bytes.
* The aim concerning the compression is more to get a reasonable data reduction in a cheap way concerning minimal computing effort, than reducing to an absolute minimum. The method shown here simply counts repeated bytes and transforms them into shorter sequences. It works well also on very short messages, like 4 bytes and on very long buffers. The compressed buffer contains no 00-bytes anymore what is the aim of COBS. <!-- In the worst case, if no repeated bytes occur at all, the encoded data can be about 3% longer (1 byte per each 31 input bytes). -->
* **TCOBS is stand-alone usable in any project for package framing with data minimizing.**
* Use cases in mind are speed, limited bandwidth and long time data recording in the field.
* TCOBS is a different kind of [COBS](https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing) package framing, inspired by [rlercobs](https://docs.rs/kolben/0.0.3/kolben/rlercobs/index.html) with focus on speed and minimizing size.
* TCOBS uses various chained sigil bytes to achieve an additional lossless compression if possible.
* Each encoded package ends with an additional sigil byte. <!-- and has in the worst case 1 additional byte per 31 bytes, but usually the encoded data are smaller than the unencoded because of the compression.-->
  * TCOBS encoding is inspired also by [rCOBS](https://github.com/Dirbaio/rcobs) (the ending sigil byte). It allows a straight forward encoding avoiding lookahead and makes this way the embedded device code simpler.
* `0` is used as delimiter byte between the packages containing no `0` anymore. It is up to the user to insert the **optional** delimiters for framing.

###  3.1. <a name='Whynotin2steps'></a> Why not in 2 steps?

* Usually it is better to divide this task and do compression and COBS encoding separately. This is good if size and time do not really matter. 
* The for TCOBS expected messages are typically in the range of 2 to 300 bytes, but not limited, and a run-length encoding then makes sense for real-time compression.
* Separating RLE and COBS costs more time (2 processing loops) and does not allow to squeeze out the last byte.
* With the TCOBS algorithm, in only one processing loop a smaller transfer packet size is expected, combined with more speed.

##  4. <a name='COBSDataDisruption'></a>COBS Data Disruption
  
* In case of data disruption, the receiver will wait for the next 0-delimiter byte. As a result it will get a packet start and end of 2 different packages A and Z.

<!-- ![COBSDataDisruption](./ref/COBSDataDisruption.svg) -->

* For the COBS decoder it makes no difference if the COBS packages start or end with a sigil byte. In any case it will run into issues in such case with high probability. 
  * An additional package CRC before encoding could be added to decrease false match probability.
* The receiver calls continuously a `Read()` function. The received buffer can contain 0-delimited COBS packages and the receiver assumes them all to be valid because there is no known significant time delay between package start and end.
* If a package start was received and the next package end reception is more than ~100ms away, a data disruption is likely and the receiver should ignore these data.
  * To minimise data loss, each message should get COBS encoded separately.
* Of course, when the receiver starts, the first buffer can contain broken COBS data, but we have to live with that on a PC. Anyway there is a reasonable likelihood that the COBS decoder will detect a data inconsistency.

##  5. <a name='TCOBSSpecification'></a>TCOBS Specification

###  5.1. <a name='TCOBSv1Specification'></a>TCOBSv1 Specification

* See [./TCOBSv1/docs/TCOBSSpecification.md](./TCOBSv1/docs/TCOBSv1Specification.md).

###  5.2. <a name='TCOBSv2Specification'></a>TCOBSv2 Specification

* See [./TCOBSv2/docs/TCOBSSpecification.md](./TCOBSv2/docs/TCOBSv2Specification.md).

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- 
##  6. <a name='TCOBSFramingEncoderandDecoder'></a>TCOBS Framing Encoder and Decoder

###  6.1. <a name='TCOBSEncoding'></a>TCOBS Encoding

* [x] `tcobs.h` and `tcobs.c` contain the encoder as **C**-code.
* [x] The encoding in **Go** is possible with `tcobs.go` using `tcobs.c` with CGO.
* [x] The **Go** idiomatic usage is to use a `NewEncoder(w io.Writer, size int) (p *encoder)` and its *Reader* interface (see `read.go`)

###  6.2. <a name='TCOBSDecoding'></a>TCOBS Decoding

* [x] `tcobs.go` contains the decoder as **Go**-code.
* [ ] The decoding in **C** is not implemented (yet).
* [x] The **Go** idiomatic usage is to use a `NewDecoder(r io.Reader, size int, multi bool) (p *decoder)` and its *Writer* interface (see `write.go`)

###  6.3. <a name='TCOBSTesting'></a>TCOBS Testing

- [x] `tcobs_test.go` contains test code. CGO is not supported inside test files but usable through **Go** functions. 
  - Testing: `go test ./...`: ![./docs/ref/Test.PNG](./docs/ref/Test.PNG)
- The test execution can take several seconds. If your computer is slow you can reduce the loop count in the test functions `TestEncodeDecode*`.

<p align="right">(<a href="#top">back to top</a>)</p>

-->
<!-- GETTING STARTED -->

##  7. <a name='GettingStarted'></a>Getting Started

* See README.md in TCOBSv1 or TCOBSv2.

<!--
* Add [./tcobs.c](./tcobs.c) to your embedded project and use function `TCOBSEncode` to convert a buffer into TCOBS format.
* After transmitting one (or more) TCOBS package(s) transmit a 0-delimiter byte.
* Decoding is currently implemented in **Go**.
  * Encoder and Decoder in other languages are easy to implement using the TCOBS specification and the given **C**- and **Go**-code.
* Contributions are appreciated.

###  7.1. <a name='Prerequisites'></a>Prerequisites

* Just a **C** compiler and, for testing, a **Go** installation.

###  7.2. <a name='Installation'></a>Installation

* To use TCOBS with **Go** execute `go get github.com/rokath/tcobs`

###  7.3. <a name='UsageinGo'></a>Usage in Go

* For example usage check the tests.

####  7.3.1. <a name='Decoding'></a>Decoding

* The function `tcobs.Decode` is usable standalone.
* Also it is possible to create a Decoder instance and use the `Read` method.

####  7.3.2. <a name='Encoding'></a>Encoding

* The function `tcobs.CEncode` is usable standalone.
* Also it is possible to create an Encoder instance and use the `Write` method.

<p align="right">(<a href="#top">back to top</a>)</p>

-->

<!-- ROADMAP -->
##  8. <a name='Roadmap'></a>Roadmap

* [x] Add Changelog
* [x] Add back to top links
* [x] Add **Go** Reader & Writer interface
* [ ] Add decode function in **C** in TCOBSv1
* [ ] Add Additional Templates w/ Examples

See the [open issues](https://github.com/rokath/tcobs/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTRIBUTING -->
##  9. <a name='Contributing'></a>Contributing

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
##  10. <a name='License'></a>License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTACT -->
##  11. <a name='Contact'></a>Contact

Thomas Höhenleitner - <!-- [@twitter_handle](https://twitter.com/twitter_handle) - --> th@seerose.net
Project Link: [https://github.com/rokath/tcobs](https://github.com/rokath/tcobs)

<!-- ACKNOWLEDGMENTS -->
###  11.1. <a name='Acknowledgments'></a>Acknowledgments

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
    <img src="docs/ref/COBSDataDisruption.svg" alt="Logo" width="800" height="80">
  </a>

<h3 align="center">TCOBS</h3>

  <p align="center">
    Common Object Byte Stuffing with optimized Run-Length Encoding 
    <br />
    <a href="https://github.com/rokath/tcobs/blob/master/docs/TCOBSSpecification.md"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/rokath/tcobs/blob/master">View Code</a>
    ·
    <a href="https://github.com/rokath/tcobs/issues">Report Bug</a>
    ·
    <a href="https://github.com/rokath/tcobs/issues">Request Feature</a>
  </p>
</div>
