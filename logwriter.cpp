#include "logwriter.h"

LogWriter::LogWriter(QObject *parent) :
    QObject(parent)
{
}


void LogWriter::writeLog(QString logFile, QString entry)
{
    QString dateTime = QDateTime::currentDateTime().toString(STANDARD_DATE_FORMAT);

    QFile theFile(logFile);
    if( QFile::exists(logFile))
    {
        if( theFile.open(QFile::Append) )
        {
            QTextStream stream(&theFile);
            stream << "\n"
                   << dateTime
                   << ","
                   << entry;
        }
    }
    else
    {
        if( theFile.open(QFile::WriteOnly) )
        {
            QTextStream stream(&theFile);
            stream << "\n"
                   << dateTime
                   << ","
                   << entry;
        }
    }
}
