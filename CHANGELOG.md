# TCOBS Changelog

## v0.8.0

* TCOBSv2 Read function more robust against errors.

## v0.7.1

* TCOBSv1 & TCOBSv2 README.md files corrected.

## v0.7.0

* TCOBSv1 **Go** interface improved and more tested
* TCOBSv2 **Go** interface improved and more tested

## v0.6.1

* TCOBSv2 **Go** interface improved and tested

## v0.6.0

* TCOBSv2 now faster with internal look-up tables.
* TCOBSv1 passes all tests and is is unlimited.
* TCOBSv2 passes all tests and is is unlimited.

## v0.5.0

* TCOBSv1 C function decoder passes first tests and is is unlimited.
  * More tests needed.

## v0.4.0

* TCOBSv2 passes all tests and is is unlimited.
  * More tests needed.

## v0.3.0

* Code divided into TCOBSv1 and TCOBSv2
* TCOBSv1 passes all tests,and is limited:
  * Encoding implemented in C only
  * Decoding implemented in Go only
* TCOBSv2 passes all tests, but is limited:
  * Not more than 20 00-bytes in a row
  * Not more than 20 FF-bytes in a row
  * Not more than 40 equal other bytes in a row

## v0.2.0

### Added or Changed

* Code in tcobs.go, tcobs.h, tcobs.c is extensively tested (see tcobs_test.go)
* Code in remove*.* is tested but should not be used.
* Code in experimental is WIP (work in progress) and NOT stable.

## v0.1.0

### Added or Changed

* [./README.md](./README.md) improved.

## v0.0.0

### Added or Changed

- Code and Doc moved from [*Trice*](https://github.com/rokath/trice) project to this repo.

<!--
- Added this changelog :)
- Fixed typos in both templates
- Back to top links
- Added more "Built With" frameworks/libraries
- Changed table of contents to start collapsed
- Added checkboxes for major features on roadmap
-->

<!--### Removed

- Some packages/libraries from acknowledgements I no longer use
-->
