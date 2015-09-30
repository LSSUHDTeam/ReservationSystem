#ifndef PICKUPWINDOW_H
#define PICKUPWINDOW_H

#include <QDialog>
#include "structures.h"
#include "socketclient.h"
#include <QDateTime>
#include "globals.h"

namespace Ui {
class PickupWindow;
}

class PickupWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PickupWindow(_dataStructs * dataStructs, QWidget *parent = 0);
    ~PickupWindow();

signals:
    void forceUpdate();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_saveButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::PickupWindow *ui;

    _dataStructs * dataStructs;

    void setupWindow();
    QString timePickedUp;
    int currRecordID;
    int currPickup;
};

#endif // PICKUPWINDOW_H
