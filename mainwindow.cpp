#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    initializePlot();
    initializeTimer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadCsvFile(QString fileName, bool *ok)
{
    // Open a file for reading (non-blocking)
    *ok = QFile::exists(fileName);
    if (ok) {
        inputFile.setFileName(fileName);
        inputFile.open(QIODevice::ReadOnly);
        inputTextStream = new QTextStream(&inputFile);
    }
}

void MainWindow::addColumn(int c)
{
    columns.append(c);

    int i = columns.indexOf(c);
    ui->plot->addGraph();
    ui->plot->graph(i)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->plot->graph(i)->setLineStyle(QCPGraph::lsLine);
}

void MainWindow::addPoint(int index, double x, double y)
{
    x_data.append(x);
    y_data.append(y);
}

void MainWindow::plot()
{
    foreach (int column, columns) {
        int i = columns.indexOf(column);
        ui->plot->graph(i)->setData(x_data, y_data);
    }

    ui->plot->update();
    ui->plot->rescaleAxes();
    ui->plot->replot();
}

void MainWindow::on_timeout()
{
    static uint32_t x = 0;
    static uint8_t y = 0;
    QString line;
    QStringList row;
    bool ok;

    line = inputTextStream->readLine();
    if (line.length() == 0)
        return;

    row = line.split(delimiter);

    foreach (int column, columns) {
        int i = columns.indexOf(column);

        if (row.length() < column-1) {
            qWarning() << "Column " << column << " not found in row: " << line;
        }
        y = row[column].toDouble(&ok);
        if (ok) {
            addPoint(i, x, y);
            plot();
            x += 1;
        } else {
            qWarning() << "Could not understand CSV line: " << line;
        }
    }
}

void MainWindow::initializePlot()
{
    ui->setupUi(this);
}

void MainWindow::initializeTimer()
{
    QTimer *update_timer = new QTimer(this);
    update_timer->start(1.0); // [ms]
    connect(update_timer, SIGNAL(timeout()), SLOT(on_timeout()));
}
