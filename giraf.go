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

// Settings contain generic settings given by the user.
type Settings struct {
	column   int
	outfile  string
	skiprows uint64
}

// Graph contains the values necessary to draw the data as a graph.
type Graph struct {
	width_px, height_px int32
	xmin, xmax          float64
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
		width_px:  800,
		height_px: 400,
		xmin:      0.0,
		xmax:      10.0,
		ymin:      10.0,
		ymax:      30.0,
	}
)

// Functions ///////////////////////////////////////////////////////////////////

// xscale transforms the value from the data domain to the pixel domain for
// drawing.
func xscale(p float64) int32 {
    p -= graph.xmin
    p *= float64(graph.width_px)
    p /= (graph.xmax - graph.xmin)
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

func write(path string) (err error) {
	f, err := os.Create(settings.outfile)
	if err != nil {
		return fmt.Errorf("Could not open file %s", settings.outfile)
	}

	defer f.Close()

	var header = fmt.Sprintf("<svg width='%d' height='%d'>", graph.width_px, graph.height_px)
	_, err = f.Write([]byte(header + "\n"))
	if err != nil {
		return err
	}

    xmin_px := xscale(graph.xmin)
    xmax_px := xscale(graph.xmax)
    ymin_px := yscale(graph.ymin)
    ymax_px := yscale(graph.ymax)
    var box = fmt.Sprintf("<path d='M %d %d L %d %d L %d %d L %d %d Z' fill='#ffffff' />",
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
		var prefix byte
		if valuecount == 0 {
            prefix = 'M'
		} else {
            prefix = 'L'
		}
		path += fmt.Sprintf("%c %d %d ", prefix, xscale(float64(valuecount)), yscale(value))
		valuecount += 1
	}
	write(path)
}
