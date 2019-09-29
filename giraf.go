package main

import (
	"encoding/csv"
	"errors"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
	"time"
)

// Types ///////////////////////////////////////////////////////////////////////

// Output determines the destination for the SVG file.
type Output int
const (
    FILE Output = iota
    SERVER
)

// Settings contain generic settings given by the user.
type Settings struct {
    output     Output
	column     int
	outfile    string
	skiprows   uint64
	outputrate time.Duration
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
        output:     SERVER,
		column:     1,
		outfile:    "output.svg",
		skiprows:   1,
		outputrate: 50 * time.Millisecond,
	}
	graph Graph = Graph{
		width_px:  600,
		height_px: 400,
		xwindow:   200,
		ymin:      0.0,
		ymax:      1.0,
	}
)

// Functions ///////////////////////////////////////////////////////////////////

// xscale transforms the value from the data domain to the pixel domain for
// drawing.
func xscale(p float64) int32 {
	p *= float64(graph.width_px)
	p /= float64(graph.xwindow)
	return int32(p)
}

// yscale transforms the value from the data domain to the pixel domain for
// drawing. Note that the positive pixel direction is downwards!
func yscale(p float64) int32 {
	p -= graph.ymin
	p *= float64(graph.height_px)
	p /= (graph.ymax - graph.ymin)
	return graph.height_px - int32(p)
}

func dataToSvgPath(x, y []float64) (string, error) {
	if len(x) != len(y) {
		return "", errors.New("len(x) != len(y)")
	} else if len(x) == 0 {
		return "", nil
	}

	s := fmt.Sprintf("M %d %d ", xscale(x[0]), yscale(y[0]))
	for n := 1; n < len(x); n++ {
		s += fmt.Sprintf("L %d %d ", xscale(x[n]), yscale(y[n]))
	}
	return s, nil
}

func dataToSvg(x, y []float64) (string, error) {
    output := ""

	output += fmt.Sprintf("<svg xmlns='http://www.w3.org/2000/svg' width='%d' height='%d'>", graph.width_px, graph.height_px)

	xmin_px := xscale(0.0)
	xmax_px := xscale(float64(graph.xwindow))
	ymin_px := yscale(graph.ymin)
	ymax_px := yscale(graph.ymax)
	output += fmt.Sprintf("<path d='M %d %d L %d %d L %d %d L %d %d Z' fill='#ffffff' stroke='#000000' />",
		xmin_px, ymin_px,
		xmax_px, ymin_px,
		xmax_px, ymax_px,
		xmin_px, ymax_px,
	)

    output += "<path d='"
    s, err := dataToSvgPath(x, y)
    if err != nil {
        return "", err
    }
    output += s
    output += "' stroke='#0000ff' fill-opacity='0.0'/>\n"

	output += "</svg>"
	return output, nil
}

func writeFile(s string) (err error) {
	f, err := os.Create(settings.outfile)
	if err != nil {
		return fmt.Errorf("Could not open file %s", settings.outfile)
	}

	defer f.Close()

    _, err = f.Write([]byte(s))

    return err
}

// Go-routines /////////////////////////////////////////////////////////////////

func outputHandler(datachannel chan float64, svgchannel chan string) {
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

            switch settings.output {
            case FILE:
                err = writeFile(svg)
                if err != nil {
                    log.Fatal(err)
                    continue
                }

            case SERVER:
                svgchannel <-svg
            }

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

	go outputHandler(datachannel, svgchannel)
    if settings.output == SERVER {
        go server(svgchannel)
    }

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
