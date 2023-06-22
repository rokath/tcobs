# TCOBS v1 & v2

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>

<!-- vscode-markdown-toc -->
- [TCOBS v1 \& v2](#tcobs-v1--v2)
  - [1. About The project](#1-about-the-project)
    - [1.1. Assumptions](#11-assumptions)
  - [2.  Preface](#2--preface)
    - [2.1.  Why not in 2 steps?](#21--why-not-in-2-steps)
  - [3. Data Disruption Handling](#3-data-disruption-handling)
  - [4. Current State](#4-current-state)
  - [5. Use cases](#5-use-cases)
  - [6. TCOBS Specification](#6-tcobs-specification)
    - [6.1. TCOBSv1 Specification](#61-tcobsv1-specification)
    - [6.2. TCOBSv2 Specification](#62-tcobsv2-specification)
  - [7. Getting Started](#7-getting-started)
    - [7.1. TCOBSv1 Go only decode](#71-tcobsv1-go-only-decode)
    - [7.2. TCOBSv1 and TCOBSv2 Go with CGO encode and decode](#72-tcobsv1-and-tcobsv2-go-with-cgo-encode-and-decode)
    - [7.3. TCOBSv1 and TCOBSv2 `C` encode and decode](#73-tcobsv1-and-tcobsv2-c-encode-and-decode)
  - [8. Roadmap](#8-roadmap)
  - [9. Future improvements?](#9-future-improvements)
  - [10. Contributing](#10-contributing)
  - [11. License](#11-license)
  - [12. Contact](#12-contact)
  - [13. Acknowledgments](#13-acknowledgments)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

<div id="top"></div></ol></details>

<!--

🟢✅🟡⛔🔴🔵💧❓↩෴⚓🛑❗🌡⏱∑✳‼♦♣🚫⚠🎥📷🌊🆘🧷🐢➡☕

![GitHub Workflow Status](https://img.shields.io/github/workflow/status/rokath/tcobs/goreleaser)
![GitHub All Releases](https://img.shields.io/github/downloads/rokath/tcobs/total)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/rokath/tcobs)
![GitHub commits since latest release](https://img.shields.io/github/commits-since/rokath/tcobs/latest)
-->

![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/rokath/cobs)
![GitHub watchers](https://img.shields.io/github/watchers/rokath/tcobs?label=watch)
[![Go Report Card](https://goreportcard.com/badge/github.com/rokath/tcobs)](https://goreportcard.com/report/github.com/rokath/tcobs)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![test](https://github.com/shogo82148/actions-goveralls/workflows/test/badge.svg?branch=main)](https://coveralls.io/github/rokath/tcobs)
[![Coverage Status](https://coveralls.io/repos/github/rokath/tcobs/badge.svg?branch=master)](https://coveralls.io/github/rokath/tcobs?branch=master)
![GitHub issues](https://img.shields.io/github/issues/rokath/tcobs)

<!-- ABOUT THE PROJECT -->
##  1. <a name='AboutTheproject'></a>About The project

![./docs/ref/COBSDataDisruption.svg](./docs/ref/COBSDataDisruption.svg)

* TCOBS is a variant of [COBS](https://github.com/rokath/cobs) combined with real-time [RLE](https://en.wikipedia.org/wiki/Run-length_encoding) data compression especially for short messages containing integers.
* The **consistent overhead** with TCOBS is 1 byte for each starting 31 bytes in the worst case, when no compression is possible. (*Example: A 1000 bytes buffer can be encoded with max 33 additional bytes.*) This is more compared to the original COBS with +1 byte for each starting 254 bytes, but if the data contain integer numbers, as communication packets often do, the encoded data will be statistically shorter with TCOBS compared to the legacy COBS.

###  1.1. <a name='Assumptions'></a>Assumptions

* Most messages like [*Trices*](https://github.com/rokath/trice) consist of 16 or less bytes.
* Some messages or user data are longer.
* Several zeros in a row are a common pattern (example:`00 00 00 05`).
* Several 0xFF in a row are a common pattern too (example -1 as 32 bit value).
* Maybe some other bytes appear also in a row.
* TCOBS does not know the inner data structure and is therefore **usable on any user data**.

<p align="right">(<a href="#top">back to top</a>)</p>

##  2. <a name='Preface'></a> Preface

* TCOBS was originally developed as an optional [*Trice*](https://github.com/rokath/trice) part and that's the **T** is standing for. It aims to reduce the binary [trice](https://github.com/rokath/trice) data together with framing in one step.
  * T symbols also the joining of the 2 orthogonal tasks compression and framing.
  * Additionally, the usage of ternary and quaternary numbers in TCOBSv2 is reflected in the letter T.
* TCOBSv2 is a better approach for TCOBSv1, suited perfect when long sequences of equal characters occur in the data stream.
  * The TCOBSv1 compression is expected to be not that good as with TCOBSv2.
* About the data is assumed, that 00-bytes and FF-bytes occur a bit more often than other bytes.
* The compression aim is more to get a reasonable data reduction with minimal computing effort, than reducing to an absolute minimum. The method shown here simply counts repeated bytes and transforms them into shorter sequences. It works well also on very short messages, like 2 or 4 bytes and on very long buffers. The compressed buffer contains no 00-bytes anymore what is the aim of COBS. <!-- In the worst case, if no repeated bytes occur at all, the encoded data can be about 3% longer (1 byte per each 31 input bytes). -->
* **TCOBS is stand-alone usable in any project for package framing with data minimizing.**
* Use cases in mind are speed, limited bandwidth and long time data recording in the field.
* TCOBS is inspired by [rlercobs](https://docs.rs/kolben/0.0.3/kolben/rlercobs/index.html). The ending sigil byte idea comes from [rCOBS](https://github.com/Dirbaio/rcobs). It allows a straight forward encoding avoiding lookahead and makes this way the embedded device code simpler.
* TCOBS uses various chained sigil bytes to achieve an additional lossless compression if possible.
* Each encoded package ends with a sigil byte.
* `0` is usable as delimiter byte between the packages containing no `0` anymore. It is up to the user to insert the **optional** delimiters for framing after each or several packages.

###  2.1. <a name='Whynotin2steps'></a> Why not in 2 steps?

* Usually it is better to divide this task and do compression and COBS encoding separately. This is good if size and time do not really matter. 
* The for TCOBS expected messages are typically in the range of 2 to 300 bytes, but not limited, and a run-length encoding then makes sense for real-time compression.
* Separating compression and COBS costs more time (2 processing loops) and does not allow to squeeze out the last byte.
* With the TCOBS algorithm, in only one processing loop a smaller transfer packet size is expected, combined with more speed.

<p align="right">(<a href="#top">back to top</a>)</p>

##  3. <a name='DataDisruptionHandling'></a>Data Disruption Handling
   
* In case of data disruption, the receiver will wait for the next 0-delimiter byte. As a result it will get a packet start and end of 2 different packages A and Z.

  <a href="https://github.com/rokath/tcobs">
    <img src="./docs/ref/COBSDataDisruption.svg" alt="Logo" width="1200" height="120">
  </a>

* For the decoder it makes no difference if the packages starts or ends with a sigil byte. In any case it will run into issues in such case with high probability and report a data disruption. But a false match is not excluded for 100%.
  * If the decoded data are structured, one can estimate the false match probability and increase the safety with an additional package CRC before encoding, if needed.
* The receiver calls continuously a `Read()` function. The received buffer can contain 0-delimited packages and the receiver assumes them all to be valid because there is no known significant time delay between package start and end.
* If a package start was received and the next package end reception is more than ~100ms away, a data disruption is likely and the receiver should ignore these data.
  * Specify a maximum inter-byte delay inside a single package like ~50ms for example.
  * To minimize the loss in case of data disruption, each message should get TCOBS encoded and 0-byte delimited separately.
  * The more often 0-byte delimiters are increasing the transmit overhead a bit on the other hand. 
* Of course, when the receiver starts, the first buffer can contain broken TCOBS data, but we have to live with that on a PC. Anyway there is a reasonable likelihood that a data inconsistency is detected as explained.

<p align="right">(<a href="#top">back to top</a>)</p>

##  4. <a name='CurrentState'></a>Current State

* [x] The TCOBSv1 & TCOBSv2 code is stable and ready to use without limitations.

|       Property                                                              | TCOBSv1         | TCOBSv2 |
| -                                                                           | -               | - |
| Code amount                                                                 | 🟢 less         | 🟡 more |
| Speed assumption (not measured yet)                                         | 🟢 faster       | 🟢 fast |
| Compression on short messages from 2 bytes length                           | 🟢 yes          | 🟢 yes |
| Compression on messages with many equal bytes in a row                      | 🟡 good         | 🟢 better |
| Encoding **C** language support                                             | 🟢 yes          | 🟢 yes |
| Decoding **C** language support                                             | 🟢 yes          | 🟢 yes |
| Encoding **Go** language support                                            | 🟡 yes with CGO | 🟡 yes with CGO |
| Decoding **Go** language support                                            | 🟢 yes          | 🟡 yes with CGO |
| Other language support                                                      | 🆘 No           | 🆘 No |

<p align="right">(<a href="#top">back to top</a>)</p>

##  5. <a name='Usecases'></a>Use cases

* Compression is a wide field and there is a lot of excellent code around.
* But when it comes to **very short messages like up to 100 bytes** these algorithms fail for one of two reasons:
  * They rely on a case specific runtime generated dictionary, which must be packed into the compressed data as well.
  * They rely on a common dictionary on encoder and decoder side which then is not needed to be a part of the compressed data. An interesting example is [SMAZ](https://github.com/antirez/smaz). But this method is not usable on arbitrary data.
* If your packages contain many integers, they have statistically more 0xFF and 0x00 bytes: ✅ that is TCOBS is made for.
* If your packages contain many equal bytes in a row: ✅ that is TCOBS is made for.
* If your packages contain statistically mixed byte sequences, like encrypted data: 🛑 that is TCOBS is **NOT** made for. Such data you frame better simply with COBS, even it is possible with TCOBS. A compression may make sense before the encryption.

<p align="right">(<a href="#top">back to top</a>)</p>

##  6. <a name='TCOBSSpecification'></a>TCOBS Specification

###  6.1. <a name='TCOBSv1Specification'></a>TCOBSv1 Specification

↩ See [./docs/TCOBSv1Specification.md](./docs/TCOBSv1Specification.md).

###  6.2. <a name='TCOBSv2Specification'></a>TCOBSv2 Specification

↩ See [./docs/TCOBSv2Specification.md](./docs/TCOBSv2Specification.md).

<p align="right">(<a href="#top">back to top</a>)</p>

##  7. <a name='GettingStarted'></a>Getting Started

###  7.1. <a name='TCOBSv1Goonlydecode'></a>TCOBSv1 Go only decode

* Add `import "github.com/rokath/tcobs/v1"` to your go source file.
  * Use function `tcobs.Decode` OR
  * use function `tcobs.NewDecoder` and then method `Read`. See `read_test.go` for an example.

###  7.2. <a name='TCOBSv1andTCOBSv2GowithCGOencodeanddecode'></a>TCOBSv1 and TCOBSv2 Go with CGO encode and decode

* Add `import "tcobs github.com/rokath/tcobs/Cv1"` or `import "tcobs github.com/rokath/tcobs/Cv2"` to your go source file.
  * Use functions `tcobs.CDecode` and `tcobs.CEncode` OR
  * use functions `tcobs.NewDecoder` and `tcobs.NewEncoder` and then methods `Read` and `Write`. See `read_test.go` and `write_test.go` for an example.

###  7.3. <a name='TCOBSv1andTCOBSv2Cencodeanddecode'></a>TCOBSv1 and TCOBSv2 `C` encode and decode

* Include the Cv1 or Cv2 C sources in your C project. Check `tcobsTest.c` for usage example.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- ROADMAP -->
##  8. <a name='Roadmap'></a>Roadmap

* [x] Add Changelog
* [x] Add back to top links
* [x] Add **Go** Reader & Writer interface
* [x] Add generic CCTN & CCQN conversions to remove TCOBSv2 limitations.
* [x] Improve testing with groups of equal bytes.
* [ ] Add fuzzing testing.
* [ ] Compare efficiency TCOBSv2 with TCOBSv1.

See the [open issues](https://github.com/rokath/tcobs/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#top">back to top</a>)</p>

##  9. <a name='Futureimprovements'></a>Future improvements?

❓ One could think that arbitrary byte buffer examples could be analyzed concerning the statistical usage of bytes and find that 0xFC...0xFF and 0x00...0x20 are used more often than 0xBD for example. This would allow to code some bytes with 5 bits and others with 11 bits creating a universal table, like huffman encoding. This table than is commonly used and the need to pack it into the compressed buffer disappears. Maybe some 2-byte sequences get also in this table and the table code could get enhanced with run-length codes.

❓ Several such "universal" tables are thinkable and during compression the encoder decides which "universal" table fits best for a specific short buffer. Then the table index must get into the compressed data.

❗ Because these "universal" tables then must reside together with the encoder and the decoder, this will increase the needed code space significantly. Alternatively these tables reside accessible outside the embedded device.

<p align="right">(<a href="#top">back to top</a>)</p>

##  10. <a name='Contributing'></a>Contributing

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
##  11. <a name='License'></a>License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTACT -->
##  12. <a name='Contact'></a>Contact

Thomas Höhenleitner - <!-- [@twitter_handle](https://twitter.com/twitter_handle) - --> th@seerose.net
Project Link: [https://github.com/rokath/tcobs](https://github.com/rokath/tcobs)

<!-- ACKNOWLEDGMENTS -->
##  13. <a name='Acknowledgments'></a>Acknowledgments

* [COBS](https://pypi.org/project/cobs/)
* [rCOBS](https://github.com/Dirbaio/rcobs)
* [rlercobs](https://docs.rs/kolben/0.0.3/kolben/rlercobs/index.html)

## Maybe Interesting Too

* https://github.com/lemire/streamvbyte
* https://github.com/kiyo-masui/bitshuffle

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
    <img src="./docs/ref/COBSDataDisruption.svg" alt="Logo" width="800" height="80">
  </a>

<h3 align="center">TCOBS</h3>

  <p align="center">
    Common Object Byte Stuffing with optimized Run-Length Encoding 
    <br />
    <a href="https://github.com/rokath/tcobs/blob/master/docs"><strong>Explore docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/rokath/tcobs/tree/master/v1">v1 Code</a>
    ·
    <a href="https://github.com/rokath/tcobs/tree/master/Cv1">Cv1 Code</a>
    ·
    <a href="https://github.com/rokath/tcobs/tree/master/Cv2">Cv2 Code</a>
    ·
    <a href="https://github.com/rokath/tcobs/issues">Report Bug / Request Feature</a>
  </p>
</div>
