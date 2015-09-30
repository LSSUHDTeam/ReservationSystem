#ifndef RESERVATIONVIEWER_H
#define RESERVATIONVIEWER_H

#include <QDialog>
#include "structures.h"
#include "globals.h"

namespace Ui {
class ReservationViewer;
}

class ReservationViewer : public QDialog
{
    Q_OBJECT

public:
    explicit ReservationViewer(_dataStructs * _structs, QWidget *parent = 0);
    ~ReservationViewer();

private slots:
    void on_doneButton_clicked();

private:
    Ui::ReservationViewer *ui;

    void setupWindow();

    _dataStructs * dataStructs;
};

#endif // RESERVATIONVIEWER_H
