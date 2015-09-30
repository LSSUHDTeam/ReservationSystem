#include "noteswindow.h"
#include "ui_noteswindow.h"

NotesWindow::NotesWindow(struct reservation * theReservation, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NotesWindow)
{
    ui->setupUi(this);

    reservedCharachters << RESULT_MEMBER_DELIMETER << INCOMING_RESULT_DELIMETER << PACKAGE_DELIMETER
                        << DEVICE_LIST_DELIMETER << QUERY_DELIMETER << SERVER_SEND_DELIMITER << ","
                        << "'" << QString('"') << QString('\n') << QString('\t') << "_" << "`" << "?" << ";";

    editingReservation = theReservation;

    if ( editingReservation->notes != EMPTY_DB_ENTRY )
    {
        editingNote = editingReservation->notes;
        ui->plainTextEdit->insertPlainText(editingNote);
    }
    else
    {
        editingNote = "";
    }
}

NotesWindow::~NotesWindow()
{
    delete ui;
}

void NotesWindow::on_saveButton_clicked()
{
    /*
    editingNote = ui->plainTextEdit->toPlainText();

    // Filter Reserved Words
    QString temp = "";
    foreach(QString ch, editingNote)
    {
        if( !reservedCharachters.contains(ch) )
        {
            temp += ch;
        }
    }
    editingNote = temp;

    if ( editingNote.length() < 1 )
    {
        editingNote = EMPTY_DB_ENTRY;
    }

    editingReservation->notes = editingNote;
    */

    editingNote = subroutine_filterText(ui->plainTextEdit->toPlainText());
    editingReservation->notes = editingNote;
    this->close();
}

void NotesWindow::on_cancelButton_clicked()
{
    this->close();
}

void NotesWindow::on_resetButton_clicked()
{
    ui->plainTextEdit->clear();
    if ( editingReservation->notes != EMPTY_DB_ENTRY )
    {
        editingNote = editingReservation->notes;
        ui->plainTextEdit->insertPlainText(editingNote);
    }
    else
    {
        editingNote = "";
    }
}

void NotesWindow::on_removeButton_clicked()
{
    editingNote = "";
    ui->plainTextEdit->clear();
}
