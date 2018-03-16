#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

    void loadCsvFile(QString fileName, bool *ok);

    void addColumn(int c);
    void addPoint(int index, double x, double y);
    void plot();

private slots:
    void on_timeout();

private:
    Ui::MainWindow *ui;

    // Setup
    QFile inputFile;
    QVector<int> columns;
    QString delimiter = ",";

    // Data

    // TODO: Make x and y containers multidimensional
    QVector<double> x_data;
    QVector<double> y_data;

    // Handles
    QTextStream *inputTextStream;

    void initializeTimer();
};

#endif // MAINWINDOW_H
