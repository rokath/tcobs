# Sigil Bytes sle


| Value 7-5 | Bits 7-0   | Hex Range| Byte Name       | Single Cipher Value | Sign  | Offset Bits      | Offset Value| Usage | Remark |
|    -      | -          | -        | -               | :-:                 | -     | -                | -           | -     | - |
|    0      | `1nnnnnnn` |  80 - FF | N               |                     |       |                  |             |       | used later as delimiter byte |
|    0      | `00nnnnnn` |  00 - 3F | S0              |                     |       |                  |             |       | used later as delimiter byte |
|    0      | `01nnnnnn` |  40 - 7F | S1              |                     |       |                  |             |       | used later as delimiter byte |

|X | Y|
|- | - |
|1 * aa | aa |
|2 * aa | aa aa | 
|3 * aa | aa S0 |
|4 * aa | aa S1 |
|5 * aa | aa S0 S0 |
|6 * aa | aa S0 S1 |
|7 * aa | aa S1 S0 |
|8 * aa | aa S1 S1 |
|9 * aa | aa S0 S0 S0 |
2...    | ... |
217 * aa | aa S1 S1 S1 |

worst case: + 1 per starting 127 (only n)
If at least 1 time 3 * aa within starting 127 it is equal
If at least 2 times 3 * aa or 1 time 4 * aa we are geting better.

A following COBS adds + 1 byte per starting 255 bytes.
In sum we have max + 3 bytes per starting 255 bytes resulting in + max 1.2 % mor data in worst case. 

## Altenative

`aa aa` - no run
`aa aa aa` - smallest run
N    `0nnnnnnn` - sigil byte for 0 to 127 bytes without a run
N0   `00000000`
N127 `01111111`
R    `1nnnnnnn` - sigil byte for 3 to 129 run bytes
R3   `10000000`
R129 `11111111`

   xx aa aa aa xx xx -> 
N1 xx R3 aa N2 xx xx

Especially short buffers are getting a +1 byte typically!
