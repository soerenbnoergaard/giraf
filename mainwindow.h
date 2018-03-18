#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "datavector.h"

#include <QMainWindow>
#include <QTimer>
#include <QFile>
#include <QTextStream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadCsvFile(QString filename, bool *ok);
    void loadPipeInput();
    void setNumberOfRowsToSkip(int num_rows);
    void setHeaderRow(int row_number);
    void setXAxisColumn(int column_number);
    void setDelimiter(QString this_delimiter);
    void setMarker(char this_marker);
    void setLineEnable(bool on);
    void setXLimits(double xmin, double xmax);
    void setYLimits(double ymin, double ymax);
    void addColumn(int c);
    void updateGraph();
    void start();

private slots:
    void reset();
    void save();
    void copy();
    void on_timeout();

private:
    Ui::MainWindow *ui;

    // Settings
    QString delimiter = ",";
    char marker_symbol = ' ';
    int skiprows = 0;
    int headerrow = 0;
    int x_axis_column = -1;
    bool line_enabled = true;
    double xlim[2] = {0.0, 0.0};
    double ylim[2] = {0.0, 0.0};

    // File and data
    QFile inputfile;
    QTextStream *inputfileTextStream;
    QList<DataVector *> columns;
    int row_counter = 0;

    void initializeTimer();
};

#endif // MAINWINDOW_H
