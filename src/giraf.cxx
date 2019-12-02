#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Chart.H>
#include <FL/fl_draw.H>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

static struct {
    int window_size = 100;
    double ymin = -10.0;
    double ymax = 10.0;
} arg;

double sample()
{
    return 2.0 + 1.0*rand()/RAND_MAX;
}

void update(Fl_Chart *chart)
{
    chart->add(sample(), NULL, FL_BLUE);
}

void grid(Fl_Chart *chart)
{
    int y;
    for (y = 0; y < 10; y += chart->h()/10) {
        fl_line(chart->x(),
                chart->y() + y,
                chart->x() + chart->w(),
                chart->y() + y);
    }
}

int main(int argc, char **argv)
{
    Fl_Window *win = new Fl_Window(1000, 480);
    Fl_Chart *chart0 = new Fl_Chart(20, 20, win->w()-40, win->h()-40, NULL);

    chart0->type(FL_LINE_CHART);
    chart0->bounds(arg.ymin, arg.ymax);
    // chart0->autosize(true);
    chart0->color(FL_WHITE);
    chart0->maxsize(arg.window_size);

    win->resizable(win);
    win->show();

    // grid(chart0);


    while (Fl::check()) {
        update(chart0);
        usleep(50000);
    }
    return Fl::run();
}

