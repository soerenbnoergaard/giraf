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

    void loadCsvFile(QString fileName);

    void addPoint(double x, double y);
    void clearData();
    void plot();

private slots:
    void on_timeout();

private:
    Ui::MainWindow *ui;

    QVector<double> x_data, y_data;
    QFile inputFile;
    QTextStream *inputTextStream;

    void initializePlot();
    void initializeTimer();
};

#endif // MAINWINDOW_H
