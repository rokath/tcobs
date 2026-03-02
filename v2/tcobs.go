package tcobsv2

// #include <stdint.h>
// #include "tcobs.h"
// #include "tcobsInternal.h"
// #cgo CFLAGS: -g -Wall
import "C"
import (
	"unsafe"
)

// todo: -flto switch does not work on some Windows (Antivirus?) setups.

// CEncode encodes i into o and returns the number of written bytes.
// For details see https://github.com/rokath/tcobs/blob/master/docs/TCOBSv2Specification.md.
// The CEncode implementation is done in C because the aimed use case is an embedded device running C.
func CEncode(o, i []byte) (n int) {
	if len(i) == 0 {
		return
	}
	in := unsafe.Pointer(&i[0])
	out := unsafe.Pointer(&o[0])
	n = int(C.TCOBSEncode(out, in, C.size_t(len(i))))
	return
}

// CDecode decodes i into o and returns the decoded length.
// Decoded bytes are located at o[len(o)-n:].
// For details see https://github.com/rokath/tcobs/blob/master/docs/TCOBSv2Specification.md.
// The CDecode implementation is done in C because the aimed use case is an embedded device running C.
func CDecode(o, i []byte) (n int) {
	if len(i) == 0 {
		return
	}
	in := unsafe.Pointer(&i[0])
	out := unsafe.Pointer(&o[0])
	n = int(C.TCOBSDecode(out, C.size_t(len(o)), in, C.size_t(len(i))))
	return
}
