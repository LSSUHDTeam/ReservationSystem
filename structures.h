#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QWidget>
#include <QDebug>
#include "globals.h"

struct reservation
{
    QString resId,
    resStart,
    resEnd, resFor, resBy,
    location, deliveredBy, pickedupBy,
    timeDelivered, timePickedup,
    notes, inventoryList,repeatedIds;
};

struct schedule
{
    QString inventoryItemID;
    QStringList datesUnavailable;
};

struct links
{
    QString
    repeatedID,
    reservations,
    type,
    firstOccurance,
    lastOccurance;
};

struct inventory
{
    QString
    id,
    title,
    desc,
    type,
    barcode,
    lastCheckOut,
    lastCheckIn,
    nextCheckout;
};

struct rooms
{
    QString id, name, desc;
};

struct updateFlags
{
    bool reservations,
    pickups,
    schedules, links,
    inventory, rooms;
    int resIndex, schedIndex,
    linkIndex, invIndex,
    rmIndex, pickIndex;
};

struct counter
{
    int resCount, pickCount,
    schedCount, linkCount,
    invCount, roomCount;
};

struct userStruct
{
    QString userName,
    md5Pass;
};

struct dataStructures
{
    struct reservation reservations[MAX_FUTURE_DELIVERIES];
    struct reservation pickups[MAX_FUTURE_DELIVERIES];
    struct links linkedInformation[MAX_LINKS];
    struct schedule itemSchedules[MAX_INVENTORY_ITEMS];
    struct inventory inventoryItems[MAX_INVENTORY_ITEMS];
    struct rooms schoolRooms[500];
    struct updateFlags updates;
    struct counter counters;
    struct userStruct currentUser;
};

typedef struct dataStructures _dataStructs;

struct errorStruct
{
    QString errCode, errText, errCancelText, errOkText;
    bool userOption; // 0 - Cancel | 1 - Ok
};

struct conflictStruct
{
    QString conflictType,
    conflictTableId,
    startDateTime,
    endDateTime,
    repeatType,
    itemToReplace;

    QStringList availableDevices;
};

struct changesMade
{
    bool repeatTypeChange;
    bool locationChange;
    bool deviceChange;
    bool noteChange;
    bool timeChange;
    bool dateChange;

    changesMade()
    {
        repeatTypeChange = 0;
        locationChange = 0;
        deviceChange = 0;
        noteChange = 0;
        timeChange = 0;
        dateChange = 0;
    }
};

QString subroutine_retrieveItemNameById(QString ID, _dataStructs _structs);
QString subroutine_retrieveBarcodeById(QString ID,_dataStructs _structs);
QString subroutine_retrieveDescById(QString ID,_dataStructs _structs);
QString subroutine_retrieveTypeById(QString ID,_dataStructs _structs);
int subroutine_retrieveLinkIndexByLinkId(QString ID,_dataStructs _structs);

QString subroutine_filterText(QString);


#endif // STRUCTURES_H
