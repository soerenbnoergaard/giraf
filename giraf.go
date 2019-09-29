package main

import (
	"encoding/csv"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
)

// Types ///////////////////////////////////////////////////////////////////////

// Settings contain generic settings given by the user
type Settings struct {
	column   int
	outfile  string
	skiprows uint64
}

type Graph struct {
    width_px, height_px int32
}

// Globals /////////////////////////////////////////////////////////////////////
var (
    settings Settings = Settings{
        column: 1,
        outfile: "output.svg",
        skiprows: 1,
    }
    graph Graph = Graph{
        width_px: 200,
        height_px: 100,
    }
)

// Private functions ///////////////////////////////////////////////////////////
func Write(path string) (err error) {
	f, err := os.Create(settings.outfile)
	if err != nil {
		return fmt.Errorf("Could not open file %s", settings.outfile)
	}

	defer f.Close()

    var header = fmt.Sprintf("<svg width='%d' height='%d'>", graph.width_px, graph.height_px)
    var footer = "</svg>"

    _, err = f.Write([]byte(header + "\n"))
    if err != nil {
        return err
    }

	_, err = f.Write([]byte(
        "<path d='" +
        path +
        "' color='#0000ff'/>\n"))
    if err != nil {
        return err
    }

    _, err = f.Write([]byte(footer + "\n"))
    if err != nil {
        return err
    }

	return nil
}

// Main ////////////////////////////////////////////////////////////////////////
func main() {
	reader := csv.NewReader(os.Stdin)

	var rowcount uint64 = 0
	var valuecount uint64 = 0
	var path string = ""

	for {
		input, err := reader.Read()
		if err == io.EOF {
			break
		}
		if err != nil {
			log.Fatal(err)
            continue
		}

		rowcount += 1
		if rowcount < settings.skiprows {
			continue
		}

		value, err := strconv.ParseFloat(input[settings.column], 64)
		if err != nil {
			continue
		}

		// println(value)
		var s string
		if valuecount == 0 {
			s = fmt.Sprintf("M %d %f ", valuecount, value)
		} else {
			s = fmt.Sprintf("L %d %f ", valuecount, value)
		}
		path += s

		valuecount += 1
	}
    Write(path)
}
