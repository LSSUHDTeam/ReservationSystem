#include "pickupwindow.h"
#include "ui_pickupwindow.h"

PickupWindow::PickupWindow(_dataStructs * _structs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PickupWindow)
{
    dataStructs = _structs;
    ui->setupUi(this);

    QStringList tableHeaders;
    tableHeaders << "Item" << "Barcode";
    ui->invItemsTable->setColumnCount(2);
    ui->invItemsTable->setHorizontalHeaderLabels(tableHeaders);
    ui->invItemsTable->setColumnWidth(0, 200);
    ui->invItemsTable->setColumnWidth(1, 194);

    setupWindow();
}

PickupWindow::~PickupWindow()
{
    delete ui;
}

void PickupWindow::setupWindow()
{
    currPickup = dataStructs->updates.pickIndex;

    QStringList invItemIds = dataStructs->pickups[currPickup].inventoryList.split(DEVICE_LIST_DELIMETER);

    ui->invItemsTable->setRowCount(invItemIds.count());

    timePickedUp = QDateTime::currentDateTime().toString(STANDARD_DATE_FORMAT);

    foreach(QString el, invItemIds)
    {
        QTableWidgetItem *itemName = new QTableWidgetItem();
        itemName->setText( subroutine_retrieveItemNameById(el, *dataStructs) );
        itemName->setFlags(itemName->flags() ^ Qt::ItemIsEditable);
        ui->invItemsTable->setItem(invItemIds.indexOf(el), 0, itemName);

        QTableWidgetItem *barcode = new QTableWidgetItem();
        barcode->setText( subroutine_retrieveBarcodeById(el, *dataStructs) );
        barcode->setFlags(barcode->flags() ^ Qt::ItemIsEditable);
        ui->invItemsTable->setItem(invItemIds.indexOf(el), 1, barcode);
    }

    currRecordID = dataStructs->pickups[currPickup].resId.trimmed().toInt();

    ui->resIdLabel->setText(dataStructs->pickups[currPickup].resId);
    ui->resForLabel->setText(dataStructs->pickups[currPickup].resFor);
    ui->createdByLabel->setText(dataStructs->pickups[currPickup].resBy);
    ui->deliveryTimeLabel->setText( dataStructs->pickups[currPickup].timeDelivered);
    ui->reservedItemCountLabel->setText(QString::number(invItemIds.count()));
    ui->timeDisplayLabel->setText(timePickedUp);
    ui->expectedPickupTime->setText(dataStructs->pickups[currPickup].resEnd);
}

void PickupWindow::on_lineEdit_textChanged(const QString &arg1)
{
    qDebug() << "Text changed";

    int x = 0;
    foreach(QString el, arg1)
    {
        if ( el != " " )
        {
            x++;
        }
    }

    if (x >= 3)
    {
        ui->saveButton->setEnabled(true);
    }
    else
    {
        ui->saveButton->setEnabled(false);
    }
}

void PickupWindow::on_saveButton_clicked()
{
    ui->saveButton->setEnabled(false);

    QString update = "";
    socketClient socketConn;

    update += timePickedUp;
    update += RESULT_MEMBER_DELIMETER;
    update += subroutine_filterText(ui->lineEdit->text().trimmed());

    socketConn.SubmitQuery(DELIVERY_QUERY, 8, currRecordID, update );

    dataStructs->updates.pickIndex = 0;
    emit forceUpdate();
    this->close();
}

void PickupWindow::on_cancelButton_clicked()
{
    this->close();
}
