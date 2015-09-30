#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QDate"
#include <QTimer>
#include <QSound>
#include <QMainWindow>
#include "jobalert.h"
#include "structures.h"
#include "adminpanel.h"
#include "adminlogin.h"
#include "socketclient.h"
#include "pickupwindow.h"
#include "newreservation.h"
#include "deliverywindow.h"
#include "reservationviewer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void requestForUpdate();
    void processData(QString);

private slots:
    void on_newReservationBtn_clicked();

    void on_editReservation_clicked();

    void on_pickupRetrievedBtn_clicked();

    void on_deliveredButton_clicked();

    void on_viewDetailsButton_clicked();

    void updateTimerTimeOut();

    void checkToRunAlert();

    void on_deliveriesTable_clicked(const QModelIndex &index);

    void on_pickupsTable_clicked(const QModelIndex &index);

    void on_actionAdmin_Panel_triggered();

    void on_actionForce_Update_triggered();

    void adminReturnAccess(bool);

    void adminPanelCloseEvent();


private:
    Ui::MainWindow *ui;

    _dataStructs dataStructs;

    socketClient socketConn;

    QStringList reservationTableColumns,
    pickupTableColumns;

    QStringList donePickupAlerts, doneDeliveryAlerts;

    int deliveryObjectToColor;
    int pickupObjectToColor;

    void setupMainScreen();
    void commenceUpdateChain();
    void purgePreviousRecords();
    void populateReservations(QStringList);
    void populatePickups(QStringList);
    void populateLinks(QStringList);
    void populateInventory(QStringList);
    void populateSchedule(QStringList);
    void populateRooms(QStringList);


    void updateScreen();

    bool updatingStructures;
    bool showingLogin;
    bool showingAdminPanel;

    bool connectedToApi;
    bool apiConnectedToServer;

    QTimer updateTimer;
    QTimer checkForNeedAlert;

};

#endif // MAINWINDOW_H
