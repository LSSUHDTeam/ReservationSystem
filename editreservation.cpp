#include "editreservation.h"
#include "ui_editreservation.h"

editReservation::editReservation(_dataStructs *_struct, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editReservation)
{
    dataStructs = _struct;
    currIndex = dataStructs->updates.resIndex;
    (dataStructs->reservations[currIndex].repeatedIds == "NA") ? isRepeated = false : isRepeated = true;

    scheduleManager = new scheduler(dataStructs);

    connect(&socketConn, SIGNAL(dataReady(QString)), this, SLOT(processData(QString)));

    originalNote = dataStructs->reservations[currIndex].notes;

    ui->setupUi(this);

    setupWindow();
}

editReservation::~editReservation()
{
    delete ui;
}

void editReservation::setupWindow()
{
    // Load information
    repeatCombo = (QStringList() << "Daily" << "Weekly" << "Monthly" << "Tuesday, Thursday"\
                   << "Monday, Wednesday, Friday");
    repeatAbbrs = (QStringList() << "D" << "W" << "M" << "TR" << "MWF");

    int roomIndex = 0; // Where at in roomSelect current reservations data is
    for( int i = 0; i < dataStructs->counters.roomCount; i++ )
    {
        ui->roomSelect->addItem(dataStructs->schoolRooms[i].name);

        if(dataStructs->schoolRooms[i].name == dataStructs->reservations[currIndex].location)
            roomIndex = i;
    }

    // Display reservation information on form
    ui->repetitionSelect->addItems(repeatCombo);
    ui->resForText->setText(dataStructs->reservations[currIndex].resFor);
    ui->reservedBy->setText(dataStructs->reservations[currIndex].resBy);
    ui->roomSelect->setCurrentIndex(roomIndex);

    // Add items on reservation to screen
    foreach(QString el, dataStructs->reservations[currIndex].inventoryList.split(DEVICE_LIST_DELIMETER))
    {
        itemsReserved.append(el);
        itemsInCurrentReservation.append(el);
        ui->reservationList->addItem(new QListWidgetItem(subroutine_retrieveItemNameById(el, *dataStructs)));
    }

    // Repetition Check. If repeated, id it first in series ?
    if( isRepeated )
    {
        int ret = subroutine_retrieveLinkIndexByLinkId(
                    dataStructs->reservations[currIndex].repeatedIds,
                    *dataStructs);

        if( -1 != ret )
        {
            // Set id of linked structure
            dataStructs->updates.linkIndex = ret;

            // Test if is first in series
            (dataStructs->linkedInformation[dataStructs->updates.linkIndex].reservations.split(LINKED_INFORMATION_DELIMETER)[0] == dataStructs->reservations[currIndex].resId) ?
                        isFirstInSeries = true : isFirstInSeries = false;

            // Set repeated information on form
            int repeatIndex = repeatAbbrs.indexOf(dataStructs->linkedInformation[dataStructs->updates.linkIndex].type);
            ui->repetitionSelect->setCurrentIndex(repeatIndex);
            ui->repeatedCheckBox->setChecked(true);

            // Find start and end date time of reservation series
            QStringList series = dataStructs->linkedInformation[dataStructs->updates.linkIndex].reservations.split(LINKED_INFORMATION_DELIMETER);

            // Set start / end date times to the first start time and last end time of link chain
            processSwitch = 0;
            socketConn.SubmitQuery(DELIVERY_QUERY, 12, 0, series[0].trimmed());

            processSwitch = 1;
            socketConn.SubmitQuery(DELIVERY_QUERY, 12, 0, series.last().trimmed());

        }
        else
        {
            qDebug() << "THROW AN ERROR HERE SAYING AN ISSUE OCCURED FINDING INDEX OF CURRENTLY LINKED STRUCTURE";
        }

        ui->repLabel->show();
    }
    else
    {
        ui->repLabel->hide();
        ui->repetitionSelect->hide();
        isFirstInSeries = true;

        ui->startTimeEdit->setDateTime(QDateTime::fromString(dataStructs->reservations[currIndex].resStart, STANDARD_DATE_FORMAT));
        ui->endTimeEdit->setDateTime(QDateTime::fromString(dataStructs->reservations[currIndex].resEnd, STANDARD_DATE_FORMAT));
    }

}

void editReservation::processData(QString data)
{
    if( data != EMPTY_DB_ENTRY )
    {
        QStringList dataList = data.split(INCOMING_RESULT_DELIMETER);

        switch( socketConn.lastQuery.first )
        {
        case DELIVERY_QUERY:

            switch( socketConn.lastQuery.second )
            {
            case 12:
            {
                QStringList cells = data.split(RESULT_MEMBER_DELIMETER);

                if( !processSwitch )
                {
                    ui->startTimeEdit->setDateTime(QDateTime::fromString(cells[1], STANDARD_DATE_FORMAT));
                }
                else
                {
                    ui->endTimeEdit->setDateTime(QDateTime::fromString(cells[2], STANDARD_DATE_FORMAT));
                }
                break;
            }

            default:
                break;
            }

            break;

        // END OUTTER SWITCH
        default:
            break;
        }
    }
}

void editReservation::on_repeatedCheckBox_clicked()
{
    if ( ui->repeatedCheckBox->isChecked() )
    {
        if( ui->startTimeEdit->date() != ui->endTimeEdit->date() )
        {
            ui->repLabel->show();
            ui->repetitionSelect->show();
        }
        else
        {
            ui->repeatedCheckBox->setChecked(false);
        }
    }
    else
    {
        if ( isFirstInSeries )
        {
            ui->repLabel->hide();
            ui->repetitionSelect->hide();
            itemsReserved.clear();
            itemsThatCanBeReserved.clear();
            ui->inventoryList->clear();
            ui->reservationList->clear();
        }
        else
        {
            QString Message = (" Removing repition can only be done via the first reservation in the series.\n If you want to remove the reservation, please use the 'Remove' button.");
            generateError("Not First In Series", Message, 0);
            ui->repeatedCheckBox->setChecked(true);
        }
    }

    // Need to generate the list of dates
    dateTimeChanged();
}

void editReservation::on_repetitionSelect_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    dateTimeChanged();
}

void editReservation::on_startTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    Q_UNUSED(dateTime);
    dateTimeChanged();
}

void editReservation::on_endTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    Q_UNUSED(dateTime);
    dateTimeChanged();
}

void editReservation::dateTimeChanged()
{
    ui->inventoryList->clear();
    itemsThatCanBeReserved.clear();

    int repeatedType = -1;

    QDateTime startDateTime = ui->startTimeEdit->dateTime();
    QDateTime endDateTime = ui->endTimeEdit->dateTime();


    if ( ui->repeatedCheckBox->isChecked() )
    {
        repeatedType = ui->repetitionSelect->currentIndex();
    }

    listOfRepeatedDates.clear();
    listOfRepeatedDates = scheduleManager->calculateDifferenceInDates(startDateTime.date(), endDateTime.date(), repeatedType);

    QString startTime = startDateTime.time().toString(STANDARD_TIME_FORMAT);
    QString endTime = endDateTime.time().toString(STANDARD_TIME_FORMAT);

    QStringList tempItemsThatCanBeReserved, tempHoldList;
    for(int i = 0; i < listOfRepeatedDates.count(); i++)
    {
        QList<int> deletions;
        QString tempStart = listOfRepeatedDates[i] + " " + startTime;
        QString tempEnd = listOfRepeatedDates[i] + " " + endTime;
        QDateTime testStart = QDateTime::fromString(tempStart, STANDARD_DATE_FORMAT);
        QDateTime testEnd = QDateTime::fromString(tempEnd, STANDARD_DATE_FORMAT);

        tempItemsThatCanBeReserved = scheduleManager->getListOfAvailableItemsForRange(testStart, testEnd, repeatedType);

        // If first date, add what is available
        if(itemsThatCanBeReserved.isEmpty())
        {
            itemsThatCanBeReserved = tempItemsThatCanBeReserved;
        }
        else
        {
            tempHoldList.clear();

            // Go through each available device from first day
            foreach(QString ell, itemsThatCanBeReserved)
            {
                // If a once available device is no longer available, mark it for deletion
                if( !tempItemsThatCanBeReserved.contains(ell) )
                {
                    deletions.append(itemsThatCanBeReserved.indexOf(ell));
                }
            }

            // Once spots for deletions found, reiterate and determine if removed
            foreach(QString ell, itemsThatCanBeReserved)
            {
                // if deletions doesn't contain the index of current element add to temp list
                if( !deletions.contains(itemsThatCanBeReserved.indexOf(ell)) && !itemsReserved.contains(ell))
                {
                    tempHoldList.append(ell);
                }
            }
            itemsThatCanBeReserved = tempHoldList;
        }
    }

    // If dateTime overlapping between original reservation dt, add items currently in reservation
    // if they are not still in itemsreserved or not already present in itemsthatcanbereserved

    QDateTime resStart = QDateTime::fromString(dataStructs->reservations[currIndex].resStart, STANDARD_DATE_FORMAT);

    if( resStart >= ui->startTimeEdit->dateTime() && resStart <= ui->endTimeEdit->dateTime() )
    {
        foreach(QString el, itemsInCurrentReservation)
        {
            if( !itemsReserved.contains(el) && !itemsThatCanBeReserved.contains(el) )
            {
                itemsThatCanBeReserved.append(el);
            }
        }
    }

    // Go through available equipment and populate the list.
    QStringList availableEquipment;

    foreach(QString ID, itemsThatCanBeReserved)
    {
        availableEquipment.append(subroutine_retrieveItemNameById(ID, *dataStructs));
    }

    if( availableEquipment.count() > 0 )
    {
        ui->inventoryList->clear();
        foreach (QString title, availableEquipment)
        {
            ui->inventoryList->addItem(new QListWidgetItem(title));
        }
    }

    // Check to see if either date or time changed.
    QDateTime resStartDT = QDateTime::fromString(dataStructs->reservations[currIndex].resStart, STANDARD_DATE_FORMAT);
    QDateTime resEndDT = QDateTime::fromString(dataStructs->reservations[currIndex].resEnd, STANDARD_DATE_FORMAT);
    if( (resStartDT.time() != ui->startTimeEdit->time() || resEndDT.time() != ui->endTimeEdit->time()) &&
            (resStartDT.date() == ui->startTimeEdit->date() && resEndDT.date() == ui->endTimeEdit->date() ))
    {
        changeTracker.timeChange = true;
    }
    else
    {
        changeTracker.timeChange = false;
    }

    if( resStartDT.date() != ui->startTimeEdit->date() || resEndDT.date() != ui->endTimeEdit->date())
    {
        changeTracker.dateChange = true;
    }
    else
    {
        changeTracker.dateChange = false;
    }
}

void editReservation::on_addItemButton_clicked()
{
    int itemIndex = ui->inventoryList->currentIndex().row();

    // Making sure that an item is selected
    if( itemIndex != -1 )
    {
        // Add to items reserved
        itemsReserved.append(itemsThatCanBeReserved[itemIndex]);
        // Remove from items that can be reserved
        itemsThatCanBeReserved.removeAt(itemIndex);

        // Rebuild inventory List
        QStringList availableEquipment;

        foreach(QString ID, itemsThatCanBeReserved)
        {
            availableEquipment.append(subroutine_retrieveItemNameById(ID, *dataStructs));
        }

        ui->inventoryList->clear();
        if( availableEquipment.count() > 0 )
        {
            foreach (QString title, availableEquipment)
            {
                ui->inventoryList->addItem(new QListWidgetItem(title));
            }
        }

        // Rebuild items reserved list
        QStringList equipmentReserved;

        foreach(QString ID, itemsReserved)
        {
            equipmentReserved.append(subroutine_retrieveItemNameById(ID, *dataStructs));
        }

        ui->reservationList->clear();
        if( equipmentReserved.count() > 0 )
        {
            foreach (QString title, equipmentReserved)
            {
                ui->reservationList->addItem(new QListWidgetItem(title));
            }
        }
    }
}

void editReservation::on_removeItemButton_clicked()
{
    int itemIndex = ui->reservationList->currentIndex().row();

    if( itemIndex != -1 )
    {
        // Add to items reserved
        itemsThatCanBeReserved.append(itemsReserved[itemIndex]);
        // Remove from items that can be reserved
        itemsReserved.removeAt(itemIndex);

        // Rebuild inventory List
        QStringList availableEquipment;

        foreach(QString ID, itemsThatCanBeReserved)
        {
            availableEquipment.append(subroutine_retrieveItemNameById(ID, *dataStructs));
        }

        ui->inventoryList->clear();
        if( availableEquipment.count() > 0 )
        {
            ui->inventoryList->clear();
            foreach (QString title, availableEquipment)
            {
                ui->inventoryList->addItem(new QListWidgetItem(title));
            }
        }

        // Rebuild items reserved list
        QStringList equipmentReserved;

        foreach(QString ID, itemsReserved)
        {
            equipmentReserved.append(subroutine_retrieveItemNameById(ID, *dataStructs));
        }

        ui->reservationList->clear();
        if( equipmentReserved.count() > 0 )
        {
            ui->reservationList->clear();
            foreach (QString title, equipmentReserved)
            {
                ui->reservationList->addItem(new QListWidgetItem(title));
            }
        }


    }
}

void editReservation::on_newLocationButton_clicked()
{
    AddLocation * NewLocation = new AddLocation(dataStructs, this);
    connect(NewLocation, SIGNAL(doneEditing()), this,SLOT(addLocationClosed()));
    NewLocation->show();
}

void editReservation::addLocationClosed()
{
    emit forceUpdate();
    ui->roomSelect->clear();
    for( int i = 0; i < dataStructs->counters.roomCount; i++ )
    {
        ui->roomSelect->addItem(dataStructs->schoolRooms[i].name);
    }
}

void editReservation::on_addNoteButton_clicked()
{
    NotesWindow * noteWindow = new NotesWindow(&dataStructs->reservations[currIndex], this);
    noteWindow->show();
}

void editReservation::on_resetButton_clicked()
{
    itemsReserved.clear();
    itemsThatCanBeReserved.clear();
    itemsInCurrentReservation.clear();
    ui->repeatedCheckBox->setChecked(false);
    ui->inventoryList->clear();
    ui->reservationList->clear();
    repeatAbbrs.clear();
    repeatCombo.clear();
    dataStructs->reservations[currIndex].notes = originalNote;

    setupWindow();
    dateTimeChanged();
}

void editReservation::on_cancelButton_clicked()
{
    this->close();
}

bool editReservation::checkUserEntry()
{
    int x = 0, y = x;

    if( ui->resForText->text() == "" || ui->reservedBy->text() == "" )
        return false;

    foreach(QString el, ui->resForText->text())
    {
        if( el != " ")
        {
            x++;
        }
    }

    foreach(QString el, ui->reservedBy->text())
    {
        if( el != " ")
        {
            y++;
        }
    }

    if ( x < 3 || y < 3 )
    {
        return false;
    }

    return true;
}

void editReservation::generateError(QString eCode, QString eText, int wErr)
{
    struct errorStruct err;
    whichErr = wErr;
    err.errCode = eCode;
    err.errText = eText;

    errWindow* newError = new errWindow(&err, this);
    connect(newError, SIGNAL(doneDisplaying()), this, SLOT(errWindowDoneDispaying()));
    newError->show();
}

void editReservation::errWindowDoneDispaying()
{
    switch(whichErr)
    {
    case 0:
        // Do nothing
        break;
    default:
        qDebug() << "Unknown error code from errWindow";
        break;
    }
}

void editReservation::on_saveButton_clicked()
{
    // Make sure that items are reserved
    if ( itemsReserved.length() > 0 )
    {
        // Make sure that the time scale is correct
        if ( ui->startTimeEdit->time() <= ui->endTimeEdit->time() || ui->startTimeEdit->date() <= ui->endTimeEdit->date() )
        {
            // Make sure that reservation has a who for / by
            if( checkUserEntry() )
            {
                if( ui->startTimeEdit->date() <= ui->endTimeEdit->date() )
                {
                 //   if(!(!ui->repeatedCheckBox->isChecked() && ui->startTimeEdit->date()!= ui->endTimeEdit->date()))
                 //   {
                        socketClient socketConn;

                       // ui->saveButton->setEnabled(false);

                        QString itemsList = "";

                        for(int i = 0; i < itemsReserved.length(); i++ )
                        {
                            itemsList += itemsReserved[i];
                            if( i != itemsReserved.length()-1)
                            {
                                itemsList += DEVICE_LIST_DELIMETER;
                            }
                        }

                        QString update = "";
                        update += ui->startTimeEdit->dateTime().toString(STANDARD_DATE_FORMAT); update += RESULT_MEMBER_DELIMETER;
                        update += ui->endTimeEdit->dateTime().toString(STANDARD_DATE_FORMAT); update += RESULT_MEMBER_DELIMETER;
                        update += subroutine_filterText(ui->resForText->text()); update += RESULT_MEMBER_DELIMETER;
                        update += subroutine_filterText(ui->reservedBy->text()); update += RESULT_MEMBER_DELIMETER;
                        update += dataStructs->schoolRooms[ui->roomSelect->currentIndex()].name; update += RESULT_MEMBER_DELIMETER;
                        update += EMPTY_DB_ENTRY; update += RESULT_MEMBER_DELIMETER;
                        update += EMPTY_DB_ENTRY; update += RESULT_MEMBER_DELIMETER;
                        update += BLANK_TIME_ENTRY; update += RESULT_MEMBER_DELIMETER;
                        update += BLANK_TIME_ENTRY; update += RESULT_MEMBER_DELIMETER;
                        update += dataStructs->reservations[currIndex].notes; update += RESULT_MEMBER_DELIMETER;
                        update += itemsList; update += RESULT_MEMBER_DELIMETER;


                        // Check to see if item list has changed, and if it is repeated or not
                        if( dataStructs->reservations[currIndex].repeatedIds == EMPTY_DB_ENTRY )
                        {
                            update += "UPDATE_SINGLE";
                            update += PACKAGE_DELIMETER;
                            update += dataStructs->reservations[currIndex].resId;
                        }
                        else
                        {
                            update += "UPDATE_REPEATED";
                            update += PACKAGE_DELIMETER;
                            update += dataStructs->reservations[currIndex].repeatedIds;
                        }

                        if( ui->repeatedCheckBox->isChecked())
                        {
                            QString temp = "";
                            for( int x = 0; x < listOfRepeatedDates.length(); x++ )
                            {
                                temp += listOfRepeatedDates[x];
                                if( x != listOfRepeatedDates.length()-1)
                                {
                                    temp += RESULT_MEMBER_DELIMETER;
                                }
                            }
                            update += QUERY_DELIMETER;
                            update += temp;

                            // Find "Type" and add to update for repeated table
                            update += QUERY_DELIMETER;
                            update += repeatAbbrs.at(ui->repetitionSelect->currentIndex());
                        }

                        // If the reservation is to be removed, tack on an extra item
                        if( !ui->repeatedCheckBox->isChecked() && isRepeated )
                        {
                            update += QUERY_DELIMETER;
                            update += "REM";
                        }

                        socketConn.SubmitQuery(DELIVERY_QUERY, 13, 0, update);
                        emit forceUpdate();
                        this->close();

                /*    }
                    // Dates not same and repeat not checked
                    else
                    {
                        QString Message = " Dates not equal, and repetition information not selected.\n\n";
                        Message += "If you would like to remove this repetition but keep the delivery for today, please make the end date the";
                        Message += " same as the start date";
                        Message += "\n\nIf you would like to remove the delivery, please us the 'Remove' button";
                        generateError(ERR_INCORRECT_TIME_DESIGNATION, Message,0);
                    } */
                }
                // Issue error for incorrect date
                else
                {
                    generateError(ERR_INCORRECT_TIME_DESIGNATION, "Reservation end date must ba after reservation start date.",0);
                }
            }
            // Issue error for incorrect text field entry
            else
            {
                generateError(ERR_INCOMPLETE_FORM, "A textbox is present with < 3 charachters.", 0);
            }

        }
        // Issue error for time setup
        else
        {
            generateError(ERR_INCORRECT_TIME_DESIGNATION, "Reservation end time must be after reservation start time.", 0);
        }
    }
    // Issue error for blank reservation
    else
    {
        generateError(ERR_INCOMPLETE_FORM, "No items added to reservation", 0);
    }
}

void editReservation::on_removeButton_clicked()
{
    QString update = "REMOVE^";
    update+= dataStructs->reservations[currIndex].resId;
    update+= QUERY_DELIMETER; update += dataStructs->reservations[currIndex].repeatedIds;
    socketConn.SubmitQuery(DELIVERY_QUERY, 13, 0, update);
    forceUpdate();
    this->close();
}
