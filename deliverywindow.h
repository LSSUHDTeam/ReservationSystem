#ifndef DELIVERYWINDOW_H
#define DELIVERYWINDOW_H

#include <QDialog>
#include "structures.h"
#include "globals.h"
#include "socketclient.h"
#include <QDateTime>

namespace Ui {
class DeliveryWindow;
}

class DeliveryWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DeliveryWindow(_dataStructs * _structs, QWidget *parent = 0);
    ~DeliveryWindow();

signals:
    void forceUpdate();

private slots:
    void on_cancelButton_clicked();

    void on_saveButton_clicked();

    void on_deliveredByTextBox_textChanged(const QString &arg1);

private:
    Ui::DeliveryWindow *ui;

    _dataStructs * dataStructs;

    void setupWindow();

    int currIndex;

};



#endif // DELIVERYWINDOW_H
