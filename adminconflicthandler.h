#ifndef ADMINCONFLICTHANDLER_H
#define ADMINCONFLICTHANDLER_H

#include <QDialog>
#include "socketclient.h"
#include "structures.h"
#include "scheduler.h"
#include <QDateTime>

namespace Ui {
class AdminConflictHandler;
}

class AdminConflictHandler : public QDialog
{
    Q_OBJECT

public:
    explicit AdminConflictHandler(_dataStructs * dataStructs, QStringList conflicts, QWidget *parent = 0);
    ~AdminConflictHandler();

signals:
    void complete();

private slots:
    void on_chooseButton_clicked();

    void on_cancelButton_clicked();

    void on_tableWidget_clicked(const QModelIndex &index);

    void on_AdminConflictHandler_rejected();

private:
    Ui::AdminConflictHandler *ui;

    QStringList currentConflicts;

    _dataStructs * dataStructs;
    socketClient socketConn;

    struct conflictStruct conflictList[MAX_FUTURE_DELIVERIES];
    int conflictCounter, conflictIndex;


    void populateConflictList();
    void updateWindow();

};

#endif // ADMINCONFLICTHANDLER_H
