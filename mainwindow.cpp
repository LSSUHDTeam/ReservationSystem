#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "editreservation.h"
#include "errwindow.h"

#include "globals.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup Socket Connection
    connect(&socketConn, SIGNAL(dataReady(QString)), this, SLOT(processData(QString)));

    // Setup Update Timer Connection
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateTimerTimeOut()));

    // Setup alert timer
    connect(&checkForNeedAlert, SIGNAL(timeout()), this, SLOT(checkToRunAlert()));

    apiConnectedToServer = false;
    connectedToApi = false;
    updatingStructures = false;
    showingAdminPanel = false;
    showingLogin = false;

    // Test to see if we are connected to API
    socketConn.SubmitQuery(CONN_QUERY, 1);

    // Test to see if API is connected to 10. network
    socketConn.SubmitQuery(CONN_QUERY, 2);

    setupMainScreen();

    if(connectedToApi && apiConnectedToServer)
    {
        commenceUpdateChain();
        updateTimer.start(STANDARD_UPDATE_TIME);
        checkForNeedAlert.start(ALERT_TIMER);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMainScreen()
{
    // Reservation Table
    reservationTableColumns << "Rec#" << "Time" << "For" << "Location" << "Items Reserved";
    ui->deliveriesTable->setColumnCount(5);
    ui->deliveriesTable->setHorizontalHeaderLabels(reservationTableColumns);
    ui->deliveriesTable->setColumnWidth(0, 50);
    ui->deliveriesTable->setColumnWidth(3, 85);
    ui->deliveriesTable->setColumnWidth(4, 369);

    // Pickup Table
    pickupTableColumns << "Rec#" << "Pickup Time" << "Location" << "Items Reserved";
    ui->pickupsTable->setColumnCount(4);
    ui->pickupsTable->setHorizontalHeaderLabels(pickupTableColumns);
    ui->pickupsTable->setColumnWidth(0, 50);
    ui->pickupsTable->setColumnWidth(1, 150);
    ui->pickupsTable->setColumnWidth(2, 85);
    ui->pickupsTable->setColumnWidth(3, 419);
}

void MainWindow::commenceUpdateChain()
{
    if (updatingStructures)
    {
        purgePreviousRecords();
    }
    else
    {
        // If first time ran, initiate counters out to 0
        dataStructs.counters.resCount = 0;
        dataStructs.counters.pickCount = 0;
        dataStructs.counters.linkCount = 0;
        dataStructs.counters.invCount = 0;
        dataStructs.counters.roomCount = 0;
        dataStructs.counters.schedCount = 0;
    }

    // On first update, assume that the server is connected.
    // From this update it can be determined if it should keep trying

    // Pupulate Deliveries
    socketConn.SubmitQuery(1,1);

    // Populate Pickups
    socketConn.SubmitQuery(1,6);

    // Populate Links
    socketConn.SubmitQuery(1,2);

    // Populate Inventory
    socketConn.SubmitQuery(1,3);

    // Populate Schedule
    socketConn.SubmitQuery(1,4);

    // Populate Rooms
    socketConn.SubmitQuery(1,5);

    // Update displayed data
    updateScreen();
}

void MainWindow::processData(QString data)
{
    if (data == "NC")
    {
        ui->connectionIndicator->setText("API can not reach server");
    }
    else if ( data != EMPTY_DB_ENTRY )
    {
        ui->connectionIndicator->setText("Connected to server");

        ui->deliveriesTable->selectRow(-1);
        ui->pickupsTable->selectRow(-1);

        QStringList dataList = data.split(INCOMING_RESULT_DELIMETER);

        switch( socketConn.lastQuery.first )
        {
            case DELIVERY_QUERY:

                switch( socketConn.lastQuery.second )
                {
                    case 1:
                        // Daily deliveries update
                        populateReservations(dataList);
                        break;
                    case 2:
                        // Grab Links table
                        populateLinks(dataList);
                        break;
                    case 3:
                        // Grab Inventory
                        populateInventory(dataList);
                        break;
                    case 4:
                        // Grab Schedule
                        populateSchedule(dataList);
                        break;
                    case 5:
                        // Grab Room Names
                        populateRooms(dataList);
                        break;
                    case 6:
                        // Grab Pickups
                        populatePickups(dataList);

                        break;

                    default:
                        qDebug() << "Error in delivery query [ lastQuery.second ]";
                        break;
                }

                break; // END DELIVERY QUERY CASE

            case DEVICE_QUERY:

                switch( socketConn.lastQuery.second )
                {
                    case 1:
                        // All Devices

                    default:
                        qDebug() << "Error in devie query [ lastQuery.second ]";
                        break;
                }

                break; // END DEVICE QUERY CASE

            case CONN_QUERY:

                switch( socketConn.lastQuery.second )
                {
                    case 1:
                        if (data == "y")
                        {
                            ui->connectionIndicator->setText("Connected to API");
                            connectedToApi = true;
                        }
                        else
                        {
                            ui->connectionIndicator->setText("Not connected to API");
                            connectedToApi = false;
                        }
                    break;

                    case 2:
                        if (data == "y")
                        {
                            ui->connectionIndicator->setText("Connected to server");
                            apiConnectedToServer = true;
                        }
                        else
                        {
                            if(connectedToApi)
                            {
                                ui->connectionIndicator->setText("API not connected to server");
                                apiConnectedToServer = false;
                            }
                            else
                                apiConnectedToServer = false;
                        }
                    break;

                    default:
                        qDebug() << "Error in conn query [ lastQuery.second ]";
                        break;
                }

                break; // END CONN QUERY

            default:
                qDebug() << "Error Processing Data";
                break;
        }
    }
    else
    {
        qDebug() << "No data returned from server";
        ui->connectionIndicator->setText("API Unreachable");
    }
}

// Purge Existing Records
void MainWindow::purgePreviousRecords()
{
    int i;

    // Purge Reservations
    for( i = 0 ; i < dataStructs.counters.resCount; i++)
    {
        dataStructs.reservations[i].resId = "";
        dataStructs.reservations[i].resStart = "";
        dataStructs.reservations[i].resEnd = "";
        dataStructs.reservations[i].resFor = "";
        dataStructs.reservations[i].resBy = "";
        dataStructs.reservations[i].location = "";
        dataStructs.reservations[i].deliveredBy = "";
        dataStructs.reservations[i].pickedupBy = "";
        dataStructs.reservations[i].timeDelivered = "";
        dataStructs.reservations[i].timePickedup = "";
        dataStructs.reservations[i].notes = "";
        dataStructs.reservations[i].inventoryList = "";
        dataStructs.reservations[i].repeatedIds = "";
    }
    dataStructs.counters.resCount = 0;

    // Purge Pickups
    for( i = 0 ; i < dataStructs.counters.pickCount; i++)
    {
        dataStructs.pickups[i].resId = "";
        dataStructs.pickups[i].resStart = "";
        dataStructs.pickups[i].resEnd = "";
        dataStructs.pickups[i].resFor = "";
        dataStructs.pickups[i].resBy = "";
        dataStructs.pickups[i].location = "";
        dataStructs.pickups[i].deliveredBy = "";
        dataStructs.pickups[i].pickedupBy = "";
        dataStructs.pickups[i].timeDelivered = "";
        dataStructs.pickups[i].timePickedup = "";
        dataStructs.pickups[i].notes = "";
        dataStructs.pickups[i].inventoryList = "";
        dataStructs.pickups[i].repeatedIds = "";
    }
    dataStructs.counters.pickCount = 0;

    // Purge Links
    for( i = 0 ; i < dataStructs.counters.linkCount; i++)
    {
        dataStructs.linkedInformation[i].repeatedID = "";
        dataStructs.linkedInformation[i].reservations = "";
        dataStructs.linkedInformation[i].type = "";
        dataStructs.linkedInformation[i].firstOccurance = "";
        dataStructs.linkedInformation[i].lastOccurance = "";
    }
    dataStructs.counters.linkCount = 0;

    // Purge Inventory
    for( i = 0 ; i < dataStructs.counters.invCount; i++)
    {
        dataStructs.inventoryItems[i].id = "";
        dataStructs.inventoryItems[i].title = "";
        dataStructs.inventoryItems[i].desc = "";
        dataStructs.inventoryItems[i].type = "";
        dataStructs.inventoryItems[i].barcode = "";
        dataStructs.inventoryItems[i].lastCheckOut = "";
        dataStructs.inventoryItems[i].lastCheckIn = "";
        dataStructs.inventoryItems[i].nextCheckout = "";
    }
    dataStructs.counters.invCount = 0;

    // Purge Schedule
    for( i = 0 ; i < dataStructs.counters.schedCount; i++)
    {
        dataStructs.itemSchedules[i].inventoryItemID = "";
        dataStructs.itemSchedules[i].datesUnavailable.clear();
    }
    dataStructs.counters.schedCount = 0;

    // Purge Rooms
    for( i = 0; i < dataStructs.counters.roomCount; i++ )
    {
        dataStructs.schoolRooms[i].id = "";
        dataStructs.schoolRooms[i].name = "";
        dataStructs.schoolRooms[i].desc = "";
    }
    dataStructs.counters.roomCount = 0;
}

// This function is first link in update chain
void MainWindow::populateReservations(QStringList data)
{

    int i;
    for( i = 0 ; i < data.count(); i++)
    {
        QStringList current = data[i].split(RESULT_MEMBER_DELIMETER);
        dataStructs.reservations[i].resId = current[0];
        dataStructs.reservations[i].resStart = current[1];
        dataStructs.reservations[i].resEnd = current[2];
        dataStructs.reservations[i].resFor = current[3];
        dataStructs.reservations[i].resBy = current[4];
        dataStructs.reservations[i].location = current[5];
        dataStructs.reservations[i].deliveredBy = current[6];
        dataStructs.reservations[i].pickedupBy = current[7];
        dataStructs.reservations[i].timeDelivered = current[8];
        dataStructs.reservations[i].timePickedup = current[9];
        dataStructs.reservations[i].notes = current[10];
        dataStructs.reservations[i].inventoryList = current[11];
        dataStructs.reservations[i].repeatedIds = current[12];
    }
    dataStructs.counters.resCount = i;
}

void MainWindow::populatePickups(QStringList data)
{
    int i;
    for( i = 0 ; i < data.count(); i++)
    {
        QStringList current = data[i].split(RESULT_MEMBER_DELIMETER);
        dataStructs.pickups[i].resId = current[0];
        dataStructs.pickups[i].resStart = current[1];
        dataStructs.pickups[i].resEnd = current[2];
        dataStructs.pickups[i].resFor = current[3];
        dataStructs.pickups[i].resBy = current[4];
        dataStructs.pickups[i].location = current[5];
        dataStructs.pickups[i].deliveredBy = current[6];
        dataStructs.pickups[i].pickedupBy = current[7];
        dataStructs.pickups[i].timeDelivered = current[8];
        dataStructs.pickups[i].timePickedup = current[9];
        dataStructs.pickups[i].notes = current[10];
        dataStructs.pickups[i].inventoryList = current[11];
        dataStructs.pickups[i].repeatedIds = current[12];
    }
    dataStructs.counters.pickCount = i;
}

void MainWindow::populateLinks(QStringList data)
{
    int i;
    for( i = 0 ; i < data.count(); i++)
    {
        QStringList current = data[i].split(RESULT_MEMBER_DELIMETER);
        dataStructs.linkedInformation[i].repeatedID = current[0];
        dataStructs.linkedInformation[i].reservations = current[1];
        dataStructs.linkedInformation[i].type = current[2];
        dataStructs.linkedInformation[i].firstOccurance = current[3];
        dataStructs.linkedInformation[i].lastOccurance = current[4];
    }
    dataStructs.counters.linkCount = i;


}

void MainWindow::populateInventory(QStringList data)
{
    int i;
    for( i = 0 ; i < data.count(); i++)
    {
        QStringList current = data[i].split(RESULT_MEMBER_DELIMETER);
        dataStructs.inventoryItems[i].id = current[0];
        dataStructs.inventoryItems[i].title = current[1];
        dataStructs.inventoryItems[i].desc = current[2];
        dataStructs.inventoryItems[i].type = current[3];
        dataStructs.inventoryItems[i].barcode = current[4];
    }
    dataStructs.counters.invCount = i;
}

void MainWindow::populateSchedule(QStringList data)
{
    int i;
    for( i = 0 ; i < data.count(); i++)
    {
        dataStructs.itemSchedules[i].inventoryItemID = data[i].split(DEVICE_LIST_DELIMETER)[0];
        QString dates = data[i].split(DEVICE_LIST_DELIMETER)[1];
        dataStructs.itemSchedules[i].datesUnavailable = dates.split(PACKAGE_DELIMETER);
    }
    dataStructs.counters.schedCount = i;
}

void MainWindow::populateRooms(QStringList data)
{
    int i;
    for( i = 0; i < data.count(); i++ )
    {
        QStringList roomInfo = data[i].split(RESULT_MEMBER_DELIMETER);
        dataStructs.schoolRooms[i].id = roomInfo[0];
        dataStructs.schoolRooms[i].name = roomInfo[1];
        dataStructs.schoolRooms[i].desc = roomInfo[2];
    }
    dataStructs.counters.roomCount = i;
}


void MainWindow::requestForUpdate()
{
    apiConnectedToServer = false;
    connectedToApi = false;
    // Test to see if we are connected to API
    socketConn.SubmitQuery(CONN_QUERY, 1);

    // Test to see if API is connected to 10. network
    socketConn.SubmitQuery(CONN_QUERY, 2);

    if(connectedToApi && apiConnectedToServer)
    {
        updatingStructures = true;
        commenceUpdateChain();
        updatingStructures = false;
        updateScreen();
    }
}

void MainWindow::updateScreen()
{
    // Todays date in [yyyy-MM-dd] format
    QString todaysDate = QDate::currentDate().toString(STANDARD_DATE_FORMAT_2);

    /*

            POPULATE DELIVERY TABLE

    */

    // Get number of deliveries that are to be displayed today
    int deliveryCount = 0;
    for( int i = 0; i < dataStructs.counters.resCount; i++ )
    {
        if( dataStructs.reservations[i].resStart.split(" ")[0] == todaysDate )
        {
            deliveryCount++;
        }
    }

    // Set the row count to that number
    ui->deliveriesTable->setRowCount(deliveryCount);

    QPalette* palette = new QPalette();
    palette->setColor(QPalette::Highlight,Qt::red);
    ui->deliveriesTable->setPalette(*palette);

    // Make sure that there are deliveries
    if( ui->deliveriesTable->rowCount() > 0 )
    {
        // Go through all deliveries
        for( int i = 0; i < dataStructs.counters.resCount; i++)
        {
            // 0 = Date , 1 = Time
            QStringList reservationDateTimeInformation = dataStructs.reservations[i].resStart.split(" ");

            // Show the ones for today
            if( reservationDateTimeInformation[0] == todaysDate)
            {
                QTableWidgetItem *rec = new QTableWidgetItem();
                rec->setText( dataStructs.reservations[i].resId );
                rec->setFlags(rec->flags() ^ Qt::ItemIsEditable);
                ui->deliveriesTable->setItem(i, 0, rec);

                QTableWidgetItem *time = new QTableWidgetItem();
                time->setText(reservationDateTimeInformation[1]);
                time->setFlags(time->flags() ^ Qt::ItemIsEditable);
                ui->deliveriesTable->setItem(i, 1, time);

                QTableWidgetItem *whoFor = new QTableWidgetItem();
                whoFor->setText( dataStructs.reservations[i].resFor );
                whoFor->setFlags(whoFor->flags() ^ Qt::ItemIsEditable);
                ui->deliveriesTable->setItem(i, 2, whoFor);

                QTableWidgetItem *location = new QTableWidgetItem();
                location->setText(  dataStructs.reservations[i].location );
                location->setFlags(location->flags() ^ Qt::ItemIsEditable);
                ui->deliveriesTable->setItem(i, 3, location);

                QString inventoryNames = "";
                QStringList invItems = dataStructs.reservations[i].inventoryList.split(",");

                for( int x = 0; x < invItems.count(); x++)
                {
                    inventoryNames += subroutine_retrieveItemNameById(invItems[x], dataStructs);

                    if( x != invItems.count()-1 )
                        inventoryNames += ", ";
                }

                QTableWidgetItem *items = new QTableWidgetItem();
                items->setText(  inventoryNames );
                items->setFlags(items->flags() ^ Qt::ItemIsEditable);
                ui->deliveriesTable->setItem(i, 4, items);
            }
        }
    }

    /*

            POPULATE PICKUP TABLE

    */

    // Set the row count to # of pickups
    ui->pickupsTable->setRowCount(dataStructs.counters.pickCount);
    ui->pickupsTable->setPalette(*palette);

    // Make sure that there are pickups
    if( ui->pickupsTable->rowCount() > 0 )
    {
        // Go through all pickups, because if they got to the program then they need to be done.
        for( int i = 0; i < dataStructs.counters.pickCount; i++)
        {
            QTableWidgetItem *rec = new QTableWidgetItem();
            rec->setText( dataStructs.pickups[i].resId );
            rec->setFlags(rec->flags() ^ Qt::ItemIsEditable);
            ui->pickupsTable->setItem(i, 0, rec);

            QTableWidgetItem *time = new QTableWidgetItem();
            time->setText(dataStructs.pickups[i].resEnd.split(" ")[1]);
            time->setFlags(time->flags() ^ Qt::ItemIsEditable);
            ui->pickupsTable->setItem(i, 1, time);

            QTableWidgetItem *location = new QTableWidgetItem();
            location->setText(  dataStructs.pickups[i].location );
            location->setFlags(location->flags() ^ Qt::ItemIsEditable);
            ui->pickupsTable->setItem(i, 2, location);

            QString inventoryNames = "";
            QStringList invItems = dataStructs.pickups[i].inventoryList.split(",");

            for( int x = 0; x < invItems.count(); x++)
            {
                inventoryNames += subroutine_retrieveItemNameById(invItems[x], dataStructs);
                if( x != invItems.count()-1 )
                    inventoryNames += ", ";
            }

            QTableWidgetItem *items = new QTableWidgetItem();
            items->setText( inventoryNames );
            items->setFlags(items->flags() ^ Qt::ItemIsEditable);
            ui->pickupsTable->setItem(i, 3, items);
        }
    }
}

void MainWindow::on_newReservationBtn_clicked()
{
    newReservation * newResWindow = new newReservation( &dataStructs, this);
    connect(newResWindow, SIGNAL(forceUpdate()), this, SLOT(requestForUpdate()));
    newResWindow->setAttribute( Qt::WA_DeleteOnClose, true );
    newResWindow->show();
}

void MainWindow::on_pickupRetrievedBtn_clicked()
{
    if ( ui->pickupsTable->currentRow() >= 0)
    {
        dataStructs.updates.pickIndex = ui->pickupsTable->currentRow();
        PickupWindow * newPickupWindow = new PickupWindow(&dataStructs, this);
        connect(newPickupWindow, SIGNAL(forceUpdate()), this, SLOT(requestForUpdate()));
        newPickupWindow->setAttribute( Qt::WA_DeleteOnClose, true );
        newPickupWindow->show();

        ui->pickupsTable->selectRow(-1);
    }
}

void MainWindow::on_editReservation_clicked()
{
    if ( ui->deliveriesTable->currentRow() >= 0)
    {
        dataStructs.updates.resIndex = ui->deliveriesTable->currentRow();
        editReservation * editResWindow = new editReservation(&dataStructs, this);
        connect(editResWindow, SIGNAL(forceUpdate()), this, SLOT(requestForUpdate()));
        editResWindow->setAttribute( Qt::WA_DeleteOnClose, true );
        editResWindow->show();
    }
}

void MainWindow::on_deliveredButton_clicked()
{
    if ( ui->deliveriesTable->currentRow() >= 0)
    {
        dataStructs.updates.resIndex = ui->deliveriesTable->currentRow();
        DeliveryWindow * newDeliveryWindow = new DeliveryWindow(&dataStructs, this);
        connect(newDeliveryWindow, SIGNAL(forceUpdate()), this, SLOT(requestForUpdate()));
        newDeliveryWindow->setAttribute( Qt::WA_DeleteOnClose, true );
        newDeliveryWindow->show();

        ui->deliveriesTable->selectRow(-1);
    }
}

void MainWindow::on_viewDetailsButton_clicked()
{
    if ( ui->deliveriesTable->currentRow() >= 0)
    {
        dataStructs.updates.resIndex = ui->deliveriesTable->currentRow();
        ReservationViewer * newResViewer = new ReservationViewer(&dataStructs, this);
        newResViewer->setAttribute( Qt::WA_DeleteOnClose, true );
        newResViewer->show();
    }
}

void MainWindow::updateTimerTimeOut()
{
    requestForUpdate();
    updateTimer.start(STANDARD_UPDATE_TIME);
}

void MainWindow::checkToRunAlert()
{
    QTime now = QTime::currentTime();

    // Check Deliveries
    QStringList currDeliveries;
    for(int i = 0 ; i < dataStructs.counters.resCount; i++)
    {
        QDateTime resStart = QDateTime::fromString(dataStructs.reservations[i].resStart, STANDARD_DATE_FORMAT);
        QTime startTime = resStart.time();
        int msecsToDelivery = now.msecsTo(startTime);

        currDeliveries.append(dataStructs.reservations[i].resId);
        if( msecsToDelivery <= STANDARD_UPDATE_TIME && !doneDeliveryAlerts.contains(dataStructs.reservations[i].resId)) // 5 Min
        {
            deliveryObjectToColor = i;
            QString Message = "\n\n  5 Minute warning for delivery [ " + dataStructs.reservations[i].resId +" ]\n";

            JobAlert * newJobAlert = new JobAlert(Message, this);
            newJobAlert->setAttribute( Qt::WA_DeleteOnClose, true );
            newJobAlert->show();

            ui->deliveriesTable->selectRow(i);

            doneDeliveryAlerts.append(dataStructs.reservations[i].resId);
        }
    }

    // Remove no longer existing deliveries from alert list to keep size down
    QStringList deletions;
    foreach(QString el, doneDeliveryAlerts)
    {
        if( !currDeliveries.contains(el))
        {
            deletions.append(el);
        }
    }
    foreach(QString el, deletions)
    {
        doneDeliveryAlerts.removeAt(doneDeliveryAlerts.indexOf(el));
    }

    // Check Pickups
    QStringList currPickups;
    for(int i = 0 ; i < dataStructs.counters.pickCount; i++)
    {
        QDateTime resEnd = QDateTime::fromString(dataStructs.pickups[i].resEnd, STANDARD_DATE_FORMAT);
        QTime endTime = resEnd.time();
        int msecsToDelivery = now.msecsTo(endTime);

        currPickups.append(dataStructs.pickups[i].resId);
        if( msecsToDelivery <= STANDARD_UPDATE_TIME && !donePickupAlerts.contains(dataStructs.pickups[i].resId)) // 5 Min
        {
            pickupObjectToColor = i;
            QString Message = "\n\n  5 Minute warning for pickup [ " + dataStructs.pickups[i].resId +" ]\n";

            JobAlert * newJobAlert = new JobAlert(Message, this);
            newJobAlert->setAttribute( Qt::WA_DeleteOnClose, true );
            newJobAlert->show();

            ui->pickupsTable->selectRow(i);

            // START ALARM

            donePickupAlerts.append(dataStructs.pickups[i].resId);
        }
    }

    // Remove no longer existing pickups from alert list to keep size down
    deletions.clear();
    foreach(QString el, donePickupAlerts)
    {
        if( !currPickups.contains(el))
        {
            deletions.append(el);
        }
    }
    foreach(QString el, deletions)
    {
        donePickupAlerts.removeAt(donePickupAlerts.indexOf(el));
    }

    // Restart Check Counter
    checkForNeedAlert.start(ALERT_TIMER);
}

void MainWindow::on_deliveriesTable_clicked(const QModelIndex &index)
{
    ui->deliveryNotePreview->clear();
    ui->deliveriesTable->selectRow(index.row());
    ui->deliveryNotePreview->insertPlainText(dataStructs.reservations[ui->deliveriesTable->currentRow()].notes);
}

void MainWindow::on_pickupsTable_clicked(const QModelIndex &index)
{
    ui->pickupNotePreview->clear();
    ui->pickupsTable->selectRow(index.row());
    ui->pickupNotePreview->insertPlainText(dataStructs.pickups[ui->pickupsTable->currentRow()].notes);
}

/*

    Admin Panel

*/

void MainWindow::on_actionAdmin_Panel_triggered()
{
    if( !showingLogin && !showingAdminPanel )
    {
        showingLogin = true;
        AdminLogin *login = new AdminLogin(&dataStructs, this);
        connect(login, SIGNAL(accessReport(bool)), this, SLOT(adminReturnAccess(bool)));
        login->setAttribute( Qt::WA_DeleteOnClose, true );
        login->show();
    }
}

void MainWindow::on_actionForce_Update_triggered()
{
    requestForUpdate();
}

void MainWindow::adminReturnAccess(bool returnCode)
{
    showingLogin = false;

    if( returnCode && !showingAdminPanel )
    {
        showingAdminPanel = true;
        AdminPanel * adminPanel = new AdminPanel( &dataStructs, this);
        connect(adminPanel, SIGNAL(adminPanelClosed()), this, SLOT(adminPanelCloseEvent()));
        connect(adminPanel, SIGNAL(forceUpdate()), this, SLOT(requestForUpdate()));
        adminPanel->setAttribute( Qt::WA_DeleteOnClose, true );
        adminPanel->show();
    }
}

void MainWindow::adminPanelCloseEvent()
{
    showingAdminPanel = false;
    requestForUpdate();
}

