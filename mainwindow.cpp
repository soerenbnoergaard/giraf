#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsLine);

    QTimer *update_timer = new QTimer(this);
    update_timer->start(10.0);
    connect(update_timer, SIGNAL(timeout()), SLOT(on_update_timer_timeout()));
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::on_update_timer_timeout()
{
    static uint32_t x = 0;
    static uint8_t y = 0;

    addPoint(x, y);
    plot();

    x += 1;
    y += 1;
}
