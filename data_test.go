package tcobs_test

const testBufferSize = 64 * 1024

type testTable []struct {
	dec []byte
	enc []byte
}

var testData = testTable{

	{[]byte{}, []byte{}},
	{[]byte{0}, []byte{0x20}},
	{[]byte{0, 0}, []byte{0x60}},
	{[]byte{0, 0, 0}, []byte{0x50}},
	{[]byte{0, 0, 0, 0}, []byte{0x10}},
	{[]byte{0, 0, 0, 0, 0}, []byte{0x20, 0x20}},
	{[]byte{9}, []byte{9, 0xA1}},
	{[]byte{9, 9}, []byte{9, 9, 0xA2}},
	{[]byte{9, 9, 9}, []byte{9, 0x81}},
	{[]byte{9, 9, 9, 9}, []byte{9, 0x41}},
	{[]byte{9, 9, 9, 9, 9}, []byte{9, 0x02}},
	{[]byte{9, 9, 9, 9, 9, 9}, []byte{9, 0x81, 0x80}},
	{[]byte{0xFF}, []byte{0xFF, 0xA1}},
	{[]byte{0xFF, 0xFF}, []byte{0xC0}},
	{[]byte{0xFF, 0xFF, 0xFF}, []byte{0xE0}},
	{[]byte{0xFF, 0xFF, 0xFF, 0xFF}, []byte{0xF0}},
	{[]byte{0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, []byte{0xFF, 0xFF}},
}

// createEncodedStream concatenates all trestData enc sequences with 0-delimiters into one byte slice.
func createEncodedStream() []byte {
	enc := make([]byte, 0, testBufferSize)
	for _, k := range testData {
		enc = append(enc, k.enc...)
		enc = append(enc, 0)
	}
	return enc
}

// createDecodedStream concatenates all trestData dec sequences with 0-delimiters into one byte slice.
func createDecodedStream() []byte {
	dec := make([]byte, 0, testBufferSize)
	for _, k := range testData {
		dec = append(dec, k.dec...)
	}
	return dec
}

//  // printAsGoCode prints x for easy copy & paste into test table.
//  func printAsGoCode(x []byte) {
//  	fmt.Print("[]byte{")
//  	for _, b := range x {
//  		fmt.Printf("0x%02x, ", b)
//  	}
//  	fmt.Println("}")
//  }
