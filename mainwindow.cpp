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

void MainWindow::loadCsvFile(QString fileName)
{
    // Open a file for reading (non-blocking)
    inputFile.setFileName(fileName);
    inputFile.open(QIODevice::ReadOnly);
    inputTextStream = new QTextStream(&inputFile);
}

void MainWindow::addPoint(double x, double y)
{
    x_data.append(x);
    y_data.append(y);
}

void MainWindow::clearData()
{
    x_data.clear();
    y_data.clear();
}

void MainWindow::plot()
{
    ui->plot->graph(0)->setData(x_data, y_data);
    ui->plot->update();
    ui->plot->rescaleAxes();
    ui->plot->replot();
}

void MainWindow::on_timeout()
{
    static uint32_t x = 0;
    static uint8_t y = 0;
    bool ok;

    y = inputTextStream->readLine().toDouble(&ok);

    if (ok) {
        addPoint(x, y);
        plot();
        x += 1;
    }
}

void MainWindow::initializePlot()
{
    ui->setupUi(this);
    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsLine);
}

void MainWindow::initializeTimer()
{
    QTimer *update_timer = new QTimer(this);
    update_timer->start(1.0); // [ms]
    connect(update_timer, SIGNAL(timeout()), SLOT(on_timeout()));
}
