#include "reservationviewer.h"
#include "ui_reservationviewer.h"

ReservationViewer::ReservationViewer(_dataStructs * _structs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReservationViewer)
{
    dataStructs = _structs;
    ui->setupUi(this);
    setupWindow();
}

ReservationViewer::~ReservationViewer()
{
    delete ui;
}

void ReservationViewer::setupWindow()
{
    int index = dataStructs->updates.resIndex;

    ui->resIdLabel->setText(dataStructs->reservations[index].resBy);
    ui->startLabel->setText(dataStructs->reservations[index].resStart);
    ui->endLabel->setText(dataStructs->reservations[index].resEnd);
    ui->forLabel->setText(dataStructs->reservations[index].resFor);
    ui->createdByLabel->setText(dataStructs->reservations[index].resBy);
    ui->locationLabel->setText(dataStructs->reservations[index].location);

    ( dataStructs->reservations[index].notes != EMPTY_DB_ENTRY ) ?
                ui->notesField->insertPlainText(dataStructs->reservations[index].notes) :
                ui->notesField->insertPlainText("There are no notes for this delivery.");

    ( dataStructs->reservations[index].repeatedIds != EMPTY_DB_ENTRY ) ?
                ui->repeatedLabel->setText("Yes") :
                ui->repeatedLabel->setText("No");

    QStringList headerLabels;
    headerLabels << "TITLE" << "DESC" << "TYPE" << "BARCODE";

    QStringList itemList = dataStructs->reservations[index].inventoryList.split(DEVICE_LIST_DELIMETER);

    ui->invTable->setColumnCount(4);
    ui->invTable->setRowCount(itemList.count());
    ui->invTable->setHorizontalHeaderLabels(headerLabels);
    ui->invTable->setColumnWidth(0, 160);
    ui->invTable->setColumnWidth(3, 174);

    foreach(QString el, itemList)
    {
        QTableWidgetItem *title = new QTableWidgetItem();
        title->setText( subroutine_retrieveItemNameById(el, *dataStructs) );
        title->setFlags(title->flags() ^ Qt::ItemIsEditable);
        ui->invTable->setItem(itemList.indexOf(el), 0, title);

        QTableWidgetItem *desc = new QTableWidgetItem();
        desc->setText( subroutine_retrieveDescById(el, *dataStructs) );
        desc->setFlags(desc->flags() ^ Qt::ItemIsEditable);
        ui->invTable->setItem(itemList.indexOf(el), 1, desc);

        QTableWidgetItem *type = new QTableWidgetItem();
        type->setText( subroutine_retrieveTypeById(el, *dataStructs) );
        type->setFlags(type->flags() ^ Qt::ItemIsEditable);
        ui->invTable->setItem(itemList.indexOf(el), 2, type);

        QTableWidgetItem *barcode = new QTableWidgetItem();
        barcode->setText( subroutine_retrieveBarcodeById(el, *dataStructs) );
        barcode->setFlags(barcode->flags() ^ Qt::ItemIsEditable);
        ui->invTable->setItem(itemList.indexOf(el), 3, barcode);
    }
}

void ReservationViewer::on_doneButton_clicked()
{
    this->close();
}
