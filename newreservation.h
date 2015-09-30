#ifndef NEWRESERVATION_H
#define NEWRESERVATION_H

#include "scheduler.h"
#include "structures.h"
#include "noteswindow.h"
#include "addlocation.h"
#include "errwindow.h"

#include <QDialog>

namespace Ui {
class newReservation;
}

class newReservation : public QDialog
{
    Q_OBJECT


public:
    explicit newReservation(_dataStructs * dataStructs, QWidget *parent = 0);
    ~newReservation();

signals:
    void forceUpdate();

public slots:
    void addLocationClosed();
    void errWindowDoneDispaying();

private slots:
    void on_saveButton_clicked();

    void on_endTimeEdit_dateTimeChanged(const QDateTime &dateTime);

    void on_startTimeEdit_dateTimeChanged(const QDateTime &dateTime);

    void on_repeatedCheckBox_clicked();

    void on_repetitionSelect_currentIndexChanged(int index);

    void on_addItemButton_clicked();

    void on_removeItemButton_clicked();

    void on_newLocationButton_clicked();

    void on_addNoteButton_clicked();

    void on_resetButton_clicked();

    void on_cancelButton_clicked();

    void on_resForText_returnPressed();

    void on_nonItemButton_clicked();

private:
    Ui::newReservation *ui;

    _dataStructs * dataStructs;
    struct reservation theNewReservation;
    scheduler * scheduleManager;

    QStringList repeatCombo;
    QStringList repeatAbbrs;

    QStringList itemsReserved;
    QStringList itemsThatCanBeReserved;
    QStringList listOfRepeatedDates;

    bool setupController;
    bool nonItemReservation;
    int whichErr;

    void setupWindow();
    void dateTimeChanged();
    void setReservation();

    bool checkUserEntry();

    void generateError(QString eCode, QString eText, int wErr);



};

#endif // NEWRESERVATION_H
