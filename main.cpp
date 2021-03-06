#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>

static void initialize_arguments(QCommandLineParser *parser)
{
    parser->setApplicationDescription("Description: Real-time plotting of a CSV file.");

    parser->addHelpOption();

    parser->addPositionalArgument("file", QCoreApplication::translate("main", "CSV file to open."));
    parser->addOption(QCommandLineOption({"y", "ycolumn"}, "column to plot (indexed from 1)", " "));
    parser->addOption(QCommandLineOption({"x", "xcolumn"}, "column to use as x-axis (indexed from 1, 0 to disable)", "0"));
    parser->addOption(QCommandLineOption({"s", "skiprows"}, "number of rows to skip before data", "0"));
    parser->addOption(QCommandLineOption({"l", "legendrow"}, "row containing the column names for the legend (indexed from 1, 0 to disable)", "1"));
    parser->addOption(QCommandLineOption({"d", "delimiter"}, "column delimiter", ","));
    parser->addOption(QCommandLineOption({"m", "marker"}, "data marker symbol (matlab-like syntax)", " "));
    parser->addOption(QCommandLineOption({"n", "noline"}, "disable line and plot only markers"));
    parser->addOption(QCommandLineOption({"w", "window"}, "windowed plotting (strip chart) showing only the most recent values (only avalilable without the xcolumn option)", "0"));
    parser->addOption(QCommandLineOption("xlim", "fixed x-axis limits (format: \"min,max\")", "\"0,0\""));
    parser->addOption(QCommandLineOption("ylim", "fixed y-axis limits (format: \"min,max\")", "\"0,0\""));
}

static void argument_warning(QString parameter)
{
    qWarning() << "Incorrect argument for parameter:" << parameter;
}

static void apply_arguments(QCommandLineParser *parser, MainWindow *w, bool *ok)
{
    int n;

    // file (positional)
    const QStringList positional_args = parser->positionalArguments();
    if (positional_args.length() > 0) {
        w->loadCsvFile(positional_args.at(0), ok);
    }
    else {
        w->loadPipeInput();
        *ok = true;
    }

    // -d|--delimiter
    if (parser->isSet("delimiter")) {
        w->setDelimiter(parser->value("delimiter"));
    }

    // -m|--marker
    if (parser->isSet("marker")) {
        w->setMarker(parser->value("marker").toStdString()[0]);
    }

    // -n|--noline
    if (parser->isSet("noline")) {
        w->setLineEnable(false);
        if (!parser->isSet("marker"))
            w->setMarker('o');
    }

    // -s|--skiprows
    if (parser->isSet("skiprows")) {
        n = parser->value("skiprows").toInt();
        if (n > 0)
            w->setNumberOfRowsToSkip(n);
        else
            argument_warning("skiprows");
    }

    // -w|--window
    if (parser->isSet("window")) {
        n = parser->value("window").toInt();
        if (n > 0)
            w->setWindowSize(n);
        else
            argument_warning("window");
    }

    // --xlim
    if (parser->isSet("xlim")) {
        QStringList l = parser->value("xlim").split(",");
        if (l.length() == 2)
            w->setXLimits(l[0].toDouble(), l[1].toDouble());
        else
            argument_warning("xlim");
    }

    // --ylim
    if (parser->isSet("ylim")) {
        QStringList l = parser->value("ylim").split(",");
        if (l.length() == 2)
            w->setYLimits(l[0].toDouble(), l[1].toDouble());
        else
            argument_warning("ylim");
    }

    //
    // From the user's perspective, indexing is done from 1. Internally in C++,
    // the indexing is done from 0. That is the reason for the "- 1" below.
    //

    // -l|--legendrow
    if (parser->isSet("legendrow")) {
        n = parser->value("legendrow").toInt();
        if (n >= 0)
            w->setHeaderRow(n - 1);
        else
            argument_warning("legendrow");
    }

    // -x|--xcolumn
    if (parser->isSet("xcolumn")) {
        n = parser->value("xcolumn").toInt();
        if (n >= 0)
            w->setXAxisColumn(n - 1);
        else
            argument_warning("xcolumn");
    }

    // -y|--ycolumn
    foreach (QString s, parser->values("ycolumn")) {
        n = s.toInt();
        if (n > 0)
            w->addColumn(n - 1);
        else
            argument_warning("ycolumn");
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    bool ok;

    // Argument parser
    QCommandLineParser parser;

    initialize_arguments(&parser);
    parser.process(a);
    apply_arguments(&parser, &w, &ok);

    if (ok) {
        w.start();
        w.show();
        return a.exec();
    }
    else {
        return 1;
    }
}
