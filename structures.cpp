#include "structures.h"
#include "QDebug"

QString subroutine_retrieveItemNameById(QString ID,_dataStructs _structs)
{
    if( ID == "" || ID == " ")
    {
        return QString(EMPTY_DB_ENTRY);
    }
    for(int i = 0; i < _structs.counters.invCount; i++)
    {
        if( ID.trimmed() == _structs.inventoryItems[i].id)
        {
            return _structs.inventoryItems[i].title;
        }
    }
    return "Item Not Found";
}

QString subroutine_retrieveDescById(QString ID,_dataStructs _structs)
{
    if( ID == "" || ID == " ")
    {
        return QString(EMPTY_DB_ENTRY);
    }
    for(int i = 0; i < _structs.counters.invCount; i++)
    {
        if( ID.trimmed() == _structs.inventoryItems[i].id)
        {
            return _structs.inventoryItems[i].desc;
        }
    }
    return "Item Not Found";
}

QString subroutine_retrieveTypeById(QString ID,_dataStructs _structs)
{
    if( ID == "" || ID == " ")
    {
        return QString(EMPTY_DB_ENTRY);
    }
    for(int i = 0; i < _structs.counters.invCount; i++)
    {
        if( ID.trimmed() == _structs.inventoryItems[i].id)
        {
            return _structs.inventoryItems[i].type;
        }
    }
    return "Item Not Found";
}

QString subroutine_retrieveBarcodeById(QString ID,_dataStructs _structs)
{
    if( ID == "" || ID == " ")
    {
        return QString(EMPTY_DB_ENTRY);
    }
    for(int i = 0; i < _structs.counters.invCount; i++)
    {
        if( ID.trimmed() == _structs.inventoryItems[i].id)
        {
            return _structs.inventoryItems[i].barcode;
        }
    }
    return "Item Not Found";
}

int subroutine_retrieveLinkIndexByLinkId(QString ID,_dataStructs _structs)
{
    if( ID == "" || ID == " ")
    {
        return -1;
    }
    for(int i = 0; i < _structs.counters.linkCount; i++)
    {
        if( ID.trimmed() == _structs.linkedInformation[i].repeatedID )
        {
            return i;
        }
    }
    return -1;
}

QString subroutine_filterText(QString text)
{
    QStringList res = QStringList() << RESULT_MEMBER_DELIMETER << INCOMING_RESULT_DELIMETER << PACKAGE_DELIMETER
                                    << DEVICE_LIST_DELIMETER << QUERY_DELIMETER << SERVER_SEND_DELIMITER << QString(SERVER_RECV_DELIMITER)
                                     << "," << "'" << QString('"') << QString('\n') << QString('\t') << "_" << "`" << "?" << ";";
    QString temp = "";
    foreach(QString ch, text)
    {
        if( !res.contains(ch) )
        {
            temp += ch;
        }
    }

    if ( temp.length() < 1 )
        return QString(EMPTY_DB_ENTRY);
    else
        return temp;
}
