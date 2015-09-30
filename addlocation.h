#ifndef ADDLOCATION_H
#define ADDLOCATION_H

#include <QDialog>
#include "structures.h"
#include "socketclient.h"
#include "QDebug"

namespace Ui {
class AddLocation;
}

class AddLocation : public QDialog
{
    Q_OBJECT

public:
    explicit AddLocation(_dataStructs * _structs, QWidget *parent = 0);
    ~AddLocation();

signals:
    void doneEditing();

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_resetButton_clicked();

    void on_cancelButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::AddLocation *ui;

    _dataStructs * dataStructs;

    void setupWindow();

    void checkRoomValidity();

    bool checkRoomNumber();

    void setRoomCode();

    QString err;

    bool done;
};

#endif // ADDLOCATION_H
