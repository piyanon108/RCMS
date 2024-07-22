#include "SocketClient.h"

QT_USE_NAMESPACE

//! [constructor]
SocketClient::SocketClient(QObject *parent) :
    QObject(parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &SocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &SocketClient::onDisconnected);
    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
    [=](QAbstractSocket::SocketError error){
        if (isConnected){
            emit closed(m_socketID, m_ipaddress);
            isConnected = false;
        }
        qDebug() << "Connecting Error: ";
    });
    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &SocketClient::onTextMessageReceived);
}
//! [constructor]

//! [createConnection]
void SocketClient::createConnection(int idInRole, QString ipaddress, quint16 port)
{
    QString url = QString("ws://%1:%2").arg(ipaddress).arg(port);
    QUrl iGateSocketServerUrl(url);
    if (m_debug)
        qDebug() << "WebSocket server:" << url;
    m_url = iGateSocketServerUrl;
    m_ipaddress = ipaddress;
    m_socketID = idInRole;
    m_webSocket.open(QUrl(iGateSocketServerUrl));

}
//! [createConnection]

//! [onConnected]
void SocketClient::onConnected()
{
    isConnected = true;
    qDebug() << "WebSocket connected";

//    QString message = QString("{\"menuID\":\"ServerReg\", \"txIndex\":%1}").arg(m_socketID);
//    m_webSocket.sendTextMessage(message);
    emit onDeviceConnected(m_socketID, m_ipaddress);
}
//! [onConnected]

//! [onTextMessageReceived]
void SocketClient::onTextMessageReceived(QString message)
{
    emit TextMessageReceived(message, m_socketID, m_ipaddress);
}
//! [onTextMessageReceived]

void SocketClient::onDisconnected()
{
    isConnected = false;
    qDebug() << m_ipaddress << "WebSocket disconnected";
    emit closed(m_socketID, m_ipaddress);
}

void SocketClient::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "Connecting Error: ";
}

qint64 SocketClient::sendTextMessage(const QString &message)
{
    qDebug() << "sendTextMessage" << message;
    return m_webSocket.sendTextMessage(message);
}
