#include "jobalert.h"
#include "ui_jobalert.h"

JobAlert::JobAlert(QString Message, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JobAlert)
{
    ui->setupUi(this);

    ui->plainTextEdit->insertPlainText(Message);

    alert = new QSound(ALERT_SOUND, this);
    alert->setLoops(100);
    alert->play();
}

JobAlert::~JobAlert()
{
    delete ui;
}

void JobAlert::on_pushButton_clicked()
{
    alert->stop();
    this->close();
}
