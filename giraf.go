package main

import (
	"encoding/csv"
	"io"
	"log"
	"os"
	"strconv"
	"time"
)

// Types ///////////////////////////////////////////////////////////////////////

// Settings contain generic settings given by the user.
type Settings struct {
	column     int
	skiprows   uint64
	outputrate time.Duration
	port       int
}

// Graph contains the values necessary to draw the data as a graph.
type Graph struct {
	width_px, height_px int32
	xwindow             uint64
	ymin, ymax          float64
}

// Globals /////////////////////////////////////////////////////////////////////

var (
	settings Settings = Settings{
		column:     1,
		skiprows:   1,
		outputrate: 10 * time.Millisecond,
		port:       8080,
	}
	graph Graph = Graph{
		width_px:  600,
		height_px: 400,
		xwindow:   200,
		ymin:      -3.0,
		ymax:      3.0,
	}
)

// Go-routines /////////////////////////////////////////////////////////////////

func handler(datachannel chan float64, svgchannel chan string) {
	var x []float64
	var y []float64
	var n uint64 = 0

	tnext := time.Now()
	for {
		// Check for input
		select {
		case value := <-datachannel:
			if n < graph.xwindow {
				x = append(x, float64(n))
				y = append(y, value)
			} else {
				y = append(y[1:], value)
			}

			n += 1
		}

		// Update the output at the output rate
		tnow := time.Now()
		if time.Now().After(tnext) {
			svg, err := dataToSvg(x, y)
			if err != nil {
				log.Fatal(err)
				continue
			}

			svgchannel <- svg
			tnext = tnow.Add(settings.outputrate)
		}
	}
}

// Main ////////////////////////////////////////////////////////////////////////

func main() {
	reader := csv.NewReader(os.Stdin)
	reader.ReuseRecord = true

	var rowcount uint64 = 0

	datachannel := make(chan float64)
	svgchannel := make(chan string)

	go handler(datachannel, svgchannel)
	go server(svgchannel)

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

		datachannel <- value
	}
}
