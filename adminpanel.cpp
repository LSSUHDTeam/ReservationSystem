#include "adminpanel.h"
#include "ui_adminpanel.h"

AdminPanel::AdminPanel(_dataStructs * _structs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminPanel)
{
    dataStructs = _structs;

    reservedCharachters << RESULT_MEMBER_DELIMETER << INCOMING_RESULT_DELIMETER << PACKAGE_DELIMETER
                        << DEVICE_LIST_DELIMETER << QUERY_DELIMETER << SERVER_SEND_DELIMITER << ",";

    editingExistingDevice = false;

    lookupStructs.counters.invCount = 0;
    lookupStructs.counters.resCount = 0;
    lookupStructs.counters.linkCount = 0;
    lookupStructs.counters.schedCount = 0;
    lookupStructs.counters.roomCount = 0;

    connect(&socketConn, SIGNAL(dataReady(QString)), this, SLOT(processData(QString)));

    ui->setupUi(this);

    QPalette* palette = new QPalette();
    palette->setColor(QPalette::Highlight,Qt::darkBlue);
    ui->lookupTable->setPalette(*palette);
    ui->deviceTable->setPalette(*palette);

    ui->lookupDateEdit->setDate(QDate::currentDate());

    ui->lookupTable->setColumnCount(7);
    ui->lookupTable->setHorizontalHeaderLabels( (QStringList() << "Rec#" << "Start" << "End" << "For" << "Location" << "Items Reserved" << "Type") );
    ui->deviceTable->setColumnWidth(0, 160);
    ui->deviceTable->setColumnWidth(1, 100);
    ui->deviceTable->setColumnWidth(2, 120);
    ui->deviceTable->setColumnWidth(3, 153);

    QStringList headerLabels;
    headerLabels << "TITLE" << "DESC" << "TYPE" << "BARCODE";
    ui->deviceTable->setHorizontalHeaderLabels(headerLabels);
    ui->lookupTable->setColumnWidth(0, 50);
    ui->lookupTable->setColumnWidth(1, 110);
    ui->lookupTable->setColumnWidth(2, 110);
    ui->lookupTable->setColumnWidth(5, 301);

    updateWindow();
}

AdminPanel::~AdminPanel()
{
    delete ui;
}

void AdminPanel::updateWindow()
{
    ui->currentPassword->setEchoMode(QLineEdit::Password);
    ui->newPasswordOne->setEchoMode(QLineEdit::Password);
    ui->newPasswordTwo->setEchoMode(QLineEdit::Password);
    ui->usernameLabel->setText(dataStructs->currentUser.userName);
    dataStructs->updates.invIndex = 0;
    ui->deviceTable->clear();
    deviceBarcodes.clear();
    deviceNames.clear();

    ui->deviceTable->setColumnCount(4);
    ui->deviceTable->setRowCount(dataStructs->counters.invCount);

    QStringList headerLabels;
    headerLabels << "TITLE" << "DESC" << "TYPE" << "BARCODE";
    ui->deviceTable->setHorizontalHeaderLabels(headerLabels);

    for( int i = 0 ; i < dataStructs->counters.invCount; i++ )
    {
        deviceNames.append(dataStructs->inventoryItems[i].title);
        deviceBarcodes.append(dataStructs->inventoryItems[i].barcode);

        QTableWidgetItem *title = new QTableWidgetItem();
        title->setText( dataStructs->inventoryItems[i].title );
        title->setFlags(title->flags() ^ Qt::ItemIsEditable);
        ui->deviceTable->setItem(i, 0, title);

        QTableWidgetItem *desc = new QTableWidgetItem();
        desc->setText( dataStructs->inventoryItems[i].desc );
        desc->setFlags(desc->flags() ^ Qt::ItemIsEditable);
        ui->deviceTable->setItem(i, 1, desc);

        QTableWidgetItem *type = new QTableWidgetItem();
        type->setText( dataStructs->inventoryItems[i].type );
        type->setFlags(type->flags() ^ Qt::ItemIsEditable);
        ui->deviceTable->setItem(i, 2, type);

        QTableWidgetItem *barcode = new QTableWidgetItem();
        barcode->setText( dataStructs->inventoryItems[i].barcode );
        barcode->setFlags(barcode->flags() ^ Qt::ItemIsEditable);
        ui->deviceTable->setItem(i, 3, barcode);
    }
    ui->tabWidget->setCurrentIndex(0);
}

/*

        ADD/EDIT DEVICE PAGE PAGE

*/

void AdminPanel::on_clearButton_clicked()
{
    ui->deviceName->clear();
    ui->deviceDescription->clear();
    ui->deviceType->clear();
    ui->deviceBarcode->clear();
    ui->addButton->setText("Add Device");
    editingExistingDevice = false;
    dataStructs->updates.invIndex = 0;
}

void AdminPanel::on_deviceTable_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    ui->deviceTable->selectRow(ui->deviceTable->currentRow());
}

void AdminPanel::on_AdminPanel_rejected()
{
    emit adminPanelClosed();
}

void AdminPanel::on_removeButton_clicked()
{

 //   qDebug() << ui->deviceTable->currentRow() << dataStructs->updates.invIndex;


    if( ui->deviceTable->currentRow() != -1 && dataStructs->updates.invIndex != -1 )
    {
        // Assume conflict.
        idOfConflictingDevice = dataStructs->inventoryItems[ui->deviceTable->currentRow()].id;
        socketConn.SubmitQuery(ADMIN_QUERY, 4, 0, dataStructs->inventoryItems[ui->deviceTable->currentRow()].id);
        forceUpdate();
        updateWindow();
    }
    else
    {
   //     qDebug() << " invINDEX == -1 OR currRow == -1";
    }
}

void AdminPanel::on_loadButton_clicked()
{
    if( ui->deviceTable->currentRow() >= 0 )
    {
        dataStructs->updates.invIndex = ui->deviceTable->currentRow();
        ui->deviceDescription->clear();
        ui->deviceName->setText( dataStructs->inventoryItems[dataStructs->updates.invIndex].title );
        ui->deviceDescription->insertPlainText(dataStructs->inventoryItems[dataStructs->updates.invIndex].desc);
        ui->deviceBarcode->setText( dataStructs->inventoryItems[dataStructs->updates.invIndex].barcode );
        ui->deviceType->setText( dataStructs->inventoryItems[dataStructs->updates.invIndex].type );
        ui->deviceName->setFocus();
        editingExistingDevice = true;
        ui->addButton->setText("Save");
    }
}

QString AdminPanel::filterText(QString text)
{
    // Filter Reserved Words
    QString temp = "";
    foreach(QString ch, text)
    {
        if( !reservedCharachters.contains(ch) )
        {
            temp += ch;
        }
    }
    return temp;
}

void AdminPanel::on_addButton_clicked()
{
    if( editingExistingDevice && formValidForSave() )
    {
        // Call server to edit an existing device
        QString update = dataStructs->inventoryItems[dataStructs->updates.invIndex].id;
        update += RESULT_MEMBER_DELIMETER; update += filterText(ui->deviceName->text());
        update += RESULT_MEMBER_DELIMETER; update += filterText(ui->deviceDescription->toPlainText());
        update += RESULT_MEMBER_DELIMETER; update += filterText(ui->deviceType->text());
        update += RESULT_MEMBER_DELIMETER; update += filterText(ui->deviceBarcode->text());

        socketConn.SubmitQuery(ADMIN_QUERY, 2, 0, update);

        emit forceUpdate();
        updateWindow();
    }
    else
    {
        // Call server to add device

        if ( formValidForSave() )
        {
            QString update = filterText(ui->deviceName->text());
            update += RESULT_MEMBER_DELIMETER; update += filterText(ui->deviceDescription->toPlainText());
            update += RESULT_MEMBER_DELIMETER; update += filterText(ui->deviceType->text());
            update += RESULT_MEMBER_DELIMETER; update += filterText(ui->deviceBarcode->text());

            socketConn.SubmitQuery(ADMIN_QUERY, 3, 0, update);

            emit forceUpdate();
            updateWindow();
        }
    }
    ui->deviceName->clear();
    ui->deviceDescription->clear();
    ui->deviceType->clear();
    ui->deviceBarcode->clear();
    editingExistingDevice = false;
    ui->addButton->setText("Add Device");
    dataStructs->updates.invIndex = 0;
}


bool AdminPanel::formValidForSave()
{
    if ( filterText(ui->deviceName->text()).length() > 3 )
    {
        if ( filterText(ui->deviceDescription->toPlainText()).length() < 200 )
        {
            if ( filterText(ui->deviceType->text()).length() > 0 )
            {
                if ( filterText(ui->deviceBarcode->text()).length() > 0 )
                {
                    if( !editingExistingDevice )
                    {
                        if ( deviceNames.contains(filterText(ui->deviceName->text())))
                        {
                            generateError(ERR_INCOMPLETE_FORM, "A device exists with this name already.", 1);
                            return 0;
                        }

                        if ( deviceBarcodes.contains(filterText(ui->deviceBarcode->text())) )
                        {
                            generateError(ERR_INCOMPLETE_FORM, "A device exists with this barcode already.", 1);
                            return 0;
                        }
                        // New device does not have any conflicts with existing devices, return 1 for update
                        return 1;
                    }
                    else
                    {
                        // Data for edited device good, let update happen
                        return 1;
                    }
                }
                else
                {
                    generateError(ERR_INCOMPLETE_FORM, "A barcode must be entered.", 1);
                    return 0;
                }
            }
            else
            {
                generateError(ERR_INCOMPLETE_FORM, "A device type must be entered.", 1);
                return 0;
            }
        }
        else
        {
            generateError(ERR_INCOMPLETE_FORM, "Description must be under 200 charachters.", 1);
            return 0;
        }
    }
    else
    {
        generateError(ERR_INCOMPLETE_FORM, "Device name must be greater than 3 charachters.", 1);
        return 0;
    }
}


void AdminPanel::processData(QString data)
{
    if ( data != EMPTY_DB_ENTRY )
    {
        QStringList dataList = data.split(INCOMING_RESULT_DELIMETER);

        switch( socketConn.lastQuery.first )
        {
        case ADMIN_QUERY:

            switch( socketConn.lastQuery.second )
            {
            // List of problems returned from removing device
            case 4:
            {
                if ( data != "1" )
                {
                    conflicts.clear();
                    conflicts = dataList;
                    generateError(ERR_CONFLICT_ON_REMOVAL, (" " + QString::number(conflicts.count()) + " conflict(s) caused attempting to remove device."), 0);
                }
                break;
            }
            case 7:
                populateLookupStructs(dataList);
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }
    }
}

void AdminPanel::generateError(QString eCode, QString eText, int wErr)
{
    whichErr = wErr;
    adminErr.errCode = eCode;
    adminErr.errText = eText;

    errWindow* newError = new errWindow(&adminErr, this);
    connect(newError, SIGNAL(doneDisplaying()), this, SLOT(errWindowDoneDispaying()));
    newError->show();
}

void AdminPanel::errWindowDoneDispaying()
{
    switch(whichErr)
    {
    case 0:
    {
        if( adminErr.userOption )
        {
            AdminConflictHandler * handler = new AdminConflictHandler(dataStructs, conflicts, this);
            connect(handler, SIGNAL(complete()), this, SLOT(conflictHandled()));
            handler->show();
        }
        break;
    }
    case 1:
        dataStructs->updates.invIndex = 0;
        break;
    default:
        break;
    }
}

void AdminPanel::conflictHandled()
{
    qDebug() << " Conflict handled ";

    // All conflicts should have been taken care of. Resend request for deletion.
    socketConn.SubmitQuery(ADMIN_QUERY, 4, 0, idOfConflictingDevice);
    forceUpdate();
    updateWindow();
}

/*

        EDIT USER PAGE

*/

void AdminPanel::on_changePasswordButton_clicked()
{
    if( ui->newPasswordOne->text().length() > 5 || ui->newPasswordTwo->text().length() > 5 )
    {
        QString currPass = QString(QCryptographicHash::hash((ui->currentPassword->text().toUtf8()),QCryptographicHash::Md5).toHex());
        QString passOne = QString(QCryptographicHash::hash((ui->newPasswordOne->text().toUtf8()),QCryptographicHash::Md5).toHex());
        QString passTwo = QString(QCryptographicHash::hash((ui->newPasswordTwo->text().toUtf8()),QCryptographicHash::Md5).toHex());

        if(currPass == dataStructs->currentUser.md5Pass)
        {
            if (passOne == passTwo)
            {
                if( passOne != currPass )
                {

                    QString update = dataStructs->currentUser.userName + QUERY_DELIMETER + passTwo;
                    socketConn.SubmitQuery(ADMIN_QUERY, 6, 0, update);
                    ui->currentPassword->clear();
                    ui->newPasswordOne->clear();
                    ui->newPasswordTwo->clear();
                    ui->errLabel->setText("Password changed");
                }
                else
                {
                    ui->errLabel->setText("Thats not changing anything.");
                }
            }
            else
            {
                ui->errLabel->setText("Passwords do not match.");
            }
        }
        else
        {
            ui->errLabel->setText("Current password is incorrect.");
        }
    }
    else
    {
         ui->errLabel->setText("New password must be at least 6 charachters.");
    }
}

/*

        RESERVATION LOOKUP PAGE

*/

void AdminPanel::purgeLookupStructs()
{
    int i;
    for( i = 0 ; i < lookupStructs.counters.resCount; i++)
    {
        lookupStructs.reservations[i].resId = "";
        lookupStructs.reservations[i].resStart = "";
        lookupStructs.reservations[i].resEnd = "";
        lookupStructs.reservations[i].resFor = "";
        lookupStructs.reservations[i].resBy = "";
        lookupStructs.reservations[i].location = "";
        lookupStructs.reservations[i].deliveredBy = "";
        lookupStructs.reservations[i].pickedupBy = "";
        lookupStructs.reservations[i].timeDelivered = "";
        lookupStructs.reservations[i].timePickedup = "";
        lookupStructs.reservations[i].notes = "";
        lookupStructs.reservations[i].inventoryList = "";
        lookupStructs.reservations[i].repeatedIds = "";
    }

    for( i= 0; i < lookupStructs.counters.linkCount; i++)
    {
        lookupStructs.linkedInformation[i].repeatedID = "";
        lookupStructs.linkedInformation[i].reservations = "";
        lookupStructs.linkedInformation[i].firstOccurance = "";
        lookupStructs.linkedInformation[i].lastOccurance = "";
        lookupStructs.linkedInformation[i].type = "";
    }

    for( i=0; i< lookupStructs.counters.invCount; i++ )
    {
        lookupStructs.inventoryItems[i].id = "";
        lookupStructs.inventoryItems[i].desc = "";
        lookupStructs.inventoryItems[i].barcode = "";
        lookupStructs.inventoryItems[i].lastCheckIn = "";
        lookupStructs.inventoryItems[i].lastCheckOut = "";
        lookupStructs.inventoryItems[i].title = "";
        lookupStructs.inventoryItems[i].nextCheckout = "";
        lookupStructs.inventoryItems[i].type = "";
    }

    for( i=0; i< lookupStructs.counters.schedCount; i++ )
    {
        lookupStructs.itemSchedules[i].inventoryItemID = "";
        lookupStructs.itemSchedules[i].datesUnavailable.clear();
    }

    for( i=0; i< lookupStructs.counters.roomCount; i++ )
    {
        lookupStructs.schoolRooms[i].name = "";
        lookupStructs.schoolRooms[i].desc = "";
        lookupStructs.schoolRooms[i].id = "";
    }

    lookupStructs.counters.invCount = 0;
    lookupStructs.counters.resCount = 0;
    lookupStructs.counters.linkCount = 0;
    lookupStructs.counters.schedCount = 0;
    lookupStructs.counters.roomCount = 0;
}

void AdminPanel::populateLookupStructs(QStringList reservations)
{
    int i;
    for( i = 0 ; i < reservations.count(); i++)
    {
        QStringList current = reservations[i].split(RESULT_MEMBER_DELIMETER);
        lookupStructs.reservations[i].resId = current[0];
        lookupStructs.reservations[i].resStart = current[1];
        lookupStructs.reservations[i].resEnd = current[2];
        lookupStructs.reservations[i].resFor = current[3];
        lookupStructs.reservations[i].resBy = current[4];
        lookupStructs.reservations[i].location = current[5];
        lookupStructs.reservations[i].deliveredBy = current[6];
        lookupStructs.reservations[i].pickedupBy = current[7];
        lookupStructs.reservations[i].timeDelivered = current[8];
        lookupStructs.reservations[i].timePickedup = current[9];
        lookupStructs.reservations[i].notes = current[10];
        lookupStructs.reservations[i].inventoryList = current[11];
        lookupStructs.reservations[i].repeatedIds = current[12];

        // If the reservation found is linked, copy the link information from dataStructs for later viewing.
        if( lookupStructs.reservations[i].repeatedIds != EMPTY_DB_ENTRY )
        {
            for(int j = 0; j < dataStructs->counters.linkCount; j++)
            {
                QStringList resList = dataStructs->linkedInformation[j].reservations.split(LINKED_INFORMATION_DELIMETER);

                foreach(QString el, resList)
                {
                    if( el == lookupStructs.reservations[i].resId)
                    {
                        lookupStructs.linkedInformation[lookupStructs.counters.linkCount].repeatedID = dataStructs->linkedInformation[j].repeatedID;
                        lookupStructs.linkedInformation[lookupStructs.counters.linkCount].reservations = dataStructs->linkedInformation[j].reservations;
                        lookupStructs.linkedInformation[lookupStructs.counters.linkCount].firstOccurance = dataStructs->linkedInformation[j].firstOccurance;
                        lookupStructs.linkedInformation[lookupStructs.counters.linkCount].lastOccurance = dataStructs->linkedInformation[j].lastOccurance;
                        lookupStructs.linkedInformation[lookupStructs.counters.linkCount].type = dataStructs->linkedInformation[j].type;
                        lookupStructs.counters.linkCount++;
                    }
                }
            }
        }
    }
    lookupStructs.counters.resCount = i;

    for( i=0; i< dataStructs->counters.invCount; i++ )
    {
        lookupStructs.inventoryItems[i].id = dataStructs->inventoryItems[i].id;
        lookupStructs.inventoryItems[i].desc = dataStructs->inventoryItems[i].desc;
        lookupStructs.inventoryItems[i].barcode = dataStructs->inventoryItems[i].barcode;
        lookupStructs.inventoryItems[i].lastCheckIn = dataStructs->inventoryItems[i].lastCheckIn;
        lookupStructs.inventoryItems[i].lastCheckOut = dataStructs->inventoryItems[i].lastCheckOut;
        lookupStructs.inventoryItems[i].title = dataStructs->inventoryItems[i].title;
        lookupStructs.inventoryItems[i].nextCheckout = dataStructs->inventoryItems[i].nextCheckout;
        lookupStructs.inventoryItems[i].type = dataStructs->inventoryItems[i].type;
    }
    lookupStructs.counters.invCount = dataStructs->counters.invCount;

    for( i=0; i< dataStructs->counters.schedCount; i++ )
    {
        lookupStructs.itemSchedules[i].inventoryItemID = dataStructs->itemSchedules[i].inventoryItemID;
        lookupStructs.itemSchedules[i].datesUnavailable = dataStructs->itemSchedules[i].datesUnavailable;
       // qDebug() << lookupStructs.itemSchedules[i].inventoryItemID << " " << dataStructs->itemSchedules[i].datesUnavailable;
    }
    lookupStructs.counters.schedCount = dataStructs->counters.schedCount;

    for( i=0; i< dataStructs->counters.roomCount; i++ )
    {
        lookupStructs.schoolRooms[i].name = dataStructs->schoolRooms[i].name;
        lookupStructs.schoolRooms[i].desc = dataStructs->schoolRooms[i].desc;
        lookupStructs.schoolRooms[i].id = dataStructs->schoolRooms[i].id;

    }
    lookupStructs.counters.roomCount = dataStructs->counters.roomCount;
}

void AdminPanel::on_searchButton_clicked()
{
    requestForUpdate();
    populateLookupTable();

}

void::AdminPanel::populateLookupTable()
{
    ui->statusLabel->setHidden(true);
    ui->lookupTable->setRowCount(lookupStructs.counters.resCount);
    ui->lookupTable->setHorizontalHeaderLabels( (QStringList() << "Rec#" << "Start" << "End" << "For" << "Location" << "Items Reserved" << "Type") );

    if( lookupStructs.counters.resCount != 0 )
    {
        for(int i = 0; i < lookupStructs.counters.resCount; i++ )
        {
            QTableWidgetItem *rec = new QTableWidgetItem();
            rec->setText( lookupStructs.reservations[i].resId );
            rec->setFlags(rec->flags() ^ Qt::ItemIsEditable);
            ui->lookupTable->setItem(i, 0, rec);

            QTableWidgetItem *start = new QTableWidgetItem();
            start->setText( lookupStructs.reservations[i].resStart );
            start->setFlags(start->flags() ^ Qt::ItemIsEditable);
            ui->lookupTable->setItem(i, 1, start);

            QTableWidgetItem *end = new QTableWidgetItem();
            end->setText( lookupStructs.reservations[i].resEnd );
            end->setFlags(end->flags() ^ Qt::ItemIsEditable);
            ui->lookupTable->setItem(i, 2, end);

            QTableWidgetItem *who = new QTableWidgetItem();
            who->setText( lookupStructs.reservations[i].resFor );
            who->setFlags(who->flags() ^ Qt::ItemIsEditable);
            ui->lookupTable->setItem(i, 3, who);

            QTableWidgetItem *location = new QTableWidgetItem();
            location->setText( lookupStructs.reservations[i].location );
            location->setFlags(location->flags() ^ Qt::ItemIsEditable);
            ui->lookupTable->setItem(i, 4, location);

            QString showList = "";
            QStringList items = lookupStructs.reservations[i].inventoryList.split(DEVICE_LIST_DELIMETER);
            for(int j = 0; j < items.count(); j++)
            {
                // Send dataStructs because lookupStructs did not have inventory populated
                showList += subroutine_retrieveItemNameById(items[j], lookupStructs);
                if( j != items.count()-1 )
                {
                    showList += DEVICE_LIST_DELIMETER;
                }
            }

            QTableWidgetItem *itemList = new QTableWidgetItem();
            itemList->setText( showList );
            itemList->setFlags(itemList->flags() ^ Qt::ItemIsEditable);
            ui->lookupTable->setItem(i, 5, itemList);


            QDate sDate = QDate::fromString(lookupStructs.reservations[i].resStart.split(" ")[0],STANDARD_DATE_FORMAT_2);
            QDate eDate = QDate::fromString(lookupStructs.reservations[i].resEnd.split(" ")[0],STANDARD_DATE_FORMAT_2);


            //qDebug() << lookupStructs.reservations[i].timeDelivered;

            if(lookupStructs.reservations[i].timeDelivered == EMPTY_FIELD_RETURNED)
            {
                QTableWidgetItem *type = new QTableWidgetItem();
                type->setText( "Delivery" );
                type->setFlags(itemList->flags() ^ Qt::ItemIsEditable);
                ui->lookupTable->setItem(i, 6, type);
            }
            else if( eDate == QDate::currentDate() && lookupStructs.reservations[i].timeDelivered != EMPTY_FIELD_RETURNED )
            {
                QTableWidgetItem *type = new QTableWidgetItem();
                type->setText( "Pickup" );
                type->setFlags(itemList->flags() ^ Qt::ItemIsEditable);
                ui->lookupTable->setItem(i, 6, type);
            }
            else if(sDate == QDate::currentDate() && lookupStructs.reservations[i].timeDelivered != EMPTY_FIELD_RETURNED)
            {
                QTableWidgetItem *type = new QTableWidgetItem();
                type->setText( "Dropped Off" );
                type->setFlags(itemList->flags() ^ Qt::ItemIsEditable);
                ui->lookupTable->setItem(i, 6, type);
            }

        }
    }
    else
    {
        ui->statusLabel->setHidden(false);
        ui->statusLabel->setText(("There are no reservations on ["+ui->lookupDateEdit->date().toString(STANDARD_DATE_FORMAT_2)+"]"));
    }
}

void AdminPanel::on_lookupTable_clicked(const QModelIndex &index)
{
    ui->lookupTable->selectRow(index.row());
}

void AdminPanel::on_viewButton_clicked()
{
    if( ui->lookupTable->currentRow() != -1)
    {
        lookupStructs.updates.resIndex = ui->lookupTable->currentRow();
        ReservationViewer * resView = new ReservationViewer(&lookupStructs, this);
        resView->show();
    }
}

void AdminPanel::on_editReservationButton_clicked()
{
    if( ui->lookupTable->currentRow() != -1)
    {
        lookupStructs.updates.resIndex = ui->lookupTable->currentRow();
        editReservation * editResWindow = new editReservation(&lookupStructs, this);
        connect(editResWindow, SIGNAL(forceUpdate()), this, SLOT(requestForUpdate()));
        editResWindow->show();
    }
}

void AdminPanel::requestForUpdate()
{
    ui->lookupTable->clear();
    QString viewDate = ui->lookupDateEdit->date().toString(STANDARD_DATE_FORMAT_2);
    purgeLookupStructs();
    socketConn.SubmitQuery(ADMIN_QUERY,7,0,viewDate);
    populateLookupTable();
}
