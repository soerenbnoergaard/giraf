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
    void setNumberOfRowsToSkip(int num_rows);
    void setHeaderRow(int row_number);
    void setXAxisColumn(int column_number);
    void setDelimiter(QString this_delimiter);
    void addColumn(int c);
    void updateGraph();
    void start();

private slots:
    void reset();
    void save();
    void on_timeout();

private:
    Ui::MainWindow *ui;

    // Settings
    QString delimiter = ",";
    int skiprows = 0;
    int headerrow = -1;
    int x_axis_column = -1;

    // File and data
    QFile inputfile;
    QTextStream *inputfileTextStream;
    QList<DataVector *> columns;
    int row_counter = 0;

    void initializeTimer();
};

#endif // MAINWINDOW_H
