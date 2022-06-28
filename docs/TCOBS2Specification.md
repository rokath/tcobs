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

## Encoded package structure

* The encoded data are structured in segments of length 0 up to 31 divided by sigil bytes.
* Each encoded package ends with a sigil byte carrying an offset 0-31 to the *next* sigil byte in front of it or to the package start.
* The **N** sigil byte is only needed as bridge to keep the sigil bytes chain linked and is mathematically ignored in the encoding.
* During decoding, the offset of the last sigil byte serves as a pointer to the segment begin, which is then interpreted forward.
* A segment is empty or contains up to 31 no-sigil bytes.

## Encoding principle

* Any sequence of equal bytes is converted into a TCCQN following these rules:
  * A 00-byte sequence is converted into a Zn sigil bytes sequence representing a TCCQN. Examples:
    * 00 = 1 times 00 = 0T0 = S0 = Z0
    * 00 00 00 00 00 00 = 6 times 00 = 0T01 = S0S1 = Z0Z1
    * 20 times 00 = 0T33 = S3S3 = Z3Z3
    * 21 times 00 = 0T000 = Z0Z0Z0
  * A FF-byte sequence is converted into Fn sigil bytes representing a TCCQN. Examples:
    * FF -> 0T0 = F0 = FF (FF represents F0)
    * FF FF FF FF FF FF = 0T01 = S0S1 = F0F1 = FFF1 (FF represents F0)
    * 20 times FF = 0T33 = S3S3 = F3F3
    * 21 times FF = 0T000 = S0S0S0 = F0F0F0 = FFFFFF
  * A AA-byte sequence is converted into AA plus Rn sigil bytes representing a TCCQN. Examples:
    * AA -> AA
    * AA AA = AA + 1 times AA = AA 0T0 = AA S0 = AA R0 = AAAA (The first AA tells AA and the 2nd AA represents R0)
    * AA AA AA AA AA AA = AA + 5 times AA = AA 0T00 = AAR0R0 = AAAAAA (The first AA tells AA and the next AA represent R0)
    * 20 times AA = AA + 19 times AA = AA 0T32 = AAR3R2
    * 21 times AA = AA + 20 times AA = AA 0T33 = AAR3R3
    * 22 times AA = AA + 21 times AA = AA 0T000 = AAR0R0R0 = AAAAAAAA (The first AA tells AA and the other AA represent R0)
* Any number >=1 can be coded as TCCQN
* The encoding is doable without regard of the chaining, which is done secondary.


##  1.1. <a name='Symbols'></a>Symbols Variant 3

| value 7-5 | bits 7-0   | Byte Name       | sign  | offset bits      | offset value| usage | Remark |
|    -      | -          | -               | -     | -                | -           | -     | - |
|    0      | `00000000` | forbidden       |       |                  |             |       | used later as delimiter byte |
|    0      | `0000oooo` | Repeat sigil    | **R3**|  `oooo` = 1-15   |  0-14       | less  | quaternary cipher 3 for an any count, offset = `oooo` - 1 |
|    0      | `0001oooo` | Zero   sigil    | **Z3**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 3 for a 0x00 count |
|    1      | `001ooooo` | Zero   sigil    | **Z0**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 0 for a 0x00 count |
|    2      | `0100oooo` | Repeat sigil    | **R2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for an any count |
|    2      | `0101oooo` | Zero   sigil    | **Z2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for a 0x00 count |
|    3      | `011ooooo` | Zero   sigil    | **Z1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for a 0x00 count |
|    4      | `100ooooo` | Repeat sigil    | **R1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for an any count |
|    5      | `101ooooo` | NOP    sigil    | **N** | `ooooo` = 0-31   |  0-31       | more  | no meaning, used for keeping the sigil chain linked |
|    6      | `110ooooo` | Full   sigil    | **F1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for a 0xFF count |
|    7      | `1111oooo` | Full   sigil    | **F2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for a 0xFF count |
|    7      | `1110oooo` | Full   sigil    | **F3**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 3 for a 0xFF count |
|           | `11111111` | Full   sigil    | **F0**|                  |             |       | TCCQN cipher 0 |
|           |not FF or 00| Repeat sigil    | **R0**|                  |             |       | TCCQN cipher 0 |

##  1.1. <a name='Symbols'></a>Symbols Variant 4

| value 7-5 | bits 7-0   | Byte Name       | sign  | offset bits      | offset value| usage | Remark |
|    -      | -          | -               | -     | -                | -           | -     | - |
|    0      | `00000000` | forbidden       |       |                  |             |       | used later as delimiter byte |
|    0      | `0000oooo` | Repeat sigil    | **R3**|  `oooo` = 1-15   |  0-14       | less  | quaternary cipher 3 for an any count, offset = `oooo` - 1 |
|    0      | `0001oooo` | Zero   sigil    | **Z3**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 3 for a 0x00 count |
|    1      | `001ooooo` | Zero   sigil    | **Z0**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 0 for a 0x00 count |
|    2      | `0100oooo` | Repeat sigil    | **R2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for an any count |
|    2      | `0101oooo` | Zero   sigil    | **Z2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for a 0x00 count |
|    3      | `011ooooo` | Zero   sigil    | **Z1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for a 0x00 count |
|    4      | `100ooooo` | Repeat sigil    | **R1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for an any count |
|    5      | `101ooooo` | NOP    sigil    | **N** | `ooooo` = 0-31   |  0-31       | more  | no meaning, used for keeping the sigil chain linked |
|    6      | `110ooooo` | Full   sigil    | **F1**| `ooooo` = 0-31   |  0-31       | more  | quaternary cipher 1 for a 0xFF count |
|    7      | `1111oooo` | Full   sigil    | **F2**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 2 for a 0xFF count |
|    7      | `1110oooo` | Full   sigil    | **F3**|  `oooo` = 0-15   |  0-15       | less  | quaternary cipher 3 for a 0xFF count |
|           | `11111111` | Full   sigil    | **F0**|                  |             |       | TCCQN cipher 0 |
|           |not FF or 00| Repeat sigil    | **R0**|                  |             |       | TCCQN cipher 0 |

### Symbols assumptions

* N, Z0, Z1, F1, R1 are a bit more often in use, therefore they can carry link offsets 0-31
* Z2, Z3, F2, F3, R2, R3 are a bit less often in use, therefore they can carry link offsets 0-15(14)
* F0 and R0 have no offset bits. Therefore their offset value is 0.
* A sigil byte sequence concatenates its offset bits to one offset counting from the leftmost cipher.
* If a package ends with yyzzAAAAAAFFFFFFFF, zz could be a single sigil or yyzz is a sigil sequence. 

## Syntax

* `0b01` is common for binary notation
* `0o01234567` is common for octal notation
* `0123456789` is common for decimal notation
* `0x0123456789abcdef` is common for hexadecimal notation
* `0q0123` is used here for [quaternary](https://en.wikipedia.org/wiki/Quaternary_numeral_system) notation
* `0Q0123` is used here for cipher counted quaternary notation
* `CCQN` Cipher Counted Quaternary Number, example `0Q0123`
* `TCCQN` Transformed Cipher Counted Quaternary Number: TCCQN = 1 + CCQN, example `0T0123`
* `S0` quaternary cipher `0` standing for `Z0`, `F0` or `R0`.
* `S1` quaternary cipher `1` standing for `Z1`, `F1` or `R1`.
* `S2` quaternary cipher `2` standing for `Z2`, `F2` or `R2`.
* `S3` quaternary cipher `3` standing for `Z3`, `F3` or `R3`.
* `00` 0-byte
* `FF` FF-byte, used also as F1 cipher
* `xx` any unencoded byte
* `xx xx` any unencoded byte sequence of possibly different bytes
* `yy` any encoded byte
* R1 is no sigil byte, it is expressed  

## Cipher Counted Quaternary Notation

`0q123 != 0Q123` !

### One Cipher

|index | decimal | CCQN       | TCCQN      | remark |
| -    | -       | -          | -          | -      |
| 0    | 0       | 0Q0        | impossible | exactly 1 cipher allowed |
| 1    | 1       | 0Q1        | 0T0        | exactly 1 cipher allowed |
| 2    | 2       | 0Q2        | 0T1        | exactly 1 cipher allowed |
| 3    | 3       | 0Q3        | 0T2        | exactly 1 cipher allowed |
| 4    | 4       | impossible | 0T3        | exactly 1 cipher allowed |

### Two Ciphers

`0q33 = 15` 

`4 = 4^1`

|index | decimal | CCQN       | TCCQN      | remark |
| -    | -       | -          | -          | -      |
| 0    | 4       | 0Q00       | impossible | exactly 2 ciphers allowed |
| 1    | 5       | 0Q01       | 0T00       | exactly 2 ciphers allowed |
| ...  | ...     | ...        | ...        | ...                       |
| 14   | 18      | 0Q32       | 0T31       | exactly 2 ciphers allowed |
| 15   | 19      | 0Q33       | 0T32       | exactly 2 ciphers allowed |
| 16   | 20      | impossible | 0T33       | exactly 2 ciphers allowed |

### Three Ciphers

`20 = 4^1 + 4^2`

|index | decimal | CCQN       | TCCQN      | remark |
| -    | -       | -          | -          | -      |
| 0    | 20      | 0Q000      | impossible | exactly 3 ciphers allowed |
| 1    | 21      | 0Q001      | 0T000      | exactly 3 ciphers allowed |
| ...  | ...     | ...        | ...        | ...                       |
| 62   | 82      | 0Q332      | 0T331      | exactly 3 ciphers allowed |
| 63   | 83      | 0Q333      | 0T332      | exactly 3 ciphers allowed |
| 64   | 84      | impossible | 0T333      | exactly 3 ciphers allowed |

### Four Ciphers

`84 = 4^1 + 4^2 + 4^3`

|index | decimal | CCQN       | TCCQN      | remark |
| -    | -       | -          | -          | -      |
| 0    | 84      | 0Q0000     | impossible | exactly 4 ciphers allowed |
| 1    | 85      | 0Q0001     | 0T0000     | exactly 4 ciphers allowed |
| ...  | ...     | ...        | ...        | ...                       |
| 254  | 338     | 0Q3332     | 0T3331     | exactly 4 ciphers allowed |
| 255  | 339     | 0Q3333     | 0T3332     | exactly 4 ciphers allowed |
| 256  | 340     | impossible | 0T3333     | exactly 4 ciphers allowed |

### Many Ciphers (CCQN)

| Cipher Count | generic start             | start | index range  | value range |
| -            | -                         | -     | -            | - |
| 1            |                           |    0  | 0-3          | 0-3 |
| 2            | 4^1                       |    4  | 0-15         | 4-19 |
| 3            | 4^1 + 4^2                 |   20  | 0-63         | 20-83 |
| 4            | 4^1 + 4^2 + 4^3           |   84  | 0-255        | 84-339 |
| 5            | 4^1 + 4^2 + 4^3 + 4^4     |  340  | 0-1023       | 340-1363 |
| ...          | ...                       |  ...  | ...          | ... |

### Many Ciphers (TCCQN)

| Cipher Count | generic start             | start | index range  | value range |
| -            | -                         | -     | -            | - |
| 1            | 1                         |    1  | 0-3          | 1-4 |
| 2            | 1 + 4^1                   |    5  | 0-15         | 5-20 |
| 3            | 1 + 4^1 + 4^2             |   21  | 0-63         | 21-84 |
| 4            | 1 + 4^1 + 4^2 + 4^3       |   85  | 0-255        | 85-340 |
| 5            | 1 + 4^1 + 4^2 + 4^3 + 4^4 |  341  | 0-1023       | 341-1364 |
| ...          | ...                       |  ...  | ...          | ... |

## Zeroes replacements with Z sigil bytes

### One Cipher 

|Z                 | Z1 | Z2 | Z3 | Z4 | remark |
|-                 | -  | -  | -  | -  | - |
|value count       |  1 |  2 |  3 |  4 | count = 0 + number |
|quaternary number |  0 |  1 |  2 |  3 | [Quaternary_numeral_system](https://en.wikipedia.org/wiki/Quaternary_numeral_system)|
|decimal    number |  0 |  1 |  2 |  3 | |

### Two ciphers

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

## Examples

| decoded           | encoded        | remark |
| -                 | -              | - |
| xx 00 xx          | xx Z1 xx       | count = 1 + S1, S1 = 0*4^0 |
| xx 00 00 00 00 xx | xx Z4 xx       | count = 1 + S4, S4 = 3*4^0 |
| xx 00 * 5  xx     | xx Z1Z1 xx     | count = 1 + S1S1, S1S1 = 1\*4^1 + 1\*4^0 |
| xx 00 * 20 xx     | xx Z4Z4 xx     | count = 1 + 0f3 + 0f33, Z4 = 3\*4^1 + 3\*4^0 |
| xx FF xx          | xx FF xx       | count = 1 + S1, S1 = 0*4^0 |
| xx FF FF FF FF xx | xx F4 xx       | count = 1 + S4, S4 = 3*4^0 |
| xx FF * 5  xx     | xx FFFF xx     | count = 1 + S1S1, S1S1 = 1\*4^1 + 1\*4^0 |
| xx FF * 20 xx     | xx F4F4 xx     | count = 1 + 0f3 + 0f33, Z4 = 3\*4^1 + 3\*4^0 |
| xx AA xx          | xx AA xx       | single occurrence of a AA |
| xx AA AA xx       | xx AA AA xx    | count = 1 + S1 = 2 |
| xx AA AA AA xx    | xx AA R2 xx    | count = 1 + S2 = 3 |
| xx AA AA AA AA xx | xx AA R3 xx    | count = 1 + S3 = 4 |
| xx AA * 5  xx     | xx AA AA AA xx | count = 1 + S1S1 = 5 |
| xx AA * 6  xx     | xx AA AA R2 xx | count = 1 + S1S2 = 6 |
| xx AA * 20 xx     | xx AA R4 R4 xx | count = 1 + 0f3 + 0f33, Z4 = 3\*4^1 + 3\*4^0 |



xx aa aa
xx aa aa aa  -> xx aa R2 -> so the aa before the first R is not counted inside the R-sequence. It stays for itself and serves then as R1.

## Algorithm

* Compute character count
* Determine Sigil sequence as S1-S4 ciphers
* Handle Offsets
