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

    void loadCsvFile(QString fileName, bool *ok);

    void addColumn(int c);
    void plot();

private slots:
    void on_timeout();

private:
    Ui::MainWindow *ui;

    // Setup
    QFile inputFile;
    QString delimiter = ",";

    QList<DataVector *> columns;

    // Handles
    QTextStream *inputTextStream;

    void initializeTimer();
};

#endif // MAINWINDOW_H
