#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "structures.h"
#include <QObject>

class scheduler : public QObject
{
    Q_OBJECT
public:
    explicit scheduler(_dataStructs * _structs, QObject *parent = 0);

    // Start, End, and type of repititions
    QStringList getListOfAvailableItemsForRange(QDateTime, QDateTime, int);
    QStringList calculateDifferenceInDates(QDate, QDate, int);
signals:

public slots:

private:
    _dataStructs * dataStructs;
    bool isDeviceAvailable(int deviceID, QDateTime timeStart, QDateTime timeEnd);
};

#endif // SCHEDULER_H
