#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

int main(int argc, char **argv)
{
    Fl_Window *window = new Fl_Window(670,300);
    Fl_Box *box;

    box = new Fl_Box(0, 0, 670, 300, "Giraf");
    box->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    window->end();
    window->show(argc, argv);
    return Fl::run();
}
