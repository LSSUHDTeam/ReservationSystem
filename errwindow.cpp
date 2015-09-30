#include "errwindow.h"
#include "ui_errwindow.h"

errWindow::errWindow( struct errorStruct * _err, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::errWindow)
{
    currErr = _err;

    ui->setupUi(this);

    setupWindow();
}

errWindow::~errWindow()
{
    delete ui;
}

void errWindow::setupWindow()
{
    ui->errNameLabel->setText(currErr->errCode);
    ui->errorText->insertPlainText(currErr->errText);

    if ( currErr->errOkText.length() > 0 )
    {
        ui->okButton->setText(currErr->errOkText);
    }

    if ( currErr->errCancelText.length() > 0 )
    {
        ui->cancelButton->setText(currErr->errCancelText);
    }
}

void errWindow::on_cancelButton_clicked()
{
    currErr->userOption = 0;
    emit doneDisplaying();
    this->close();
}

void errWindow::on_okButton_clicked()
{
    currErr->userOption = 1;
    emit doneDisplaying();
    this->close();
}
