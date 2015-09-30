#ifndef ERRWINDOW_H
#define ERRWINDOW_H

#include <QDialog>
#include "structures.h"

namespace Ui {
class errWindow;
}

class errWindow : public QDialog
{
    Q_OBJECT

public:
    explicit errWindow( struct errorStruct * _err, QWidget *parent = 0);
    ~errWindow();

signals:
    void doneDisplaying();

private slots:
    void on_cancelButton_clicked();

    void on_okButton_clicked();

private:
    Ui::errWindow *ui;
    struct errorStruct * currErr;

    void setupWindow();
};

#endif // ERRWINDOW_H
