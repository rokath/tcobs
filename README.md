# TCOBS

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>

<!-- vscode-markdown-toc -->
* 1. [About The project](#AboutTheproject)
* 2. [TCOBS Specification](#TCOBSSpecification)
* 3. [TCOCS code](#TCOCScode)
* 4. [Getting Started](#GettingStarted)
	* 4.1. [Prerequisites](#Prerequisites)
	* 4.2. [Installation](#Installation)
	* 4.3. [Roadmap](#Roadmap)
* 5. [Contributing](#Contributing)
* 6. [License](#License)
* 7. [Contact](#Contact)
	* 7.1. [Acknowledgments](#Acknowledgments)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc --><div id="top"></div>

  </ol>
</details>

<!-- ABOUT THE PROJECT -->
##  1. <a name='AboutTheproject'></a>About The project

![./docs/ref/COBSDataDisruption.svg](./docs/ref/COBSDataDisruption.svg)

TCOBS is a variant of [COBS](https://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing) combined with real-time [RLE](https://en.wikipedia.org/wiki/Run-length_encoding) data compression especially for short messages containing integers. 

##  2. <a name='TCOBSSpecification'></a>TCOBS Specification

* See [./docs/TCOBSSpecification.md](./docs/TCOBSSpecification.md).

<p align="right">(<a href="#top">back to top</a>)</p>

##  3. <a name='TCOCScode'></a>TCOCS code

* See [./pkg/tcobs/](./pkg/tcobs/) folder

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->
##  4. <a name='GettingStarted'></a>Getting Started

* Add [./pkg/tcobs/tcobs.c](./pkg/tcobs/tcobs.c) to your embedded project and use function `TCOBSEncode` to convert a buffer into TCOBS format.
* After transmitting one (or more) TCOBS package(s) transmit a 0-delimiter byte.
* Decoding is currently implemented only in **Go**, but its is no big deal to write a decoder in **C** or an other language of your choice using the documentation and the easy readable **Go** code.
* Contributions are appreciated.

<p align="right">(<a href="#top">back to top</a>)</p>

###  4.1. <a name='Prerequisites'></a>Prer⁷equisites

* Just a **C** compiler and, for testing, a **Go** installation.

<p align="right">(<a href="#top">back to top</a>)</p>

###  4.2. <a name='Installation'></a>Installation

* To use TCOBS with **Go** execute `go get github.com/rokath/tcobs`

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- ROADMAP -->
###  4.3. <a name='Roadmap'></a>Roadmap

* [x] Add Changelog
* [x] Add back to top links
* [ ] Add **Go** Reader & Writer interface
* [ ] Add Additional Templates w/ Examples

See the [open issues](https://github.com/rokath/TCOBS/issues) for a full list of proposed features (and known issues).

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

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- LICENSE -->
##  6. <a name='License'></a>License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- CONTACT -->
##  7. <a name='Contact'></a>Contact

Thomas Höhenleitner - <!-- [@twitter_handle](https://twitter.com/twitter_handle) - --> th@seerose.net
Project Link: [https://github.com/rokath/TCOBS](https://github.com/rokath/TCOBS)

<p align="right">(<a href="#top">back to top</a>)</p>

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
  <a href="https://github.com/rokath/TCOBS">
    <img src="docs/ref/COBSDataDisruption.svg" alt="Logo" width="800" height="80">
  </a>

<h3 align="center">TCOBS</h3>

  <p align="center">
    Common Object Byte Stuffing with optimized Run-Length Encoding 
    <br />
    <a href="https://github.com/rokath/TCOBS/blob/master/docs/TCOBSSpecification.md"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/rokath/TCOBS/blob/master/pkg/tcobs">View Code</a>
    ·
    <a href="https://github.com/rokath/TCOBS/issues">Report Bug</a>
    ·
    <a href="https://github.com/rokath/TCOBS/issues">Request Feature</a>
  </p>
</div>
