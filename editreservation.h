#ifndef EDITRESERVATION_H
#define EDITRESERVATION_H

#include "scheduler.h"
#include "structures.h"
#include "noteswindow.h"
#include "addlocation.h"
#include "errwindow.h"
#include "socketclient.h"
#include <QDialog>

namespace Ui {
class editReservation;
}

class editReservation : public QDialog
{
    Q_OBJECT

public:
    explicit editReservation(_dataStructs * dataStructs, QWidget *parent = 0);
    ~editReservation();

signals:

    void forceUpdate();

private slots:
    void on_repetitionSelect_currentIndexChanged(int index);

    void on_startTimeEdit_dateTimeChanged(const QDateTime &dateTime);

    void on_endTimeEdit_dateTimeChanged(const QDateTime &dateTime);

    void on_repeatedCheckBox_clicked();

    void on_addItemButton_clicked();

    void on_removeItemButton_clicked();

    void processData(QString);

    void on_newLocationButton_clicked();

    void on_addNoteButton_clicked();

    void on_resetButton_clicked();

    void on_cancelButton_clicked();

    void on_saveButton_clicked();

    void addLocationClosed();

    void errWindowDoneDispaying();

    void on_removeButton_clicked();

private:
    Ui::editReservation *ui;

    _dataStructs * dataStructs;
    struct reservation theNewReservation;
    struct changesMade changeTracker;
    scheduler * scheduleManager;

    QStringList repeatCombo;
    QStringList repeatAbbrs;

    QStringList itemsReserved;
    QStringList itemsThatCanBeReserved;
    QStringList listOfRepeatedDates;
    QStringList itemsInCurrentReservation;
    QString originalNote;

    socketClient socketConn;

    bool isRepeated;
    bool isFirstInSeries;
    bool setupController;
    bool processSwitch;
    int whichErr;
    int currIndex;

    void setupWindow();
    void dateTimeChanged();
    void setReservation();

    bool checkUserEntry();

    void generateError(QString eCode, QString eText, int wErr);
};

#endif // EDITRESERVATION_H
