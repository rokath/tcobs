# TCOBS2 Specification (Ideas Draft)

<details>
  <summary>Table of Contents</summary>
  <ol>

<!-- vscode-markdown-toc -->
* 1. [ Preface](#Preface)
* 2. [Ternary and Quaternary Numbers](#TernaryandQuaternaryNumbers)
* 3. [Cipher Counted Notation](#CipherCountedNotation)
	* 3.1. [Cipher Counted Ternary Notation (CCTN)](#CipherCountedTernaryNotationCCTN)
		* 3.1.1. [One CCTN Cipher](#OneCCTNCipher)
		* 3.1.2. [Two CCTN Ciphers](#TwoCCTNCiphers)
		* 3.1.3. [Three CCTN Ciphers](#ThreeCCTNCiphers)
		* 3.1.4. [Four CCTN Ciphers](#FourCCTNCiphers)
		* 3.1.5. [Many CCTN Ciphers](#ManyCCTNCiphers)
	* 3.2. [Cipher Counted Quaternary Notation (CCQN)](#CipherCountedQuaternaryNotationCCQN)
		* 3.2.1. [One CCQN Cipher](#OneCCQNCipher)
		* 3.2.2. [Two CCQN Ciphers](#TwoCCQNCiphers)
		* 3.2.3. [Three CCQN Ciphers](#ThreeCCQNCiphers)
		* 3.2.4. [Four CCQN Ciphers](#FourCCQNCiphers)
		* 3.2.5. [Many CCQN Ciphers](#ManyCCQNCiphers)
* 4. [Encoding principle](#Encodingprinciple)
* 5. [Sigil Bytes](#SigilBytes)
	* 5.1. [Symbols assumptions](#Symbolsassumptions)
* 6. [Algorithm](#Algorithm)
* 7. [Change Log](#ChangeLog)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

<div id="top"></div>

  </ol>
</details>

##  1. <a name='Preface'></a> Preface

* T stands originally for **T**rice because the initial idea to develop it came from thoughts how to reduce the binary [trice](https://github.com/rokath/trice) data together with framing.
  * T symbols also the joining of the 2 orthogonal tasks compression and framing.
  * Additionally the usage of ternary and quaternary is reflected in the letter T.
* TCOBS2 is a better approach for TCOBS, suited also when long sequences of equal characters occur in the data stream.
* TCOBS2 is planned to replace TCOBS under the name TCOBS.
* About the data is assumed, that 00-bytes and FF-bytes occur a bit more often than other bytes.
* The aim concerning the compression is more to get a reasonable data reduction in a cheap way concerning minimal computing effort, than reducing to an absolute minimum. The method shown here simply counts repeated bytes and transforms them into shorter sequences. It works well also on very short messages, like 4 bytes and on very long buffers. The compressed buffer contains no 00-bytes anymore what is the aim of COBS. In the worst case, if no repeated bytes occur at all, the encoded data can be about 3% longer (1 byte per each 31 input bytes).
* To understand the encoding principle the used numbers system is explained first.

<p align="right">(<a href="#top">back to top</a>)</p>

##  2. <a name='TernaryandQuaternaryNumbers'></a>Ternary and Quaternary Numbers

* Common numbers:
  * decimal numbers with 10 digits, `0123456789`, like `0d109 = 1 * 10^2 + 0 * 10^1 + 9 * 10^0 = 109`
  * hexadecimal numbers with 16 ciphers `0123456789abcdef`, like `0xc0de = 12 * 16^3 + 0 * 16^2 + 13 * 16^1 + 14 * 16^0 = 49374`
  * binary numbers with 2 ciphers `01`, like `0b101 = 1 * 2^2 + 0 * 2^1 + 1 * 2^0 = 5`
  * octal numbers with 8 ciphers `01234567`, like `0o77 = 7 * 8^1 + 7 * 8^0 = 63`
* Not so common numbers:
  * Ternary numbers with 3 digits `012`, like `0t201 = 2 * 3^2 + 0 * 3^1 + 1 * 3^0 = 19`
  * Quaternary numbers with 4 digits `0123`, like `0q123 = 1 * 4^2 + 2 * 4^1 + 1 * 4^0 = 25`

<p align="right">(<a href="#top">back to top</a>)</p>

##  3. <a name='CipherCountedNotation'></a>Cipher Counted Notation

For the TCOBS encoding ternary and quaternary numbers are used in way, that the ciphers are counted too. That means for example, that cipher sequence `022` is not equal `22`.

###  3.1. <a name='CipherCountedTernaryNotationCCTN'></a>Cipher Counted Ternary Notation (CCTN)

* As ternary notation `0t` in front of the ciphers is used.
* As CCTN notation `0T` in front of the ciphers is used.
* Because the 0- and 1-values are never needed in TCOBS, the CCTN numbers start with 2

####  3.1.1. <a name='OneCCTNCipher'></a>One CCTN Cipher

`2 = 1 + 3^0`

|index | decimal | CCTN       | remark |
| -    | -       | -          | -      |
|      | 0       | impossible |        |
|      | 1       | impossible |        |
|  0   | 2       | 0T0        | exactly 1 cipher allowed |
| ...  | ...     | ...        | ...                      |
|  2   | 4       | 0T2        | exactly 1 cipher allowed |

####  3.1.2. <a name='TwoCCTNCiphers'></a>Two CCTN Ciphers

`5 = 1 + 3^0 + 3^1`

|index | decimal | CCTN       | remark |
| -    | -       | -          | -      |
| 0    | 5       | 0T00       | exactly 2 ciphers allowed |
| ...  | ...     | ...        | ...                       |
| 8    | 13      | 0T33       | exactly 2 ciphers allowed |

####  3.1.3. <a name='ThreeCCTNCiphers'></a>Three CCTN Ciphers

`14 = 1 + 3^0 + 3^1 + 3^2`

|index | decimal | CCTN       | remark |
| -    | -       | -          | -      |
| 0    | 14      | 0T000      | exactly 3 ciphers allowed |
| ...  | ...     | ...        | ...                       |
| 26   | 40      | 0T333      | exactly 3 ciphers allowed |

####  3.1.4. <a name='FourCCTNCiphers'></a>Four CCTN Ciphers

`41 = 1 + 3^0 + 3^1 + 3^2 + 3^3`

|index | decimal | CCTN       | remark |
| -    | -       | -          | -      |
| 0    | 41      | 0T0000     | exactly 4 ciphers allowed |
| ...  | ...     | ...        | ...                       |
| 80   | 121     | 0T3333     | exactly 4 ciphers allowed |

####  3.1.5. <a name='ManyCCTNCiphers'></a>Many CCTN Ciphers

| Cipher Count | generic start                   | start | index range  | value range |
| -            | -                               | -     | -            | - |
| 1            | 1 + 3^0                         |    2  | 0-2          | 2-4 |
| 2            | 1 + 3^0 + 3^1                   |    5  | 0-8          | 5-13 |
| 3            | 1 + 3^0 + 3^1 + 3^2             |   14  | 0-26         | 14-40 |
| 4            | 1 + 3^0 + 3^1 + 3^2 + 3^3       |   41  | 0-80         | 41-121 |
| 5            | 1 + 3^0 + 3^1 + 3^2 + 3^3 + 3^4 |  122  | 0-242        | 122-364 |
| ...          | ...                             |  ...  | ...          | ... |

###  3.2. <a name='CipherCountedQuaternaryNotationCCQN'></a>Cipher Counted Quaternary Notation (CCQN)

* As quaternary notation `0q` in front of the ciphers is used.
* As CCQN notation `0Q` in front of the ciphers is used.
* Because the 0-value is never needed the CCQN numbers start with 1

####  3.2.1. <a name='OneCCQNCipher'></a>One CCQN Cipher

`1 = 4^0`

|index | decimal | CCQN       | remark |
| -    | -       | -          | -      |
|      | 0       | impossible |        |
|  0   | 1       | 0Q0        | exactly 1 cipher allowed |
| ...  | ...     | ...        | ...                      |
|  3   | 4       | 0Q3        | exactly 1 cipher allowed |

####  3.2.2. <a name='TwoCCQNCiphers'></a>Two CCQN Ciphers

`5 = 4^0 + 4^1`

|index | decimal | CCQN       | remark |
| -    | -       | -          | -      |
| 0    | 5       | 0Q00       | exactly 2 ciphers allowed |
| 1    | 6       | 0Q01       | ...                       |
| 2    | 7       | 0Q02       | ...                       |
| 3    | 8       | 0Q03       | ...                       |
| 4    | 9       | 0Q10       | ...                       |
| ...  | ...     | ...        | ...                       |
| 15   | 20      | 0Q33       | exactly 2 ciphers allowed |

####  3.2.3. <a name='ThreeCCQNCiphers'></a>Three CCQN Ciphers

`21 = 4^0 + 4^1 + 4^2`

|index | decimal | CCQN       | remark |
| -    | -       | -          | -      |
| 0    | 21      | 0Q000      | exactly 3 ciphers allowed |
| ...  | ...     | ...        | ...                       |
| 63   | 84      | 0Q333      | exactly 3 ciphers allowed |

####  3.2.4. <a name='FourCCQNCiphers'></a>Four CCQN Ciphers

`85 = 4^0 + 4^1 + 4^2 + 4^3`

|index | decimal | CCQN       | remark |
| -    | -       | -          | -      |
| 0    | 85      | 0Q0000     | exactly 4 ciphers allowed |
| ...  | ...     | ...        | ...                       |
| 255  | 340     | 0Q3333     | exactly 4 ciphers allowed |

####  3.2.5. <a name='ManyCCQNCiphers'></a>Many CCQN Ciphers

| Cipher Count | generic start               | start | index range  | value range |
| -            | -                           | -     | -            | - |
| 1            | 4^0                         |    1  | 0-3          | 1-4 |
| 2            | 4^0 + 4^1                   |    5  | 0-15         | 5-20 |
| 3            | 4^0 + 4^1 + 4^2             |   21  | 0-63         | 21-84 |
| 4            | 4^0 + 4^1 + 4^2 + 4^3       |   85  | 0-255        | 85-340 |
| 5            | 4^0 + 4^1 + 4^2 + 4^3 + 4^4 |  341  | 0-1023       | 341-1364 |
| ...          | ...                         |  ...  | ...          | ... |

<p align="right">(<a href="#top">back to top</a>)</p>

##  4. <a name='Encodingprinciple'></a>Encoding principle

* Legend: 
  * `xx` any byte different from its neighbor.
  * `AA` any non FF- and non 00-byte, but AA==AA.

* For count encoding different types of sigil bytes are used:
  * `Z0`, `Z1`, `Z2`, `Z3` for **1** to **n** 00-bytes in a row
  * `F0`, `F1`, `F2`, `F3` for **1** to **n** FF-bytes in a row
  * `R0`, `R1`, `R2` for     **2** to **n** equal other bytes in a row
* Z- and F- sigils are CCQN ciphers 0-3 and the R-sigils represent CCTN ciphers 0-2.
* Examples:

| decoded             | encoded         | number notation / remark |
| -                   | -               | -               |
| xx `00` xx          | xx `Z0` xx        | `0Q0` = 1 zero  |
| xx `00 00 00 00` xx | xx `Z3` xx        | `0Q3` = 4 zeros |
| xx `AA AA` xx       | xx `AA AA` xx     | 2 times AA stays the same. |
| xx `AA AA AA` xx    | xx `AA R0` xx     | 3 times `AA` gets `AA` followed by 2 `AA` coded as `R0` |
| xx `13 times AA` xx | xx `AA R3 R2` xx  | AA stands for itself and indicates what following R-sigils mean \- `0Q32` = R3 R2 stands for 12 following AA |

<p align="right">(<a href="#top">back to top</a>)</p>

##  5. <a name='SigilBytes'></a>Sigil Bytes

* Which pattern are used as sigil bytes is an optimizing question. The table seems to be reasonable concerning the assumption to have statistically more FF- and 00-bytes, especially also short rows of them, in the data stream to be encoded.

| value 7-5 | bits 7-0   | Byte Name       | sign  | offset bits      | offset value| usage | Remark |
|    -      | -          | -               | -     | -                | -           | -     | - |
|    0      | `00000000` | forbidden       |       |                  |             |       | used later as delimiter byte |
|    0      | `0000oooo` | Repeat sigil    | **R2**|  `oooo` = 1-15   |  0-14       | less  | ternary cipher 2 for an any count, offset = `oooo` - 1 |
|    0      | `0001oooo` | Zero   sigil    | **Z3**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 3 for a 0x00 count |
|    1      | `001ooooo` | Zero   sigil    | **Z0**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 0 for a 0x00 count |
|    2      | `0100oooo` | Repeat sigil    | **R1**|  `oooo` = 0-15   |  0-15       | less  | ternary cipher 1 for an any count |
|    2      | `0101oooo` | Zero   sigil    | **Z2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for a 0x00 count |
|    3      | `011ooooo` | Zero   sigil    | **Z1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for a 0x00 count |
|    4      | `100ooooo` | Repeat sigil    | **R0**| `ooooo` = 0-31   |  0-31       | more  | ternary cipher 0 for an any count |
|    5      | `101ooooo` | NOP    sigil    | **N** | `ooooo` = 0-31   |  0-31       | more  | no meaning, used for keeping the sigil chain linked |
|    6      | `110ooooo` | Full   sigil    | **F1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for a 0xFF count |
|    7      | `1110oooo` | Full   sigil    | **F2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for a 0xFF count |
|    7      | `1111oooo` | Full   sigil    | **F3**|  `oooo` = 0-14   |  0-14       | less  | quaternary cipher 3 for a 0xFF count, offset 15 forbidden to distinguish from F0=FF sigil byte |
|           | `11111111` | Full   sigil    | **F0**|                  |  0          |       | CCQN cipher 0, needs not to be inside the sigil chain, but can. |

###  5.1. <a name='Symbolsassumptions'></a>Symbols assumptions

* N, Z0, Z1, F1, R0 are a bit more often in use, therefore they can carry link offsets 0-31 in 5 offset bits.
* Z2, Z3, F2, F3, R2, R3 are a bit less often in use, therefore they can carry link offsets 0-15(14) in 4 offset bits.
* F0 has no offset bits. Therefore its implicit offset value is 0, when used inside the sigil chain.
* Concatenation of offset bits in neighbored sigil bytes is not used: makes is useless complicated or is maybe impossible.

* It would be possible to use AA as sigil byte with offset 0 but that needs more often to insert a **N** sigil byte.
* With F0=FF this is not necessary because in does not need to be in the sigil bytes link chain. That is possible by not allowing offset 15 in the F3-sigil. A single `FF` inside the data stream is treatable this way as a normal single other byte. Even `FF` is de-facto a sigil byte in the encoded data stream, it needs not to be inside the sigil chain. Examples:

| decoded             | encoded         | number notation / remark |
| -                   | -               | -               |
| xx `FF` xx          | xx `F0` xx      | `F0` == `FF` = 1 `FF`  |
| xx `FF FF` xx       | xx `F1` xx      | `0Q1` = 2 times `FF` - `F1` is part of sigil chain |
| xx `3 times FF` xx  | xx `F2` xx      | `0Q2` = 3 times `FF` - `F2` is part of sigil chain |
| xx `4 times FF` xx  | xx `F3` xx      | `0Q3` = 4 times `FF` - `F3` is part of sigil chain |
| xx `5 times FF` xx  | xx `F0 F0` xx   | `0Q00` = 5 times `FF` - not part of sigil chain |
| xx `6 times FF` xx  | xx `F0 F1` xx   | `0Q01` = 6 times `FF` - `F1` is part of sigil chain |
| xx `9 times FF` xx  | xx `F1 F0` xx   | `0Q00` = 9 times `FF` - `F1` is part of sigil chain, `F0` is allowed to be part of sigil chain |

<p align="right">(<a href="#top">back to top</a>)</p>

##  6. <a name='Algorithm'></a>Algorithm

* Compute character count
* Determine Sigil sequence as ciphers
* Handle Offsets to build sigil chain (buffer ends with a sigil byte)
* Mathematical prove? 

<p align="right">(<a href="#top">back to top</a>)</p>

##  7. <a name='ChangeLog'></a>Change Log

| Date | Version | Comment |
| - | - | - |
| 2022-JUN-00 | 0.0.0 | initial |
| 2022-JUL-07 | 0.1.0 | CCTN start now with 2. |
| 2022-JUL-07 | 0.1.1 | Explanation and samples added. |

<p align="right">(<a href="#top">back to top</a>)</p>
