#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QtSql>
#include <QString>
#include <QStringList>
class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QString dbName, QString user, QString password, QString host, QObject *parent = nullptr);
    void restartMysql();
    bool database_createConnection();
    void insertDataLogger(int txIndex, double fwdPowerWatt, double fwdPowerDB, double maxPowerWatt, double rwdPowerWatt, double rwdPowerDB, double vswr, bool connectionStatus, QString stationName, int frequency, int duration, QString startLog, QString endLog);

    void getActiveClientInDatabase(int userID, int roleID);
    void getAllClientInDatabase(int userID);
    void getNewClientInDatabase(int userID);
    void insertClientInDatabase(int roleID = 0, QString roleName = "", int userID = 0,  int templateID = 0, QString Name = "", QString ipAddress = "", QString username = "admin", QString password = "password");
    void updateClientInDatabase(int roleID = 0, QString roleName = "", int userID = 0,  int templateID = 0, QString Name = "", QString ipAddress = "", int id = 0, QString username = "admin", QString password = "password");
    void insertNewDeviceInRole(int roleID = 0,QString roleName = "",int deviceID = 0);
    bool checkSameDiviceInrole(int roleID = 0, int deviceID = 0);
    void deleteClient(int id = 0);
    void changeDeviceInRole(int id, int clientID);
    void deleteDeviceIdInRole(int id);
    void deleteRoleID(int roleID);
    void changeRoleName(int roleID, QString roleName);

    QString currentRoleName = "";
    int currentRoleID = 0;
    int currentUserID = 0;
signals:
    void mysqlError();
    void appendNewClient(int id = 0, QString name = "", QString ipAddress = "", int roleID = 0, QString roleName = "", int userID = 0, QString type = "", int subType = 0, QString databaseUser = "", QString databaseName = "", QString databasePassword = "", uint16_t socketPort = 0, QString description = "", int templateID = 0);
    void appendNewActiveClient(int roleIndex=0, int deviceIndex = 0, int deviceIdInRole = 0, QString name = "", QString ipAddress = "", int roleID = 0, QString roleName = "", int userID = 0, QString type = "", int subType = 0, QString databaseUser = "", QString databaseName = "", QString databasePassword = "", uint16_t socketPort = 0, QString description = "", int templateID = 0);


public slots:

private:
    int getNewRoleID();
    int getDeviceIdInRole(int roleID);
    QSqlDatabase db;


};

#endif // DATABASE_H
