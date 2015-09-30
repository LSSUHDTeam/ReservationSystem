#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QHash>
#include "logwriter.h"
#include "errwindow.h"


#define DELIVERY_QUERY 1
#define DEVICE_QUERY 2
#define GENERATE_QUERY 3
#define ADMIN_QUERY 4
#define CONN_QUERY 5


class socketClient : public QObject
{
    Q_OBJECT
public:
    explicit socketClient(QObject *parent = 0);

    void SubmitQuery(int type, int index, int id = 0, QString update = "");

    QPair<int,int> lastQuery;

signals:
    void dataReady(QString);

public slots:
    void errWindowDoneDispaying();

private :

    // Connection Variables
    int port;
    QString host;
    QTcpSocket *socket;

    // Selecet Queries
    QHash<int, QString> delivery_queries,
    device_queries, generate_queries,
    admin_queries, connection_queries;
    qint64 sizeLastTransfer;

    LogWriter logWriter;

    bool connectionIssueAlert;
    void generateError(QString eCode, QString eText, int wErr);

};

#endif // SOCKETCLIENT_H
