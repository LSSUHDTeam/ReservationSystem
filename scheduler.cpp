#include "scheduler.h"
#include <QDateTime>
#include <QDate>
#include "globals.h"
#include <QStringList>
#include <QDebug>

scheduler::scheduler(_dataStructs * _structs, QObject *parent) :
    QObject(parent)
{
    dataStructs = _structs;

}

QStringList scheduler::getListOfAvailableItemsForRange(QDateTime start, QDateTime end, int type)
{
    QStringList result;
    QString beginTime = start.time().toString();
    QString endTime = end.time().toString();

    if( type != -1 )
    {
        // calculate date difference,
        QStringList dateRange = calculateDifferenceInDates(start.date(), end.date(), type);
        // check each piece of equipment for said datetime, if available add id to list for return

        // dateRange only gives date, so must make date time with time from start and end out of each,
        // and test against dates unavailable

        for( int i = 0; i < dateRange.count(); i++)
        {
            // Make a datetime from each element in dateTimeList for starting and ending
            QString tempStart = (dateRange[i].trimmed() + " " + beginTime.trimmed());
            QString tempEnd = (dateRange[i].trimmed() + " " + endTime.trimmed());

            QDateTime testStart = QDateTime::fromString(tempStart, STANDARD_DATE_FORMAT);
            QDateTime testEnd = QDateTime::fromString(tempEnd, STANDARD_DATE_FORMAT);

            for( int i = 0; i < dataStructs->counters.invCount; i++ )
            {
                if(  isDeviceAvailable(dataStructs->inventoryItems[i].id.toInt(), testStart, testEnd) )
                {
                    // ALl available ids will be displayed here.
                    result.append(dataStructs->inventoryItems[i].id);
                }
            }
        }
    }
    else
    {
        // see what is available today
        for( int i = 0; i < dataStructs->counters.invCount; i++ )
        {
            if(  isDeviceAvailable(dataStructs->inventoryItems[i].id.toInt(), start, end) )
            {
                // ALl available ids will be displayed here.
                result.append(dataStructs->inventoryItems[i].id);
            }
        }
    }
    result.removeDuplicates();
    return result;
}

// Has to be send every individual day, will not go by range.
bool scheduler::isDeviceAvailable(int deviceID, QDateTime timeStart, QDateTime timeEnd)
{
    for( int i = 0; i < dataStructs->counters.schedCount; i++ )
    {
        if( deviceID == dataStructs->itemSchedules[i].inventoryItemID.toInt() )
        {
            // Go through all times unavailable
            for( int j = 0; j < dataStructs->itemSchedules[i].datesUnavailable.count(); j++ )
            {
                // Grab start and end time in datesUnavailable[j], and trim whitespaces before conversion to datetime
                QStringList temp = dataStructs->itemSchedules[i].datesUnavailable[j].split(RESULT_MEMBER_DELIMETER);
                QString s, e;
                s = temp[0].trimmed();
                e = temp[1].trimmed();

                QDateTime dateTimeUnavailableStart =
                        QDateTime::fromString(s, STANDARD_DATE_FORMAT);

                QDateTime dateTimeUnavailableEnd =
                        QDateTime::fromString(e, STANDARD_DATE_FORMAT);
/*
                  If startTime or endTime is between unavailable times, return false
                  This covers if the requested time starts when item is reserverd and
                  if the requested time will overlap into a reservation
*/

                if( (timeStart > dateTimeUnavailableStart &&
                     timeStart < dateTimeUnavailableEnd ) ||
                    (timeEnd > dateTimeUnavailableStart &&
                     timeStart < dateTimeUnavailableEnd) )
                {
                    //qDebug() << " Unavailable";
                    return 0;
                }
            }
            // If it reaches here, it means that the device is available
            return 1;
        }
    }
    return 0;
}

QStringList scheduler::calculateDifferenceInDates(QDate start, QDate end, int type)
{
//qDebug() << " \n\n CALCULATING DIFFERENCE IN DATES ";

    QStringList listOfDates;

    int difference  = start.daysTo(end);

    listOfDates.append(start.toString(STANDARD_DATE_FORMAT_2));

    switch(type)
    {
    case 0:
    {
        QDate temp;
        int difference  = start.daysTo(end);

        // Daily Delivery. Make dates for all days except Sat/Sun.

        for( int i = 1; i <= difference; i++ )
        {
            // Add days to start
            temp = start.addDays(i);

            // Ensure non-weekend
            if ( temp.dayOfWeek() != 6 && temp.dayOfWeek() != 7 )
            {
                // Add to list
                listOfDates.append(temp.toString(STANDARD_DATE_FORMAT_2));
            }
        }
        break;
    }
    case 1:
    {
//qDebug() << "\n TYPE = " << type << "\n";

        // Weekly Delivery. Make dates for every 7 days.

        int numberOfDeliveries = difference / 7;

//qDebug() << "\n NUM OF DELIVERIES : " << numberOfDeliveries;

        for( int i = 0, j = 7; i < numberOfDeliveries; i++ )
        {
            QDate temp = start.addDays(j);
            listOfDates.append(temp.toString(STANDARD_DATE_FORMAT_2));
            j+=7;
        }
        break;
    }
    case 2:
    {
//qDebug() << "\n TYPE = " << type << "\n";

        // Monthly. Make dates for every month in series.

        QDate temp;
        bool includedStart = false;
        int daysThisMonth = start.daysInMonth();

        while ( difference != 0 )
        {
            if ( !includedStart )
            {
                temp = start.addDays(daysThisMonth);
                listOfDates.append(temp.toString(STANDARD_DATE_FORMAT_2));
                includedStart = true;
            }
            else
            {
                daysThisMonth = temp.daysInMonth();
                temp = temp.addDays(daysThisMonth);
                listOfDates.append(temp.toString(STANDARD_DATE_FORMAT_2));
            }
            difference -= daysThisMonth;

            if ( difference < 0 )
            {
                // Should never be the case, but if so it prevens infinite loop.
                difference = 0;
            }
        }
        break;
    }
    case 3:
    {
//qDebug() << "\n TYPE = " << type << "\n";

        // Tuesday / Thursday Delivery. Make dates for all in series.

        for( int i = 1; i <= difference; i++ )
        {
            // Add days to start
            QDate temp = start.addDays(i);

            // Ensure is on tuesday or thursday
            if ( temp.dayOfWeek() == 2 || temp.dayOfWeek() == 4 )
            {
                // Add to list
                listOfDates.append(temp.toString(STANDARD_DATE_FORMAT_2));
            }
        }
        break;
    }
    case 4:
    {
//qDebug() << "\n TYPE = " << type << "\n";

        // Monday / Wednesday / Friday Delivery. Make dates for all in series.

        for( int i = 1; i <= difference; i++ )
        {
            // Add days to start
            QDate temp = start.addDays(i);

            // Ensure is on MWF day.
            if ( temp.dayOfWeek() == 1 || temp.dayOfWeek() == 3 || temp.dayOfWeek() == 5)
            {
                // Add to list
                listOfDates.append(temp.toString(STANDARD_DATE_FORMAT_2));
            }
        }
        break;
    }
    case 5:
    {
//qDebug() << "\n TYPE = " << type << "\n";

        // Daily Delivery. Make dates for all days except Sat/Sun.

        for( int i = 1; i <= difference; i++ )
        {
            // Add days to start
            QDate temp = start.addDays(i);

            // Ensure non-weekend
            if ( temp.dayOfWeek() != 6 && temp.dayOfWeek() != 7 )
            {
                // Add to list
                listOfDates.append(temp.toString(STANDARD_DATE_FORMAT_2));
            }
        }
        break;
    }
    default:
        // DONT DO ANYTHING
        break;
    }
    return listOfDates;
}
