#include "socketclient.h"
#include "globals.h"

socketClient::socketClient(QObject *parent) : QObject(parent)
{
    // Server Connection Information
    host = "127.0.0.1";
    port = 5002;

    connectionIssueAlert = false;

    // Build Query Hashes

    // Delivery Selects
    delivery_queries.insert(1, "s^fd" );
    delivery_queries.insert(2, "s^lnks" );
    delivery_queries.insert(3, "s^inv" );
    delivery_queries.insert(4, "s^sch" );
    delivery_queries.insert(5, "s^rms");
    delivery_queries.insert(6, "s^pd");
    delivery_queries.insert(8, "u^spe");
    delivery_queries.insert(9, "u^sde");
    delivery_queries.insert(10, "i^rn");
    delivery_queries.insert(11, "i^newr");
    delivery_queries.insert(12, "s^rbi");
    delivery_queries.insert(13, "u^eds");


    // -------------------------------------------------------


    // Device Selects
    device_queries.insert(1, "s^ad");
    device_queries.insert(2, "s^add");
    device_queries.insert(3, "s^dvi");
    device_queries.insert(4, "s^adt");


    // -------------------------------------------------------


    // Generation Queries
    generate_queries.insert(1, "g^nd");


    // -------------------------------------------------------

    admin_queries.insert(1, "a^lgn");
    admin_queries.insert(2, "a^udbi");
    admin_queries.insert(3, "a^add");
    admin_queries.insert(4, "a^rem");
    admin_queries.insert(5, "a^frem");
    admin_queries.insert(6, "a^chng");
    admin_queries.insert(7, "a^fdel");


    // Connection Status Queries
    connection_queries.insert(1, "ci^ira");
    connection_queries.insert(2, "ci^inet");

}

void socketClient::SubmitQuery(int type, int index, int id, QString update)
{
    // Store the current query for err reporting
    lastQuery.first = type;
    lastQuery.second = index;

    // Setup socket, and connect to host
    socket = new QTcpSocket(this);
    socket->connectToHost(host, port);

    if (socket->waitForConnected(5000))
    {
        qDebug() << "Connected!";

        // Byte array for converting qstring to const char *
        QByteArray bytes;
        QString temp;
        const char * query;

        // Get selected query, and convert
        switch(type)
        {
            case DELIVERY_QUERY:

                switch(index)
                {
                case 8:
                    // Update Single Pickup Entry
                    temp = delivery_queries[index] + QUERY_DELIMETER +
                            QString::number(id) + QUERY_DELIMETER + update;
                    bytes = temp.toLocal8Bit();
                    query = bytes.data();
                    break;
                case 9:
                    // Update Single Delivery Entry
                    temp = delivery_queries[index] + QUERY_DELIMETER +
                            QString::number(id) + QUERY_DELIMETER + update;
                    bytes = temp.toLocal8Bit();
                    query = bytes.data();
                    break;
                case 10:
                    // Request to add new room to DB
                    temp = delivery_queries[index] + QUERY_DELIMETER + update;
                    bytes = temp.toLocal8Bit();
                    query = bytes.data();
                    break;
                case 11:
                    // Request to add new delivery
                    temp = delivery_queries[index] + QUERY_DELIMETER + update;
                    bytes = temp.toLocal8Bit();
                    query = bytes.data();
                    break;
                case 12:
                    // Request to select reservation by id, update = id
                    temp = delivery_queries[index] + QUERY_DELIMETER + update;
                    bytes = temp.toLocal8Bit();
                    query = bytes.data();
                    break;
                case 13:
                    temp = delivery_queries[index] + QUERY_DELIMETER + update;
                    bytes = temp.toLocal8Bit();
                    query = bytes.data();
                    break;
                default:
                    bytes = delivery_queries[index].toLocal8Bit();
                    query = bytes.data();
                    break;
                }

                break;

            case DEVICE_QUERY:

                bytes = device_queries[index].toLocal8Bit();
                query = bytes.data();

                break;

            case GENERATE_QUERY:

                temp = generate_queries[index] + QUERY_DELIMETER + update;
                bytes = temp.toLocal8Bit();
                query = bytes.data();

                break;

            case ADMIN_QUERY:

                temp = admin_queries[index] + QUERY_DELIMETER + update;
                bytes = temp.toLocal8Bit();
                query = bytes.data();
                break;

            case CONN_QUERY:

                bytes = connection_queries[index].toLocal8Bit();
                query = bytes.data();
                break;

            default:
                query = "ping";
                break;
        }

        logWriter.writeLog(LOG_FILE_SOCKETS, query);

        // Send query, and recieve response
        socket->write(query);
        socket->waitForBytesWritten(10000);

        QByteArray arr;
        while(!arr.contains(SERVER_RECV_DELIMITER))
        {
            socket->waitForReadyRead();
            arr += socket->readAll();
        }

        int b = arr.indexOf(SERVER_RECV_DELIMITER);
        QByteArray message = arr.left(b);
        arr = arr.mid(b);
        emit dataReady(message);
        socket->close();
    }
    else
    {
        if(!connectionIssueAlert)
        {
            connectionIssueAlert = true;
            qDebug() << "No data returned from server";
            generateError("NO CONNECTION", "The server is unreachable", 1);
        }
         qDebug() << "Not Connected!";
         emit dataReady(EMPTY_DB_ENTRY);
         socket->close();
    }
}

void socketClient::errWindowDoneDispaying()
{
    connectionIssueAlert = false;
}

void socketClient::generateError(QString eCode, QString eText, int wErr)
{
    Q_UNUSED(wErr);

    struct errorStruct err;
    err.errCode = eCode;
    err.errText = eText;
    errWindow* newError = new errWindow(&err, 0);
    connect(newError, SIGNAL(doneDisplaying()), this, SLOT(errWindowDoneDispaying()));
    newError->setWindowFlags((Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
    newError->setAttribute( Qt::WA_DeleteOnClose, true );
    newError->show();
}
