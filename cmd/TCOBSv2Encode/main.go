package main

import (
	"fmt"
	"io"
	"log"
	"os"

	tcobs "github.com/rokath/tcobs/Cv2"
	"github.com/spf13/afero"
)

var (
	// do not initialize, goreleaser will handle that
	version string

	// do not initialize, goreleaser will handle that
	commit string

	// do not initialize, goreleaser will handle that
	date string
)

// main is the entry point.
func main() {
	fSys := &afero.Afero{Fs: afero.NewOsFs()} // os.DirFS("")
	doit(os.Stdout, fSys)
}

func doit(w io.Writer, fSys *afero.Afero) {

	if len(os.Args) != 1 {
		fmt.Fprintln(w, version, commit, date)
		fmt.Fprintln(w, "Feed with a space separated byte sequence to convert it in a TCOBSv1 sequence.")
		fmt.Fprintln(w, "Example: `echo 0xaa 123 0b1010 44 44 44 | TCOBSv1Encode` will return `170 123 10 44 132`")
		return
	}

	var i []byte
	o := make([]byte, 16*1024)
	var pos int
	for {
		var n byte
		_, err := fmt.Scan(&n)
		if err == io.EOF {
			if len(i)+31/len(i) > len(o) {
				log.Fatal("len of internal buffer too small")
			}
			count := tcobs.CEncode(o, i)
			o = o[:count]
			for _, b := range o {
				fmt.Fprintf(w, "%d ", b)
			}
			return
		}
		if err != nil {
			log.Fatal(err, " at position ", pos)
		}
		i = append(i, n)
		pos++
	}
}
