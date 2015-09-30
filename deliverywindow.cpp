#include "deliverywindow.h"
#include "ui_deliverywindow.h"

DeliveryWindow::DeliveryWindow(_dataStructs * _structs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeliveryWindow)
{
    dataStructs = _structs;
    ui->setupUi(this);
    setupWindow();
}

DeliveryWindow::~DeliveryWindow()
{
    delete ui;
}

void DeliveryWindow::setupWindow()
{
    currIndex = dataStructs->updates.resIndex;
    QString today = QDateTime::currentDateTime().toString(STANDARD_DATE_FORMAT);
    ui->recIdLabel->setText(dataStructs->reservations[currIndex].resId);
    ui->timeDeliveredLabel->setText(today);
}

void DeliveryWindow::on_deliveredByTextBox_textChanged(const QString &arg1)
{
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

void DeliveryWindow::on_cancelButton_clicked()
{
    this->close();
}

void DeliveryWindow::on_saveButton_clicked()
{
    socketClient socketConn;

    QString update = subroutine_filterText(ui->deliveredByTextBox->text());
    update += QUERY_DELIMETER;
    update += ui->timeDeliveredLabel->text();

    socketConn.SubmitQuery(DELIVERY_QUERY, 9,
                           dataStructs->reservations[currIndex].resId.toInt(),
                           update);
    dataStructs->updates.resIndex = 0;
    emit forceUpdate();
    this->close();
}
