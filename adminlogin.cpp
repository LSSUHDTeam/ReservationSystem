#include "adminlogin.h"
#include "ui_adminlogin.h"
#include <QCryptographicHash>

AdminLogin::AdminLogin(_dataStructs * _structs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminLogin)
{
    ui->setupUi(this);
    ui->passwordField->setEchoMode(QLineEdit::Password);
    dataStructs = _structs;
}

AdminLogin::~AdminLogin()
{
    delete ui;
}

void AdminLogin::on_cancelButton_clicked()
{
    emit accessReport(false);
    this->close();
}

void AdminLogin::on_loginButton_clicked()
{
    // attempt login
    md5Pass = QString(QCryptographicHash::hash((ui->passwordField->text().toUtf8()),QCryptographicHash::Md5).toHex());
    user = ui->usernameField->text();
    QString information = ui->usernameField->text() + QUERY_DELIMETER + md5Pass;

    socketClient socketConn;
    connect(&socketConn, SIGNAL(dataReady(QString)), this, SLOT(processData(QString)));
    socketConn.SubmitQuery(ADMIN_QUERY, 1, 0 ,information);
}

void AdminLogin::processData(QString data)
{
    switch(data.toInt())
    {
    case 1:
        dataStructs->currentUser.userName = user;
        dataStructs->currentUser.md5Pass = md5Pass;
        emit accessReport(1);
        this->close();
        break;
    case 2:
        ui->infoLabel->setText("Incorrect Password");
        break;
    case 3:
        ui->infoLabel->setText("Unknown User");
        break;
    default:
        break;
    }
}

void AdminLogin::on_AdminLogin_rejected()
{
    emit accessReport(false);
}

void AdminLogin::on_passwordField_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    ui->infoLabel->setText(" ");
}
