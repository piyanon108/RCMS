#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QObject>
#include <QDebug>
#include <QWebSocket>

class SocketClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketClient(QObject *parent = Q_NULLPTR);
    void createConnection(int idInRole, QString ipaddress, quint16 port);
    qint64 sendTextMessage(const QString &message);
    bool isConnected = false;
    QWebSocket m_webSocket;

Q_SIGNALS:
    void closed(int idInRole, QString ipaddress);
    void TextMessageReceived(QString message,int idInRole, QString ipaddress);
    void onDeviceConnected(int clientIndex, QString ipaddress);

private Q_SLOTS:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(QString message);
    void onError(QAbstractSocket::SocketError error);

private:

    QUrl m_url;
    QString m_ipaddress;
    int m_socketID;
    bool m_debug = false;
};

#endif // SOCKETCLIENT_H
