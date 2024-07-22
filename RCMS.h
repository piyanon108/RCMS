#ifndef RCMS_H
#define RCMS_H

#include <QObject>
#include <ChatServer.h>
#include <SocketClient.h>
#include <Database.h>
#include <QJsonObject>
#define FILESETTING "/etc/rcms/settings.ini"
class RCMS : public QObject
{
    Q_OBJECT
public:
    explicit RCMS(QObject *parent = nullptr);

signals:

public slots:

private:
    int userID = 0;
    int roleID = 0;
    QString roleName = "";
    ChatServer *socketServer;
    Database *myDatabase;
    QTimer *connectToClientTimer;
    struct clientNode
    {
        int deviceIndex = 0;
        int roleIndex = 0;
        QString name = "";
        SocketClient *socketClient;
        QString ipAddress = "";
        int roleID = 0;
        QString roleName = "";
        int userID = 0;
        int templateID = 0;
        QString type = "";
        int subType = 0;
        QString databaseUser = "";
        QString databaseName = "";
        QString databasePassword = "";
        uint16_t socketPort = 0;
        QString description = "";
        int deviceIdInRole = 0;
        int clientCountGetInfo = 0;
        int timeout = 3;
    };

    QList<clientNode *> client_list;
    QList<clientNode *> client_active_list;
    int ClientActiveIDCheck(int roleIndex);
    int ClientIDCheck(int clientID);
    void myConfigurate();
    void updateUserActive();
    void broadcastConnStatus();
    void getMessageFromClient();


private slots:
    void insertClientInDatabase(int roleID = 0, QString roleName = "", int userID = 0,  int templateID = 0, QString name = "", QString ipAddress = "", QString username = "admin", QString password = "password");
    void insertNewDeviceInRole(int roleID = 0,QString roleName = "",int deviceID = 0);
    void newCommandProcess(QJsonObject command, QWebSocket *pSender);
    void appendNewClient(int id = 0, QString name = "", QString ipAddress = "", int roleID = 0, QString roleName = "", int userID = 0, QString type = "", int subType = 0, QString databaseUser = "", QString databaseName = "", QString databasePassword = "", uint16_t socketPort = 0, QString description = "", int templateID = 0);
    void appendNewActiveClient(int roleIndex=0, int deviceIndex = 0, int deviceIdInRole = 0, QString name = "", QString ipAddress = "", int roleID = 0, QString roleName = "", int userID = 0, QString type = "", int subType = 0, QString databaseUser = "", QString databaseName = "", QString databasePassword = "", uint16_t socketPort = 0, QString description = "", int templateID = 0);
    void changeRoleName(int roleID, QString roleName);
    void updateClientInDatabase(int roleID = 0, QString roleName = "", int userID = 0,  int templateID = 0, QString Name = "", QString ipAddress = "", int id = 0, QString username = "admin", QString password = "password");
    void deleteClient(int id = 0);
    void changeDeviceInRole(int roleindex, int newDeviceID);
    void deleteDeviceIdInRole(int roleIndex);
    void deleteRoleID(int roleID);
    void changeActiveRoleID(int newRoleID);
    void connectToClient();
    void disconnectFromClient(int id);

    void TextMessageReceivedFromClient(QString message, int roleIndex, QString ipaddress);
    void sendCommandToDevice(QString message, int roleIndex, QJsonObject obj);
    void socketClientClosed(int roleIndex, QString ipaddress);
    void onDeviceConnected(int roleIndex, QString ipaddress);
};

#endif // RCMS_H
