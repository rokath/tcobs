package main

import (
	"fmt"
	"io"
	"log"
	"os"

	tcobs "github.com/rokath/tcobs/v1"
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
		fmt.Fprintln(w, "Feed with a space separated byte sequence to decode a TCOBSv1 sequence.")
		fmt.Fprintln(w, "Example: `echo 64 1 2 18 88 25 88 161 | TCOBSv1Decode` will return `0 0 1 2 2 2 2 88 88 88 88 88 88`")
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
			count := tcobs.CDecode(o, i)
			if count < 0 {
				log.Fatal("invalid input data, error code ", count)
			}
			o = o[len(o)-count:]
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
