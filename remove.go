package tcobs

// THIS CODE IS TESTED BUT WILL BE REMOVED IN THE FUTURE (bad design)

import "bytes"

// Decoder expects in s a 0-delimited number of TCOBS packages. If a 0-delimiter is found inside s,
// it tries to decode all bytes until the 0-delimiter as TCOBS package and writes the result in decoded.
// If s does not contain any 0-delimiter, decoded is nil, after is s and e is nil.
// If a 0-delimiter was found, inside 'after' all data after the found delimiter.
// If the TCOBS decoding of the data until the 0-delimiter failed, decoded has len 0, and e is not nil.
// For details see TCOBSSpecification.md.
func Decoder(s []byte) (decoded, after []byte, e error) {
	tcobs, after, found := bytes.Cut(s, []byte{0})
	if !found {
		after = s
		return
	}
	decoded = make([]byte, 2*len(tcobs))
	n, e := Decode(decoded, tcobs)
	decoded = decoded[len(decoded)-n:]
	return
}
