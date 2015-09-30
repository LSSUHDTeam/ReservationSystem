#ifndef ADMINPANEL_H
#define ADMINPANEL_H

#include <QDialog>
#include "globals.h"
#include "structures.h"
#include "socketclient.h"
#include "errwindow.h"
#include "adminconflicthandler.h"
#include "editreservation.h"
#include "reservationviewer.h"
#include <QCryptographicHash>

namespace Ui {
class AdminPanel;
}

class AdminPanel : public QDialog
{
    Q_OBJECT

public:
    explicit AdminPanel(_dataStructs * dataStructs, QWidget *parent = 0);
    ~AdminPanel();

signals:
    void adminPanelClosed();
    void forceUpdate();

private slots:
    void on_AdminPanel_rejected();

    void on_clearButton_clicked();

    void on_deviceTable_clicked(const QModelIndex &index);

    void on_removeButton_clicked();

    void on_loadButton_clicked();

    void on_addButton_clicked();

    void errWindowDoneDispaying();

    void processData(QString);

    void conflictHandled();

    void on_changePasswordButton_clicked();

    void on_searchButton_clicked();

    void on_lookupTable_clicked(const QModelIndex &index);

    void on_viewButton_clicked();

    void on_editReservationButton_clicked();

    void requestForUpdate();

private:
    Ui::AdminPanel *ui;
    _dataStructs * dataStructs;
    _dataStructs lookupStructs;
    socketClient socketConn;
    struct errorStruct adminErr;
    QString idOfConflictingDevice;

    bool editingExistingDevice;
    int whichErr;

    void updateWindow();
    bool formValidForSave();
    void populateLookupStructs(QStringList);
    void purgeLookupStructs();
    void populateLookupTable();

    QStringList reservedCharachters, deviceNames, deviceBarcodes, conflicts;
    QString filterText(QString);


    void generateError(QString eCode, QString eText, int wErr);
};

#endif // ADMINPANEL_H
