#ifndef LOGWRITER_H
#define LOGWRITER_H

#include "globals.h"
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class LogWriter : public QObject
{
    Q_OBJECT
public:
    explicit LogWriter(QObject *parent = 0);
    void writeLog(QString logFile, QString entry);

signals:

public slots:



};

#endif // LOGWRITER_H
