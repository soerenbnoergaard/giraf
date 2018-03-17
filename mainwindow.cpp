#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadCsvFile(QString filename, bool *ok)
{
    // Open a file for reading (non-blocking)
    *ok = QFile::exists(filename);
    if (ok) {
        inputfile.setFileName(filename);
        inputfile.open(QIODevice::ReadOnly);
        inputfileTextStream = new QTextStream(&inputfile);
    }
}

void MainWindow::setNumberOfRowsToSkip(int num_rows)
{
    skiprows = num_rows;
}

void MainWindow::setHeaderRow(int row_number)
{
    headerrow = row_number;
}

void MainWindow::setDelimiter(QString this_delimiter)
{
    delimiter = this_delimiter;
}

void MainWindow::addColumn(int c)
{
    DataVector *d = new DataVector(c);
    columns.append(d);

    int i = columns.indexOf(d);

    ui->plot->addGraph();
    ui->plot->graph(i)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->plot->graph(i)->setLineStyle(QCPGraph::lsLine);

    QColor color;
    switch (i % 10) {
    case 0:
        color = QColor(31,119,180);
        break;
    case 1:
        color = QColor(255,127,14);
        break;
    case 2:
        color = QColor(44,160,44);
        break;
    case 3:
        color = QColor(214,39,130);
        break;
    case 4:
        color = QColor(148,102,189);
        break;
    case 5:
        color = QColor(140,86,75);
        break;
    case 6:
        color = QColor(227,119,194);
        break;
    case 7:
        color = QColor(113,127,127);
        break;
    case 8:
        color = QColor(188,189,34);
        break;
    case 9:
        color = QColor(23,190,207);
        break;
    }
    ui->plot->graph(i)->setPen(QPen(color));
}

void MainWindow::updateGraph()
{
    for (int i = 0; i < columns.length(); i++) {
        ui->plot->graph(i)->setData(columns[i]->x, columns[i]->y);
    }

    ui->plot->update();
    ui->plot->rescaleAxes();
    ui->plot->replot();
}

void MainWindow::start()
{
    initializeTimer();
}

void MainWindow::on_timeout()
{
    static int row_counter = 0;
    static uint64_t t = 0;
    static double y = 0;

    // Read a line from the input file
    QString line;
    line = inputfileTextStream->readLine();
    if (line.length() == 0)
        return;

    // Split the line into a row of cells
    QStringList row;
    row = line.split(delimiter);
    row_counter += 1;

    // Check if the current row contains column names
    if (row_counter-1 == headerrow) {
        for (int i = 0; i < columns.length(); i++) {
            DataVector *column = columns[i];
            if (column->column_index > row.length()-1)
                continue;

            QString label = row[column->column_index];
            ui->plot->graph(i)->setName(label);
        }
        ui->plot->legend->setVisible(true);
        return;
    }

    // Skip a specified number of rows
    if (row_counter-1 < skiprows)
        return;

    // Add columns to the plot
    for (int i = 0; i < columns.length(); i++) {
        DataVector *column = columns[i];

        // If the specified column is not present in the row, proceed to next row
        if (column->column_index > row.length()-1) {
            qWarning() << "Column" << column->column_index << "not found in row:" << line;
            continue;
        }

        // Convert string to number
        bool ok;
        y = row[column->column_index].toDouble(&ok);

        if (!ok) {
            qWarning() << "Could not understand CSV line: " << line;
            continue;
        }

        // Add point to plot
        column->x.append(t);
        column->y.append(y);
        updateGraph();
    }

    t += 1;
}

void MainWindow::initializeTimer()
{
    QTimer *update_timer = new QTimer(this);
    update_timer->start(1.0); // [ms]
    connect(update_timer, SIGNAL(timeout()), SLOT(on_timeout()));
}
