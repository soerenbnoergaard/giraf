#ifndef DATAVECTOR_H
#define DATAVECTOR_H

#include <QVector>

// This is a Data Transfer Object.
class DataVector
{
public:
    DataVector(int this_column_number);

    QVector<double> x;
    QVector<double> y;

    QString name;
    int column_number;
};

#endif // DATAVECTOR_H
