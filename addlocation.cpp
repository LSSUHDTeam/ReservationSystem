#include "addlocation.h"
#include "ui_addlocation.h"

AddLocation::AddLocation(_dataStructs * _structs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLocation)
{

    dataStructs = _structs;

    ui->setupUi(this);

    setupWindow();
}

AddLocation::~AddLocation()
{
    delete ui;
}


/*

    Setup the window

*/
void AddLocation::setupWindow()
{
    done = false;

    QStringList rooms = (QStringList() << "Administration" << "Arts Center" << "Aquatics Lab" << "Brady Hall"
                        << "Brown Hall" << "Canusa Hall" << "Chippewa House" << "Cisler"
                        << "Community Service" << "Crawford" << "East Hall" << "Easterday House"
                        << "Erie Hall" << "Eskoonwid Endaad" << "Faculty Resource" << "Fletcher"
                        << "Hillside House" << "Huron" << "Industrial Incubator" << "Laker Hall"
                        << "Library" << "Marquette Hall" << "Michigan Hall" << "Moloney Hall"
                        << "Neveu Hall" << "Newman Center" << "Norris" << "Ontario" << "Osborn Hall"
                        << "Presidents House" << "Ryan House" << "South Hall" << "Student Activity Center"
                        << "Health Care Center" << "Center for Applied Sciences" << "The Village" << "Townhouse");

    ui->comboBox->addItems(rooms);

    ui->errorLabel->setStyleSheet("QLabel { background-color : red; color : black; }");
}


/*

    Check to see if data valid, and is
    not already a room.

*/
void AddLocation::checkRoomValidity()
{
    if (ui->lineEdit->text() == "")
    {
        ui->errorLabel->setText("Room number must be entered");
        ui->errorLabel->show();
    }
    else
    {
        (checkRoomNumber()) ? ui->errorLabel->setText("") :  ui->errorLabel->setText(err);
    }

    if ( ui->errorLabel->text() == "" && checkRoomNumber() )
        setRoomCode();
}


/*

    Set the room code as per user specification and ensure room is not in system

*/
void AddLocation::setRoomCode()
{
    QStringList roomCodes = (QStringList() << "ADMN" << "ARTS" << "ARL" << "BRADY"
                        << "BROWN" << "CANUSA" << "CHIPWA" << "CIS"
                        << "COMSERV" << "CRW" << "ESTHALL" << "EHOUSE"
                        << "EREHALL" << "NATV" << "FREC" << "FLC"
                        << "HH" << "HURON" << "INCUBTR" << "LAKRHALL"
                        << "LIB" << "MRQHALL" << "MICHHALL" << "MOLONY"
                        << "NEVEU" << "NEWMANr" << "NOR" << "ONTARIO" << "OSBORN"
                        << "PRESHOUSE" << "RYAN" << "SOUTHHALL" << "SAC"
                        << "HCC" << "CAS" << "VILLAGE" << "TWNHOUSE");

    QString code = roomCodes[ui->comboBox->currentIndex()];
    code += " ";
    code += ui->lineEdit->text();

    bool inUse = false;
    for( int i = 0; i < dataStructs->counters.roomCount; i++)
    {
        if( code == dataStructs->schoolRooms[i].name )
        {
            inUse = true;
        }
    }

    if( !inUse )
    {
        ui->locationCodeLabel->setText(code);
        ui->locationCodeLabel->show();
    }
    else
    {
        ui->errorLabel->setText("ROOM EXISTS");
        ui->errorLabel->show();
        ui->saveButton->setEnabled(false);
    }
}


/*

    Make sure room number is in a correct format. Ex: 205 or 205C

*/
bool AddLocation::checkRoomNumber()
{
    // Make sure that there is at least a 3 digit number,
    // and that there is only one charachter if any;

    QString enteredText = ui->lineEdit->text();

    int charCount = 0, numCount = 0;
    foreach(QString ch, enteredText)
    {
        ( ch.toInt() || ch == "0") ? numCount++ : charCount++;
    }

    // Make sure format is enforced
    if( charCount > 1 || numCount < 3 || numCount > 3 )
    {
        ui->saveButton->setEnabled(false);
        err = "Room number must contain 3 numbers, \n and no more than 1 charachter.";
        ui->errorLabel->show();
        return 0;
    }

    // Make sure char is after numbers: ex 205C , not 20C5
    if( charCount > 0 )
    {
        if (enteredText.at(enteredText.length()-1).isDigit())
        {
            err = "Charachter must be placed after room number.";
            ui->errorLabel->show();
            return 0;
        }
    }

    ui->saveButton->setEnabled(true);
    ui->errorLabel->hide();
    return 1;
}

void AddLocation::on_comboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    checkRoomValidity();
}

void AddLocation::on_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    checkRoomValidity();
}

void AddLocation::on_resetButton_clicked()
{
    ui->errorLabel->hide();
    ui->lineEdit->setText("");
    ui->comboBox->setCurrentIndex(0);
    ui->saveButton->setEnabled(false);
    // Re-issue blank field error
    checkRoomValidity();
}

void AddLocation::on_cancelButton_clicked()
{
    if( done )
    {
        emit doneEditing();
    }
    this->close();
}

void AddLocation::on_saveButton_clicked()
{
    // Disable save to keep from double saving
    ui->saveButton->setEnabled(false);

    socketClient socketConn;
    socketConn.SubmitQuery(DELIVERY_QUERY, 10, 0, ui->locationCodeLabel->text());

    ui->lineEdit->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->resetButton->setEnabled(false);
    ui->cancelButton->setText("CLOSE");
    ui->locationCodeLabel->setText( "\tROOM ADDED" );
    done = true;
}
