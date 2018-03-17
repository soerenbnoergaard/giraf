#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

static void initialize_arguments(QCommandLineParser *parser)
{
    parser->setApplicationDescription("Description: Real-time plotting of a CSV file.");

    parser->addHelpOption();
    parser->addVersionOption();

    parser->addPositionalArgument("file", QCoreApplication::translate("main", "CSV file to open."));
    parser->addOption(QCommandLineOption({"c", "column"}, "column to plot (indexed from 0)", " "));
    parser->addOption(QCommandLineOption({"s", "skiprows"}, "number of rows to skip before data", "0"));
    parser->addOption(QCommandLineOption({"l", "legendrow"}, "row containing the column names for the legend (indexed from 0)", "-1"));
    parser->addOption(QCommandLineOption({"d", "delimiter"}, "column delimiter", ","));
    parser->addOption(QCommandLineOption({"x", "xcolumn"}, "column to use as x-axis (indexed from 0)", "-1"));
}

static void apply_arguments(QCommandLineParser *parser, MainWindow *w, bool *ok)
{
    // file (positional)
    const QStringList positional_args = parser->positionalArguments();
    if (positional_args.length() > 0)
        w->loadCsvFile(positional_args.at(0), ok);

    // -c|--column
    foreach (QString s, parser->values("column"))
        w->addColumn(s.toInt());

    // -s|--skiprows
    if (parser->isSet("skiprows"))
        w->setNumberOfRowsToSkip(parser->value("skiprows").toInt());

    // -l|--legendrow
    if (parser->isSet("legendrow"))
        w->setHeaderRow(parser->value("legendrow").toInt());

    // -d|--delimiter
    if (parser->isSet("delimiter"))
        w->setDelimiter(parser->value("delimiter"));

    // -x|--xcolumn
    if (parser->isSet("xcolumn"))
        w->setXAxisColumn(parser->value("xcolumn").toInt());
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
