package main

import (
	"errors"
	"fmt"
)

// xscale transforms a data point into a pixel for the drawing.
func xscale(p float64) int32 {
	p *= float64(graph.width_px)
	p /= float64(graph.xwindow)
	return int32(p)
}

// yscale transforms a data point into a pixel for the drawing.
func yscale(p float64) int32 {
	p -= graph.ymin
	p *= float64(graph.height_px)
	p /= (graph.ymax - graph.ymin)
	return graph.height_px - int32(p)
}

// dataToSvgPath converts a set of data points to a string for
// <path d="" />.
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

// dataToSvg generates an SVG graph for a set of data points
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
