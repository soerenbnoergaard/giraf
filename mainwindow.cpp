#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRegExp>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reset()
{
    bool ok;
    foreach (DataVector *column, columns) {
        column->x.clear();
        column->y.clear();
    }
    inputfile.close();
    row_counter = 0;
    loadCsvFile(inputfile.fileName(), &ok);
    updateGraph();
}

void MainWindow::save()
{
    QString filter = "PNG (*.png);;PDF (*.pdf);;JPG (*.jpg)";
    QString selected_filter;
    QString output_file = QFileDialog::getSaveFileName(this, "Save graph", "", filter, &selected_filter);

    if (selected_filter.contains("png")) {
        if (!output_file.toLower().endsWith(".png"))
            output_file.append(".png");
        ui->plot->savePng(output_file);
    }
    else if (selected_filter.contains("pdf")) {
        if (!output_file.toLower().endsWith(".pdf"))
            output_file.append(".pdf");
        ui->plot->savePdf(output_file);
    }
    else if (selected_filter.contains("jpg")) {
        if (!output_file.toLower().endsWith(".jpg"))
            output_file.append(".jpg");
        ui->plot->saveJpg(output_file);
    }
}

void MainWindow::copy()
{
    QClipboard *clipboard = QApplication::clipboard();
    QPixmap pixmap = ui->plot->toPixmap();
    clipboard->setPixmap(pixmap);
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

void MainWindow::loadPipeInput()
{
    inputfileTextStream = new QTextStream(stdin);
}

void MainWindow::setNumberOfRowsToSkip(int num_rows)
{
    skiprows = num_rows;
}

void MainWindow::setHeaderRow(int row_number)
{
    headerrow = row_number;
}

void MainWindow::setXAxisColumn(int column_number)
{
    x_axis_column = column_number;
}

void MainWindow::setDelimiter(QString this_delimiter)
{
    delimiter = this_delimiter;
}

void MainWindow::setMarker(char this_marker)
{
    marker_symbol = this_marker;
}

void MainWindow::setLineEnable(bool on)
{
    line_enabled = on;
}

void MainWindow::setXLimits(double xmin, double xmax)
{
    xlim[0] = xmin;
    xlim[1] = xmax;
}

void MainWindow::setYLimits(double ymin, double ymax)
{
    ylim[0] = ymin;
    ylim[1] = ymax;
}

void MainWindow::addColumn(int c)
{
    DataVector *d = new DataVector(c);
    columns.append(d);

    int i = columns.indexOf(d);

    ui->plot->addGraph();

    // Line style
    if (line_enabled)
        ui->plot->graph(i)->setLineStyle(QCPGraph::lsLine);
    else
        ui->plot->graph(i)->setLineStyle(QCPGraph::lsNone);

    // Marker style
    QCPScatterStyle marker_style;
    switch (marker_symbol) {
    case '+':
        marker_style = QCPScatterStyle::ssPlus;
        break;
    case 'o':
        marker_style = QCPScatterStyle::ssCircle;
        break;
    case '*':
        marker_style = QCPScatterStyle::ssStar;
        break;
    case '.':
        marker_style = QCPScatterStyle::ssDot;
        break;
    case 'x':
        marker_style = QCPScatterStyle::ssCross;
        break;
    case 's':
        marker_style = QCPScatterStyle::ssSquare;
        break;
    case 'd':
        marker_style = QCPScatterStyle::ssDiamond;
        break;
    case '^':
        marker_style = QCPScatterStyle::ssTriangle;
        break;
    case 'v':
        marker_style = QCPScatterStyle::ssTriangleInverted;
        break;
    default:
        marker_style = QCPScatterStyle::ssNone;
        break;
    }
    ui->plot->graph(i)->setScatterStyle(marker_style);

    // Color
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

    if (xlim[0] == xlim[1])
        ui->plot->xAxis->rescale();
    else
        ui->plot->xAxis->setRange(xlim[0], xlim[1]);

    if (ylim[0] == ylim[1])
        ui->plot->yAxis->rescale();
    else
        ui->plot->yAxis->setRange(ylim[0], ylim[1]);

    ui->plot->replot();
}

void MainWindow::start()
{
    initializeTimer();
    new QShortcut(QKeySequence(Qt::Key_Space), this, SLOT(reset()));
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this, SLOT(save()));
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this, SLOT(copy()));
}

void MainWindow::on_timeout()
{
    static double x = 0;
    static double y = 0;

    // Read a line from the input file
    QString line;
    line = inputfileTextStream->readLine();
    if (line.length() == 0)
        return;

    // Split the line into a row of cells.
    // If a `delimiter` is present inside quotes, replace it with another character.
    // http://www.qtcentre.org/threads/37304-Split-strings-using-QStringList-split()-but-ignore-quotes

    QStringList row;
    QStringList tmp_list = line.split(QRegExp("\""));
    bool inside = false;
    row.clear();
    foreach (QString s, tmp_list) {
        if (inside)
            row.append(s);
        else
            row.append(s.split(QRegExp(delimiter), QString::SkipEmptyParts));
        inside = !inside;
    }

    row_counter += 1;

    // Check if the current row contains column names
    if (row_counter-1 == headerrow) {
        // X-axis label
        if ((0 <= x_axis_column) && (x_axis_column < row.length()))
            ui->plot->xAxis->setLabel(row[x_axis_column]);

        // Legend labels
        for (int i = 0; i < columns.length(); i++) {
            DataVector *column = columns[i];
            if (column->column_index > row.length()-1)
                continue;

            QString label = row[column->column_index];
            ui->plot->graph(i)->setName(label);
        }

        QColor legend_color = Qt::white;
        legend_color.setAlphaF(0.7);
        ui->plot->legend->setVisible(true);
        ui->plot->legend->setBrush(legend_color);
        ui->plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
        return;
    }

    // Skip a specified number of rows
    if (row_counter-1 < skiprows)
        return;

    // Extract x value
    if (x_axis_column >= 0) {
        // Use x_axis_column as x-axis.
        if (x_axis_column > row.length()-1) {
            qWarning() << "X column" << x_axis_column << "not found in row:" << line;
        }
        else {
            bool ok;
            x = row[x_axis_column].toDouble(&ok);
            if (!ok)
                qWarning() << "Could not understand CSV line: " << line;
        }
    }
    else {
        // Use row counter as x-axis.
        if (headerrow < skiprows)
            x = row_counter - skiprows - 1;
        else
            x = row_counter - skiprows - 2;
    }

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
        column->x.append(x);
        column->y.append(y);
        updateGraph();
    }
}

void MainWindow::initializeTimer()
{
    QTimer *update_timer = new QTimer(this);
    update_timer->start(1.0); // [ms]
    connect(update_timer, SIGNAL(timeout()), SLOT(on_timeout()));
}
