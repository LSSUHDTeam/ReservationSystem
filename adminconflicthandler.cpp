#include "adminconflicthandler.h"
#include "ui_adminconflicthandler.h"

AdminConflictHandler::AdminConflictHandler(_dataStructs * _struct, QStringList conflicts, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminConflictHandler)
{
    ui->setupUi(this);
    dataStructs = _struct;
    currentConflicts = conflicts;

    updateWindow();
}

AdminConflictHandler::~AdminConflictHandler()
{
    delete ui;
}

void AdminConflictHandler::populateConflictList()
{
    conflictIndex = 0;
    conflictCounter = 0;
    scheduler * scheduleManager = new scheduler(dataStructs, this);
    QStringList repeatedAbbrs = (QStringList() << "D" << "W" << "M" << "TR" << "MWF");

    foreach(QString conflict, currentConflicts)
    {
        int repeatedType = -1;

        QStringList cells = conflict.split(RESULT_MEMBER_DELIMETER);

        conflictList[conflictCounter].conflictType = cells[0];
        conflictList[conflictCounter].conflictTableId = cells[1];
        conflictList[conflictCounter].startDateTime = cells[2];
        conflictList[conflictCounter].endDateTime = cells[3];

        if( cells[0] == "r" )
        {
            conflictList[conflictCounter].repeatType = cells[4];
            conflictList[conflictCounter].itemToReplace = cells[5];
            repeatedType = repeatedAbbrs.indexOf(cells[4]);
            ui->conflictItemLabel->setText(subroutine_retrieveItemNameById(cells[4],*dataStructs));
        }
        else
        {
            ui->conflictItemLabel->setText(subroutine_retrieveItemNameById(cells[4],*dataStructs));
            conflictList[conflictCounter].itemToReplace = cells[4];
            conflictList[conflictCounter].repeatType = EMPTY_DB_ENTRY;
        }


        /*
                Calculate available devices for conflict and add to 'conflictList[conflictCounter].availableDevices'
                This will operate similar to the newReservation form in determining available devices.
        */

        QDateTime startDateTime = QDateTime::fromString(conflictList[conflictCounter].startDateTime, STANDARD_DATE_FORMAT);
        QDateTime endDateTime = QDateTime::fromString(conflictList[conflictCounter].endDateTime, STANDARD_DATE_FORMAT);

        QStringList listOfRepeatedDates = scheduleManager->calculateDifferenceInDates(startDateTime.date(), endDateTime.date(), repeatedType);
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
            if(conflictList[conflictCounter].availableDevices.isEmpty())
            {
                conflictList[conflictCounter].availableDevices = tempItemsThatCanBeReserved;
            }
            else
            {
                tempHoldList.clear();

                // Go through each available device from first day
                foreach(QString ell, conflictList[conflictCounter].availableDevices)
                {
                    // If a once available device is no longer available, mark it for deletion
                    if( !tempItemsThatCanBeReserved.contains(ell) )
                    {
                        deletions.append(conflictList[conflictCounter].availableDevices.indexOf(ell));
                    }
                }

                // Once spots for deletions found, reiterate and determine if removed
                foreach(QString ell, conflictList[conflictCounter].availableDevices)
                {
                    // if deletions doesn't contain the index of current element add to temp list
                    if( !deletions.contains(conflictList[conflictCounter].availableDevices.indexOf(ell)) )
                    {
                        tempHoldList.append(ell);
                    }
                }
                conflictList[conflictCounter].availableDevices = tempHoldList;
            }
        }
        // Increment counter for next conflict
        conflictCounter++;
    }
}

void AdminConflictHandler::updateWindow()
{
    populateConflictList();

    ui->tableWidget->clear();
    QPalette* palette = new QPalette();
    palette->setColor(QPalette::Highlight,Qt::darkMagenta);
    ui->tableWidget->setPalette(*palette);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setRowCount(conflictCounter);
    ui->tableWidget->setHorizontalHeaderLabels((QStringList() << "ID" << "Start Date Time" << "End Date Time" << "Type"));
    ui->tableWidget->setColumnWidth(0, 50);
    ui->tableWidget->setColumnWidth(1, 132);
    ui->tableWidget->setColumnWidth(2, 132);

    for(int i = 0; i < conflictCounter; i++)
    {
        QTableWidgetItem *area = new QTableWidgetItem();
        area->setText( conflictList[i].conflictTableId );
        area->setFlags(area->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i, 0, area);

        QTableWidgetItem *start = new QTableWidgetItem();
        start->setText( conflictList[i].startDateTime );
        start->setFlags(start->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i, 1, start);

        QTableWidgetItem *end = new QTableWidgetItem();
        end->setText( conflictList[i].endDateTime );
        end->setFlags(end->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i, 2, end);

        QTableWidgetItem *type = new QTableWidgetItem();

        if( conflictList[i].repeatType == EMPTY_DB_ENTRY )
        {
            type->setText("Single");
        }
        else
        {
            type->setText("Repeated :" + conflictList[i].repeatType);
        }

        type->setFlags(type->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget->setItem(i, 3, type);
    }
}

void AdminConflictHandler::on_chooseButton_clicked()
{
    if ( ui->listWidget->currentIndex().row() >= 0 )
    {
        currentConflicts.removeAt(conflictIndex);

        QString update = conflictList[conflictIndex].conflictType;
        update += RESULT_MEMBER_DELIMETER;
        update += conflictList[conflictIndex].conflictTableId;
        update += RESULT_MEMBER_DELIMETER;
        update += conflictList[conflictIndex].availableDevices.at(ui->listWidget->currentIndex().row());
        update += RESULT_MEMBER_DELIMETER;
        update += conflictList[conflictIndex].itemToReplace;

        socketConn.SubmitQuery(ADMIN_QUERY,5,0,update);

        if( currentConflicts.length() == 0 )
        {
            ui->chooseButton->setEnabled(false);
            ui->listWidget->clear();
            ui->cancelButton->setText("Done");
        }
        updateWindow();
    }
}
void AdminConflictHandler::on_tableWidget_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    if( ui->tableWidget->currentRow() >= 0)
    {
        ui->listWidget->clear();
        conflictIndex = ui->tableWidget->currentRow();
        ui->tableWidget->selectRow(ui->tableWidget->currentRow());

        // Go through available equipment and populate the list.
        QStringList availableEquipment;
        foreach(QString ID, conflictList[conflictIndex].availableDevices)
        {
            availableEquipment.append(subroutine_retrieveItemNameById(ID, *dataStructs));
        }
        if( availableEquipment.count() > 0 )
        {
            ui->chooseButton->setEnabled(true);
            foreach (QString title, availableEquipment)
            {
                ui->listWidget->addItem(new QListWidgetItem(title));
            }
        }
        else
        {
            ui->chooseButton->setEnabled(false);
            ui->listWidget->addItem(new QListWidgetItem("No available items"));
        }
    }
}

void AdminConflictHandler::on_cancelButton_clicked()
{
    if(ui->cancelButton->text() == "Done")
    {
        emit complete();
    }
    this->close();
}

void AdminConflictHandler::on_AdminConflictHandler_rejected()
{
    if(ui->cancelButton->text() == "Done")
    {
        emit complete();
    }
}
