package main

import (
	"bufio"
	"flag"
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

	iFn string

	oFn = ".TCOBSv1"

	verbose = true
)

func init() {
	flag.StringVar(&iFn, "i", "-", "input file name")
	flag.Parse()
	if iFn != "_" {
		oFn = iFn + oFn
	}
}

// main is the entry point.
func main() {
	fSys := &afero.Afero{Fs: afero.NewOsFs()}
	doit(os.Stdout, fSys)
}

func doit(w io.Writer, fSys *afero.Afero) {

	if len(os.Args) != 3 {
		fmt.Fprintln(w, version, commit, date)
		fmt.Fprintln(w, "Usage: TCOBSv1FileCEncode -i inputFileName")
		fmt.Fprintln(w, "Example: `TCOBSv1FileCEncode -i fn` creates fn.TCOBSv1")
		return
	}

	ih, err := fSys.Open(iFn)
	if err != nil {
		log.Fatal(err)
	}
	defer ih.Close()

	oh, err := fSys.Create(oFn)
	if err != nil {
		log.Fatal(err)
	}
	defer oh.Close()

	// Get the file size
	stat, err := ih.Stat()
	if err != nil {
		fmt.Println(err)
		return
	}
	iSize := int(stat.Size())

	// Read the file into a byte slice
	i := make([]byte, iSize)
	_, err = bufio.NewReader(ih).Read(i)
	if err != nil && err != io.EOF {
		fmt.Println(err)
		return
	}
	o := make([]byte, len(i)+len(i)/32+1)

	n0 := tcobs.CEncode(o, i)
	o = o[:n0]
	n1, err := oh.Write(o)
	if err != nil {
		log.Fatal(err)
	}
	if n1 != n0 {
		log.Fatal(n1, "written bytes not equal", n0)
	}
	if verbose {
		fmt.Println(iSize, "->", n0, "ratio", n0*100/iSize, "%")
	}
}
