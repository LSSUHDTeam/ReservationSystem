#ifndef NOTESWINDOW_H
#define NOTESWINDOW_H

#include <QDialog>
#include "globals.h"
#include "structures.h"

namespace Ui {
class NotesWindow;
}

class NotesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NotesWindow(struct reservation * theReservation, QWidget *parent = 0);
    ~NotesWindow();

private slots:
    void on_saveButton_clicked();

    void on_cancelButton_clicked();

    void on_resetButton_clicked();

    void on_removeButton_clicked();

private:
    Ui::NotesWindow *ui;

    QString editingNote;

    QStringList reservedCharachters;

    struct reservation * editingReservation;
};

#endif // NOTESWINDOW_H
