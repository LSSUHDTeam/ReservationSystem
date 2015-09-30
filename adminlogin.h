#ifndef ADMINLOGIN_H
#define ADMINLOGIN_H

#include <QDialog>
#include "socketclient.h"
#include "structures.h"
#include "globals.h"

namespace Ui {
class AdminLogin;
}

class AdminLogin : public QDialog
{
    Q_OBJECT

public:
    explicit AdminLogin(_dataStructs * dataStructs, QWidget *parent = 0);
    ~AdminLogin();

private slots:
    void on_loginButton_clicked();

    void on_cancelButton_clicked();

    void on_AdminLogin_rejected();

    void on_passwordField_textChanged(const QString &arg1);

signals:
    void accessReport(bool);

public slots:
    void processData(QString);

private:
    Ui::AdminLogin *ui;
    _dataStructs * dataStructs;
    QString md5Pass;
    QString user;
};

#endif // ADMINLOGIN_H
