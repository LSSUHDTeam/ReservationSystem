#ifndef JOBALERT_H
#define JOBALERT_H

#include <QDialog>
#include <QSound>
#include "globals.h"
namespace Ui {
class JobAlert;
}

class JobAlert : public QDialog
{
    Q_OBJECT

public:
    explicit JobAlert(QString Message, QWidget *parent = 0);
    ~JobAlert();

private slots:
    void on_pushButton_clicked();


private:
    Ui::JobAlert *ui;

    QSound * alert;
};

#endif // JOBALERT_H
