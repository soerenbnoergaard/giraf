package main

import (
	"encoding/csv"
	"errors"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
)

// Types ///////////////////////////////////////////////////////////////////////

// Settings contain generic settings given by the user.
type Settings struct {
	column   int
	outfile  string
	skiprows uint64
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
		column:   1,
		outfile:  "output.svg",
		skiprows: 1,
	}
	graph Graph = Graph{
		width_px:  600,
		height_px: 400,
		xwindow:   100,
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

func toSvgPath(x, y []float64) (string, error) {
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

func write(path string) (err error) {
	f, err := os.Create(settings.outfile)
	if err != nil {
		return fmt.Errorf("Could not open file %s", settings.outfile)
	}

	defer f.Close()

	var header = fmt.Sprintf("<svg xmlns='http://www.w3.org/2000/svg' width='%d' height='%d'>", graph.width_px, graph.height_px)
	_, err = f.Write([]byte(header + "\n"))
	if err != nil {
		return err
	}

	xmin_px := xscale(0.0)
	xmax_px := xscale(float64(graph.xwindow))
	ymin_px := yscale(graph.ymin)
	ymax_px := yscale(graph.ymax)
	var box = fmt.Sprintf("<path d='M %d %d L %d %d L %d %d L %d %d Z' fill='#ffffff' stroke='#000000' />",
		xmin_px, ymin_px,
		xmax_px, ymin_px,
		xmax_px, ymax_px,
		xmin_px, ymax_px,
	)
	_, err = f.Write([]byte(box + "\n"))
	if err != nil {
		return err
	}

	_, err = f.Write([]byte("<path d='" + path + "' stroke='#0000ff' fill-opacity='0.0'/>\n"))
	if err != nil {
		return err
	}

	var footer = "</svg>"
	_, err = f.Write([]byte(footer + "\n"))
	if err != nil {
		return err
	}

	return nil
}

// Main ////////////////////////////////////////////////////////////////////////

func main() {
	reader := csv.NewReader(os.Stdin)
	reader.ReuseRecord = true

	var rowcount uint64 = 0
	var valuecount uint64 = 0

	// Windowed data window (oscilloscope-like)
	var x, y []float64

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

		if valuecount < graph.xwindow {
			x = append(x, float64(valuecount))
			y = append(y, value)
		} else {
			y = append(y[1:], value)
		}

		path, err := toSvgPath(x, y)
		if err != nil {
			log.Fatal("Error converting x, y to SVG path string")
			return
		}
		write(path)

		valuecount += 1
	}
}
