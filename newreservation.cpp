#include "newreservation.h"
#include "ui_newreservation.h"


newReservation::newReservation(_dataStructs * _structs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newReservation)
{
    ui->setupUi(this);

    dataStructs = _structs;

    scheduleManager = new scheduler(dataStructs);
    nonItemReservation = false;

    theNewReservation.notes = EMPTY_DB_ENTRY;

    setupWindow();

}


newReservation::~newReservation()
{
    delete ui;
}

void newReservation::setupWindow()
{
    ui->startTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->endTimeEdit->setDateTime(QDateTime::currentDateTime());

    ui->repLabel->hide();
    ui->repetitionSelect->hide();
    repeatCombo = (QStringList() << "Daily" << "Weekly" << "Monthly" << "Tuesday, Thursday"\
                   << "Monday, Wednesday, Friday");
    repeatAbbrs = (QStringList() << "D" << "W" << "M" << "TR" << "MWF");
    ui->repetitionSelect->addItems(repeatCombo);

    ui->roomSelect->addItem("Select Room");
    for( int i = 0; i < dataStructs->counters.roomCount; i++ )
    {
        ui->roomSelect->addItem(dataStructs->schoolRooms[i].name);
    }

    ui->roomSelect->setCurrentIndex(0);

    setupController = true;
    dateTimeChanged();
}

void newReservation::setReservation()
{
    theNewReservation.resId = "NEW";
   // theNewReservation.inventoryList = ui->inventoryList->it
}

void newReservation::on_repeatedCheckBox_clicked()
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
        ui->repLabel->hide();
        ui->repetitionSelect->hide();
    }

    // Need to generate the list of dates
    dateTimeChanged();
}

void newReservation::on_repetitionSelect_currentIndexChanged(int index)
{
    // Re-run inventory population for days selected by
    Q_UNUSED(index);
    dateTimeChanged();
}

void newReservation::on_startTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    Q_UNUSED(dateTime);
    dateTimeChanged();
}

void newReservation::on_endTimeEdit_dateTimeChanged(const QDateTime &dateTime)
{
    Q_UNUSED(dateTime);
    dateTimeChanged();
}


/*

        Date Time Changes

*/

void newReservation::dateTimeChanged()
{
    ui->inventoryList->clear();
    ui->reservationList->clear();
    itemsThatCanBeReserved.clear();
    itemsReserved.clear();

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
                if( !deletions.contains(itemsThatCanBeReserved.indexOf(ell)) )
                {
                    tempHoldList.append(ell);
                }
            }
            itemsThatCanBeReserved = tempHoldList;
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
}


/*

    Add items to reservaion

*/

void newReservation::on_addItemButton_clicked()
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


/*

    Remove items from reservaion

*/

void newReservation::on_removeItemButton_clicked()
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


/*

    Generate New Location

*/

void newReservation::on_newLocationButton_clicked()
{
    AddLocation * NewLocation = new AddLocation(dataStructs, this);
    connect(NewLocation, SIGNAL(doneEditing()), this,SLOT(addLocationClosed()));
    NewLocation->show();
}

void newReservation::addLocationClosed()
{
    emit forceUpdate();
    ui->roomSelect->clear();
    for( int i = 0; i < dataStructs->counters.roomCount; i++ )
    {
        ui->roomSelect->addItem(dataStructs->schoolRooms[i].name);
    }
}


/*

    Add note to reservation

*/

void newReservation::on_addNoteButton_clicked()
{
    NotesWindow * noteWindow = new NotesWindow(&theNewReservation, this);
    noteWindow->show();
}


/*

    Reset form back to initial setting

*/

void newReservation::on_resetButton_clicked()
{
    ui->startTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->endTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->resForText->clear();
    ui->reservedBy->clear();
    ui->reservationList->clear();
    ui->repeatedCheckBox->setChecked(false);
    ui->repetitionSelect->hide();
    theNewReservation.notes = EMPTY_DB_ENTRY;
    nonItemReservation = false;
    ui->inventoryList->setEnabled(true);
    ui->reservationList->setEnabled(true);
    ui->addItemButton->setEnabled(true);
    ui->removeItemButton->setEnabled(true);
    itemsReserved.clear();
    dateTimeChanged();
}


/*

    Save new reservation

*/

void newReservation::on_saveButton_clicked()
{
    // Make sure that items are reserved
    if ( itemsReserved.length() > 0 || nonItemReservation)
    {
        // Make sure that the time scale is correct
        if ( ui->startTimeEdit->time() <= ui->endTimeEdit->time() || ui->startTimeEdit->date() <= ui->endTimeEdit->date() )
        {
            // Make sure that reservation has a who for / by
            if( checkUserEntry() )
            {
                if( ui->startTimeEdit->date() <= ui->endTimeEdit->date() )
                {
                    if( ui->startTimeEdit->date() == ui->endTimeEdit->date() && ui->startTimeEdit->time() > ui->endTimeEdit->time() )
                    {
                        generateError(ERR_INCORRECT_TIME_DESIGNATION, "Start/ End date are the same, but the start time is after the end time. \n Remember that we are using 24 hour time.", 0);
                    }
                    else if( ui->roomSelect->currentText() != "Select Room")
                    {
                        socketClient socketConn;

                        ui->saveButton->setEnabled(false);

                        QString itemsList = "";

                        for(int i = 0; i < itemsReserved.length(); i++ )
                        {
                            if( itemsReserved[i] != "-1")
                            {
                                itemsList += itemsReserved[i];
                                if( i != itemsReserved.length()-1)
                                {
                                    itemsList += DEVICE_LIST_DELIMETER;
                                }
                            }
                        }
                        if( itemsList == "" )
                        {
                            itemsList = "NA";
                        }

                        QString update = "";
                        update += ui->startTimeEdit->dateTime().toString(STANDARD_DATE_FORMAT); update += RESULT_MEMBER_DELIMETER;
                        update += ui->endTimeEdit->dateTime().toString(STANDARD_DATE_FORMAT); update += RESULT_MEMBER_DELIMETER;
                        update += subroutine_filterText(ui->resForText->text()); update += RESULT_MEMBER_DELIMETER;
                        update += subroutine_filterText(ui->reservedBy->text()); update += RESULT_MEMBER_DELIMETER;
                        update += dataStructs->schoolRooms[ui->roomSelect->currentIndex()-1].name; update += RESULT_MEMBER_DELIMETER;
                        update += EMPTY_DB_ENTRY; update += RESULT_MEMBER_DELIMETER;
                        update += EMPTY_DB_ENTRY; update += RESULT_MEMBER_DELIMETER;
                        update += BLANK_TIME_ENTRY; update += RESULT_MEMBER_DELIMETER;
                        update += BLANK_TIME_ENTRY; update += RESULT_MEMBER_DELIMETER;
                        update += theNewReservation.notes; update += RESULT_MEMBER_DELIMETER;
                        update += itemsList; update += RESULT_MEMBER_DELIMETER;

                        if ( ui->repeatedCheckBox->isChecked() )
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
                            update += ITEM_NEEDS_REFACTORING;
                            update += QUERY_DELIMETER;
                            update += temp;

                            // Find "Type" and add to update for repeated table
                            update += QUERY_DELIMETER;
                            update += repeatAbbrs.at(ui->repetitionSelect->currentIndex());
                        }
                        else
                        {
                            update += EMPTY_DB_ENTRY;
                        }

                        // Send to server for update

                        socketConn.SubmitQuery(DELIVERY_QUERY, 11, 0, update);
                        emit forceUpdate();
                        this->close();
                    }
                    else
                    {
                        generateError(ERR_INCOMPLETE_FORM, "You did not set the room. You may have added a room, but now you have to select it. ",0);
                    }

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
        generateError(ERR_INCOMPLETE_FORM, "No items added to reservation. If it is a non-Item reservation, please press the button indicating it as such.", 0);
    }
}


/*

    Close window, cancel changes

*/

void newReservation::on_cancelButton_clicked()
{
    this->close();
}

bool newReservation::checkUserEntry()
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

void newReservation::generateError(QString eCode, QString eText, int wErr)
{
    struct errorStruct err;
    whichErr = wErr;
    err.errCode = eCode;
    err.errText = eText;

    errWindow* newError = new errWindow(&err, this);
    connect(newError, SIGNAL(doneDisplaying()), this, SLOT(errWindowDoneDispaying()));
    newError->show();
}

void newReservation::errWindowDoneDispaying()
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

void newReservation::on_resForText_returnPressed()
{
    ui->reservedBy->setFocus();
}

void newReservation::on_nonItemButton_clicked()
{
    nonItemReservation = true;
    ui->inventoryList->setEnabled(false);
    ui->reservationList->setEnabled(false);
    ui->reservationList->clear();
    itemsReserved.clear();
    ui->addItemButton->setEnabled(false);
    ui->removeItemButton->setEnabled(false);
    itemsReserved.append("-1");

    generateError("REMINDER", "Since this is a non-item reservation, you need to add a note to the reservation.", 0);
}
