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

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

<div id="top"></div>

  </ol>
</details>

##  1. <a name='Preface'></a> Preface

* TCOBS2 is a better approach for TCOBS, suited also when long sequences of equal characters occur in the data stream.
* TCOBS2 is planned to replace TCOBS under the name TCOBS.
* About the data is assumed, that 00-bytes and FF-bytes occur a bit more often than other bytes.
* To understand the encoding principle the used numbers system is explained first.

<p align="right">(<a href="#top">back to top</a>)</p>

##  2. <a name='TernaryandQuaternaryNumbers'></a>Ternary and Quaternary Numbers

* Common numbers:
  * decimal numbers with 10 digits, `0123456789`, like `0d109 = 1 * 10^2 + 0 * 10^1 + 9 * 10^0 = 109`
  * hexadecimal numbers with 16 ciphers `0123456789abcdef`, like `0xc0de = 12 * 16^3 + 0 * 16^2 + 13 * 16^1 + 14 * 16^0 = 49374`
  * binary numbers with 2 ciphers `01`, like `0b101 = 1 * 2^2 + 0 * 2^1 + 1 * 2^0 = 5`
* Not so common numbers:
  * Ternary numbers with 3 digits `012`, like `0t201 = 2 * 3^2 + 0 * 3^1 + 1 * 3^0 = 19`
  * Quaternary numbers with 4 digits `0123`, like `0q123 = 1 * 4^2 + 2 * 4^1 + 1 * 4^0 = 25`

<p align="right">(<a href="#top">back to top</a>)</p>

##  3. <a name='CipherCountedNotation'></a>Cipher Counted Notation

For the TCOBS encoding ternary and quaternary numbers are used in way, that the ciphers are counted too. That means for example, that cipher sequence `022` is not equal `22`.

###  3.1. <a name='CipherCountedTernaryNotationCCTN'></a>Cipher Counted Ternary Notation (CCTN)

####  3.1.1. <a name='OneCCTNCipher'></a>One CCTN Cipher

`1 = 3^0`

|index | decimal | CCQN       | remark |
| -    | -       | -          | -      |
|      | 0       | impossible |        |
|  0   | 1       | 0Q0        | exactly 1 cipher allowed |
| ...  | ...     | ...        | ...                      |
|  2   | 3       | 0Q2        | exactly 1 cipher allowed |

####  3.1.2. <a name='TwoCCTNCiphers'></a>Two CCTN Ciphers

`4 = 3^0 + 3^1`

|index | decimal | CCQN       | remark |
| -    | -       | -          | -      |
| 0    | 4       | 0Q00       | exactly 2 ciphers allowed |
| ...  | ...     | ...        | ...                       |
| 8    | 12      | 0Q33       | exactly 2 ciphers allowed |

####  3.1.3. <a name='ThreeCCTNCiphers'></a>Three CCTN Ciphers

`13 = 3^0 + 3^1 + 3^2`

|index | decimal | CCQN       | remark |
| -    | -       | -          | -      |
| 0    | 13      | 0Q000      | exactly 3 ciphers allowed |
| ...  | ...     | ...        | ...                       |
| 26   | 39      | 0Q333      | exactly 3 ciphers allowed |

####  3.1.4. <a name='FourCCTNCiphers'></a>Four CCTN Ciphers

`40 = 3^0 + 3^1 + 3^2 + 3^3`

|index | decimal | CCQN       | remark |
| -    | -       | -          | -      |
| 0    | 40      | 0Q0000     | exactly 4 ciphers allowed |
| ...  | ...     | ...        | ...                       |
| 80   | 120     | 0Q3333     | exactly 4 ciphers allowed |

####  3.1.5. <a name='ManyCCTNCiphers'></a>Many CCTN Ciphers

| Cipher Count | generic start               | start | index range  | value range |
| -            | -                           | -     | -            | - |
| 1            | 3^0                         |    1  | 0-2          | 1-3 |
| 2            | 3^0 + 3^1                   |    4  | 0-9          | 4-12 |
| 3            | 3^0 + 3^1 + 3^2             |   13  | 0-27         | 13-40 |
| 4            | 3^0 + 3^1 + 3^2 + 3^3       |   40  | 0-81         | 41-120 |
| 5            | 3^0 + 3^1 + 3^2 + 3^3 + 3^4 |  121  | 0-243        | 121-363 |
| ...          | ...                         |  ...  | ...          | ... |

###  3.2. <a name='CipherCountedQuaternaryNotationCCQN'></a>Cipher Counted Quaternary Notation (CCQN)

* As quaternary notation `0q` in front of the ciphers is used.
* As CCQN notation a `0Q` in front of the ciphers is used: `0q123 != 0Q123`
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

* For count encoding different types of sigil bytes are used:
  * `Z0`, `Z1`, `Z2`, `Z3` for **1** to **n** 00-bytes in a row
  * `F0`, `F1`, `F2`, `F3` for **1** to **n** FF-bytes in a row
  * `R0`, `R1`, `R2` for     **1** to **n** equal other bytes in a row
* Z- and F- sigils are CCQN ciphers 0-3 and the R-sigils represent CCTN ciphers.
* Examples:

| decoded           | encoded         | number notation / remark |
| -                 | -               | -               |
| xx 00 xx          | xx Z0 xx        | `0Q0` = 1 zero  |
| xx 00 00 00 00    | xx Z3 xx        | `0Q3` = 4 zeros |
| xx 13 times AA xx | xx AA R3 R3 xx  | AA stands for itself and indicates what following R-sigils mean \- `0Q33` = R3 R3 stands for 12 following AA |


<p align="right">(<a href="#top">back to top</a>)</p>

##  5. <a name='SigilBytes'></a>Sigil Bytes

* Which pattern are used as sigil bytes is a optimizing question. The table seems to be reasonable concerning the assumption to have statistically more FF- and 00-bytes, especially also short rows of them, in the data stream to be encoded.

| value 7-5 | bits 7-0   | Byte Name       | sign  | offset bits      | offset value| usage | Remark |
|    -      | -          | -               | -     | -                | -           | -     | - |
|    0      | `00000000` | forbidden       |       |                  |             |       | used later as delimiter byte |
|    0      | `0000oooo` | Repeat sigil    | **R2**|  `oooo` = 1-15   |  0-14       | less  | quaternary cipher 3 for an any count, offset = `oooo` - 1 |
|    0      | `0001oooo` | Zero   sigil    | **Z3**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 3 for a 0x00 count |
|    1      | `001ooooo` | Zero   sigil    | **Z0**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 0 for a 0x00 count |
|    2      | `0100oooo` | Repeat sigil    | **R1**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for an any count |
|    2      | `0101oooo` | Zero   sigil    | **Z2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for a 0x00 count |
|    3      | `011ooooo` | Zero   sigil    | **Z1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for a 0x00 count |
|    4      | `100ooooo` | Repeat sigil    | **R0**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for an any count |
|    5      | `101ooooo` | NOP    sigil    | **N** | `ooooo` = 0-31   |  0-31       | more  | no meaning, used for keeping the sigil chain linked |
|    6      | `110ooooo` | Full   sigil    | **F1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for a 0xFF count |
|    7      | `1110oooo` | Full   sigil    | **F2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 3 for a 0xFF count |
|    7      | `1111oooo` | Full   sigil    | **F3**|  `oooo` = 0-14   |  0-14       | less  | quaternary cipher 3 for a 0xFF count, offset 15 forbidden to distinguish from F0=FF sigil byte |
|           | `11111111` | Full   sigil    | **F0**|                  |  0          |       | CCQN cipher 0, needs not to be inside the sigil chain, but can. |

###  5.1. <a name='Symbolsassumptions'></a>Symbols assumptions

* N, Z0, Z1, F1, R0 are a bit more often in use, therefore they can carry link offsets 0-31
* Z2, Z3, F2, F3, R2, R3 are a bit less often in use, therefore they can carry link offsets 0-15(14)
* F0 has no offset bits. Therefore its implicit offset value is 0, when used inside the sigil chain.
* Concatenation of offset bits in neighbored sigil bytes is not used: makes is useless complicated or is maybe impossible.

* R sigils are ternary ciphers
* F & Z sigils are quaternary ciphers

* It would be possible to use AA as sigil byte with offset 0 but that needs more often to insert a **N** sigil byte.
* With F0=FF this is not necessary because in does not need to be in the sigil bytes link chain. That is possible by not allowing offset 15 in the F3-sigil.

<p align="right">(<a href="#top">back to top</a>)</p>

##  6. <a name='Algorithm'></a>Algorithm

* Compute character count
* Determine Sigil sequence as S1-S4 ciphers
* Handle Offsets to build sigil chain
* Mathematical prove? 

<p align="right">(<a href="#top">back to top</a>)</p>
