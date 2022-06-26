# TCOBS2 Specification (Ideas Draft)

<!-- vscode-markdown-toc -->
* 1. [ Preface](#Preface)
	* 1.1. [Symbols](#Symbols)

<!-- vscode-markdown-toc-config
	numbering=true
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

##  1. <a name='Preface'></a> Preface

* TCOBS2 is an alternative for TCOBS, especially when long sequences of equal characters occur in the data stream.

###  1.1. <a name='Symbols'></a>Symbols


| value 7-5 | bits 7-0   | Byte Name       | sign  | offset bits      | offset value| Remark |
|    -      | -          | -               | -     | -                | -           | - |
|    0      | `00000000` | forbidden       |       |                  |             | used later as delimiter byte |
|    0      | `0000oooo` | Repeat sigil    | **R4**|  `oooo` = 1-15   |  0-14       | less, offset = `oooo` - 1 |
|    0      | `0001oooo` | Zero   sigil    | **Z4**|  `oooo` = 0-15   |  0-15       | less |
|    1      | `001ooooo` | Zero   sigil    | **Z1**| `ooooo` = 0-31   |  0-31       | more |
|    2      | `0100oooo` | Zero   sigil    | **Z2**|  `oooo` = 0-15   |  0-15       | less |
|    2      | `0101oooo` | Zero   sigil    | **Z3**|  `oooo` = 0-15   |  0-15       | less |
|    3      | `011ooooo` | Repeat sigil    | **R3**| `ooooo` = 0-31   |  0-31       | more |
|    4      | `100ooooo` | Repeat sigil    | **R2**| `ooooo` = 0-31   |  0-31       | more |
|    5      | `101ooooo` | NOP    sigil    | **N** | `ooooo` = 0-31   |  0-31       | more |
|    6      | `110ooooo` | Full   sigil    | **F2**| `ooooo` = 0-31   |  0-31       | more |
|    7      | `1111oooo` | Full   sigil    | **F3**|  `oooo` = 0-15   |  0-15       | less |
|    7      | `1110oooo` | Full   sigil    | **F4**|  `oooo` = 0-15   |  0-15       | less |

|Z                 | Z1 | Z2 | Z3 | Z4 | remark |
|-                 | -  | -  | -  | -  | - |
|value count       |  1 |  2 |  3 |  4 | count = 0 + number |
|quaternary number |  0 |  1 |  2 |  3 | [Quaternary_numeral_system](https://en.wikipedia.org/wiki/Quaternary_numeral_system)|
|decimal    number |  0 |  1 |  2 |  3 | |

|Z                 |Z1Z1|Z1Z2|Z1Z3|Z1Z4|Z2Z1|Z2Z2|Z2Z3|Z2Z4|Z3Z1|Z3Z2|Z3Z3|Z3Z4|Z4Z1|Z4Z2|Z4Z3|Z4Z4| remark |
|-                 | -  | -  | -  | -  | -  | -  | -  | -  | -  | -  | -  | -  | -  | -  | -  | -  | - |
|value count       |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | count = 5 + number |
|quaternary number | 00 | 01 | 02 | 03 | 10 | 11 | 12 | 13 | 20 | 21 | 22 | 23 | 30 | 31 | 32 | 33 | [Quaternary_numeral_system](https://en.wikipedia.org/wiki/Quaternary_numeral_system) |
|decimal    number |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 |  |

|Z                 |Z1Z1Z1|Z1Z1Z2|Z1Z1Z3|Z1Z1Z4|Z1Z2Z1|Z1Z2Z2|Z1Z2Z3|Z1Z2Z4|Z1Z3Z1|Z1Z3Z2|Z1Z3Z3|Z1Z3Z4|Z1Z4Z1|Z1Z4Z2|Z1Z4Z3|Z1Z4Z4| remark |
|-                 |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  | - |
|value count       |   21 |   22 |   23 |   24 |   25 |   26 |   27 |   28 |   29 |   30 |   31 |   32 |   33 |   34 |   35 |   36 | count = 21 + number |
|quaternary number |  000 |  001 |  002 |  003 |  010 |  011 |  012 |  013 |  020 |  021 |  022 |  023 |  030 |  031 |  032 |  033 | [Quaternary_numeral_system](https://en.wikipedia.org/wiki/Quaternary_numeral_system) |
|decimal    number |    0 |    1 |    2 |    3 |    4 |    5 |    6 |    7 |    8 |    9 |   10 |   11 |   12 |   13 |   14 |   15 |  |

|Z                 |Z2Z1Z1|Z2Z1Z2|Z2Z1Z3|Z2Z1Z4|Z2Z2Z1|Z2Z2Z2|Z2Z2Z3|Z2Z2Z4|Z2Z3Z1|Z2Z3Z2|Z2Z3Z3|Z2Z3Z4|Z2Z4Z1|Z2Z4Z2|Z2Z4Z3|Z2Z4Z4| remark |
|-                 |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  | - |
|value count       |   37 |   38 |   39 |   40 |   41 |   42 |   43 |   44 |   45 |   46 |   47 |   48 |   49 |   50 |   51 |   52 | count = 21 + number |
|quaternary number |  100 |  101 |  102 |  103 |  110 |  111 |  112 |  113 |  120 |  121 |  122 |  123 |  130 |  131 |  132 |  133 | [Quaternary_numeral_system](https://en.wikipedia.org/wiki/Quaternary_numeral_system) |
|decimal    number |   16 |   17 |   18 |   19 |   20 |   21 |   22 |   23 |   24 |   25 |   26 |   27 |   28 |   29 |   30 |   31 |  |

|Z                 |Z3Z1Z1|Z3Z1Z2|Z3Z1Z3|Z3Z1Z4|Z3Z2Z1|Z3Z2Z2|Z3Z2Z3|Z3Z2Z4|Z3Z3Z1|Z3Z3Z2|Z3Z3Z3|Z3Z3Z4|Z3Z4Z1|Z3Z4Z2|Z3Z4Z3|Z3Z4Z4| remark |
|-                 |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  | - |
|value count       |   53 |   54 |   55 |   56 |   57 |   58 |   59 |   60 |   61 |   62 |   63 |   64 |   65 |   66 |   67 |   68 | count = 21 + number |
|quaternary number |  200 |  201 |  202 |  203 |  210 |  211 |  212 |  213 |  220 |  221 |  222 |  223 |  230 |  231 |  232 |  233 | [Quaternary_numeral_system](https://en.wikipedia.org/wiki/Quaternary_numeral_system) |
|decimal    number |   32 |   33 |   34 |   35 |   36 |   37 |   38 |   39 |   40 |   41 |   42 |   43 |   44 |   45 |   46 |   47 |  |

|Z                 |Z4Z1Z1|Z4Z1Z2|Z4Z1Z3|Z4Z1Z4|Z4Z2Z1|Z4Z2Z2|Z4Z2Z3|Z4Z2Z4|Z4Z3Z1|Z4Z3Z2|Z4Z3Z3|Z4Z3Z4|Z4Z4Z1|Z4Z4Z2|Z4Z4Z3|Z4Z4Z4| remark |
|-                 |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  |   -  | - |
|value count       |   69 |   70 |   71 |   72 |   73 |   74 |   75 |   76 |   77 |   78 |   79 |   80 |   81 |   82 |   83 |   84 | count = 21 + number |
|quaternary number |  300 |  301 |  302 |  303 |  310 |  311 |  312 |  313 |  320 |  321 |  322 |  323 |  330 |  331 |  332 |  333 | [Quaternary_numeral_system](https://en.wikipedia.org/wiki/Quaternary_numeral_system) |
|decimal    number |   48 |   49 |   50 |   51 |   52 |   53 |   54 |   55 |   56 |   57 |   58 |   59 |   60 |   61 |   62 |   63 |  |

|Z                 |Z1Z4Z1Z1|Z1Z4Z1Z2|Z1Z4Z1Z3|Z1Z4Z1Z4|Z1Z4Z2Z1|Z1Z4Z2Z2|Z1Z4Z2Z3|Z1Z4Z2Z4|Z1Z4Z3Z1|Z1Z4Z3Z2|Z1Z4Z3Z3|Z1Z4Z3Z4|Z1Z4Z4Z1|Z1Z4Z4Z2|Z1Z4Z4Z3|Z1Z4Z4Z4| remark |
|-                 |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    | - |
|value count       |   85   |        |        |        |        |        |        |        |        |        |        |        |        |        |        |   100  | count = 85 + number |
|quaternary number |  0000  |  0001  |  0002  |  0003  |  0010  |  0011  |  0012  |  0013  |  0020  |  0021  |  0022  |  0023  |  0030  |  0031  |  0032  |  0033  | [Quaternary_numeral_system](https://en.wikipedia.org/wiki/Quaternary_numeral_system) |
|decimal    number |    0   |        |        |        |        |        |        |        |        |        |        |        |        |        |        |    15  |  |

...

|Z                 |Z4Z4Z1Z1|Z4Z4Z1Z2|Z4Z4Z1Z3|Z4Z4Z1Z4|Z4Z4Z2Z1|Z4Z4Z2Z2|Z4Z4Z2Z3|Z4Z4Z2Z4|Z4Z4Z3Z1|Z4Z4Z3Z2|Z4Z4Z3Z3|Z4Z4Z3Z4|Z4Z4Z4Z1|Z4Z4Z4Z2|Z4Z4Z4Z3|Z4Z4Z4Z4| remark |
|-                 |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    |   -    | - |
|value count       |   85   |        |        |        |        |        |        |        |        |        |        |        |        |        |        |   341  | count = 85 + number |
|quaternary number |  3300  |  3301  |  3302  |  3303  |  3310  |  3311  |  3312  |  3313  |  3320  |  3321  |  3322  |  3323  |  3330  |  3331  |  3332  |  3333  | [Quaternary_numeral_system](https://en.wikipedia.org/wiki/Quaternary_numeral_system) |
|decimal    number |   240  |        |        |        |        |        |        |        |        |        |        |        |        |        |        |   256  |  |

* 5 = 4^0 + 4^1
* 21 = 5 + 4^2 = 1 + 4^1 + 4^2
* 85 = 21 + 4^3 = 4^0 + 4^1 + 4^2 + 4^3
* 341 =  4^0 + 4^1 + 4^2 + 4^3 + 4^4
* ...

* [x] Same with F. 0xFF serves as F1.
* [x] Same with R. 0xaa serves as R1.
* [x] Algorithm: 
  * Compute character count
  * Determine Sigil sequence
  * Handle Offsets

xx aa aa
xx aa aa aa  -> xx aa R2 -> so the aa before the first R is not counted inside the R-sequence. It stays for itself and serves then as R1.
