#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // Argument parser
    QCommandLineParser parser;
    parser.setApplicationDescription("Description: Real-time plotting of a CSV file.");

    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("file", QCoreApplication::translate("main", "CSV file to open."));
    parser.addOption(QCommandLineOption({"c", "column"}, "column to plot (indexed from 0)", "0"));

    parser.process(a);
    const QStringList args = parser.positionalArguments();

    // Extract columns to plot
    foreach (QString s, parser.values("column")) {
        w.addColumn(s.toInt());
    }

    if (args.length() > 0) {
        bool ok;

        // Load input file
        w.loadCsvFile(args.at(0), &ok);
        if (!ok)
            goto failed;

        w.show();
        return a.exec();
    }

failed:
    return 1;
}
