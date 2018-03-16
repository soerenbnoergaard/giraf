#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    DataVector *d = new DataVector(c);
    columns.append(d);

    int i = columns.indexOf(d);

    ui->plot->addGraph();
    ui->plot->graph(i)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plot->graph(i)->setLineStyle(QCPGraph::lsLine);
}

void MainWindow::plot()
{
    for (int i = 0; i < columns.length(); i++) {
        ui->plot->graph(i)->setData(columns[i]->x, columns[i]->y);
    }

    ui->plot->update();
    ui->plot->rescaleAxes();
    ui->plot->replot();
}

void MainWindow::on_timeout()
{
    static uint32_t t = 0;
    static uint8_t y = 0;

    // Read a line from the input file
    QString line;
    line = inputTextStream->readLine();
    if (line.length() == 0)
        return;

    // Split the line into a row of cells
    QStringList row;
    row = line.split(delimiter);

    // Add columns to the plot
    for (int i = 0; i < columns.length(); i++) {
        DataVector *column = columns[i];

        // If the specified column is not present in the row, proceed to next row
        if (column->column_number > row.length()-1) {
            qWarning() << "Column" << column->column_number << "not found in row:" << line;
            continue;
        }

        // Convert string to number
        bool ok;
        y = row[column->column_number].toDouble(&ok);

        if (!ok) {
            qWarning() << "Could not understand CSV line: " << line;
            continue;
        }

        // Add point to plot
        column->x.append(t);
        column->y.append(y);
        plot();
    }

    t += 1;
}

void MainWindow::initializeTimer()
{
    QTimer *update_timer = new QTimer(this);
    update_timer->start(1.0); // [ms]
    connect(update_timer, SIGNAL(timeout()), SLOT(on_timeout()));
}
