#include "Database.h"

Database::Database(QString dbName, QString user, QString password, QString host,QObject *parent) :
    QObject(parent)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(password);
    database_createConnection();
}

void Database::restartMysql()
{
    system("systemctl stop mysqld");
    system("systemctl start mysqld");

    qDebug() << "Restart MySQL";
}

bool Database::database_createConnection()
{
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
        return false;
    }
    db.close();
    qDebug() << "Database connected";

    return true;
}
void Database::getActiveClientInDatabase(int userID, int roleID)
{
    int deviceIndex = 0;
    int deviceIdInRole = 0;
    int roleIndex = 0;
    QString name = "";
    QString ipAddress = "";
    QString roleName = "";
    QString type = "";
    int subType = 0;
    QString databaseUser = "";
    QString databaseName = "";
    QString databasePassword = "";
    int socketPort = 0;
    QString description = "";
    int templateID = 0;
    QString query = QString("SELECT role.deviceID, role.roleName, role.roleID, role.deviceIdInRole, role.id AS roleIndex, "
                            "deviceList.id AS deviceIndex, deviceList.Name, deviceList.templateID, deviceList.ipaddress, deviceList.username, deviceList.password, "
                            "deviceTemplate.iconName, deviceTemplate.type, deviceTemplate.subType, deviceTemplate.databaseUser , deviceTemplate.databaseName , deviceTemplate.databasePassword, deviceTemplate.socketPort, deviceTemplate.description "
                            "FROM deviceList INNER JOIN deviceTemplate ON deviceTemplate.id=deviceList.templateID INNER JOIN role ON role.deviceID = deviceList.id WHERE role.userID=%1 AND role.roleID=%2 ORDER BY role.deviceIdInRole").arg(userID).arg(roleID);
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
    }
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }else
    {
        while (qry.next()) {
            roleIndex = qry.value("roleIndex").toInt();
            deviceIdInRole = qry.value("deviceIdInRole").toInt();
            deviceIndex = qry.value("deviceIndex").toInt();
            name = qry.value("Name").toString();
            ipAddress = qry.value("ipaddress").toString();
            roleName = qry.value("roleName").toString();
            roleID= qry.value("roleID").toInt();
            type = qry.value("type").toString();
            subType = qry.value("subType").toInt();
            databaseUser = qry.value("databaseUser").toString();
            databaseName = qry.value("databaseName").toString();
            databasePassword = qry.value("databasePassword").toString();
            socketPort = qry.value("socketPort").toInt();
            description = qry.value("description").toString();
            templateID = qry.value("templateID").toInt();
            emit appendNewActiveClient(roleIndex, deviceIndex, deviceIdInRole, name, ipAddress, roleID , roleName, userID, type, subType, databaseUser, databaseName, databasePassword, uint16_t(socketPort), description, templateID);
        }
    }
    currentUserID = userID;
    currentRoleID = roleID;
    currentRoleName = roleName;
    db.close();
}
void Database::getAllClientInDatabase(int userID)
{
    int id = 0;
    QString name = "";
    QString ipAddress = "";
    QString roleName = "";
    QString type = "";
    int subType = 0;
    QString databaseUser = "";
    QString databaseName = "";
    QString databasePassword = "";
    int socketPort = 0;
    QString description = "";
    int templateID = 0;
    int roleID = 0;
    QString query = QString("SELECT deviceList.id, deviceList.Name, deviceList.templateID, deviceList.ipaddress, deviceList.roleName,  deviceList.roleID, "
                            "deviceTemplate.type, deviceTemplate.subType, deviceTemplate.databaseUser, deviceTemplate.databasePassword, deviceTemplate.databaseName, deviceTemplate.socketPort, deviceTemplate.description "
                            "FROM deviceList INNER JOIN deviceTemplate ON deviceTemplate.id=deviceList.templateID WHERE userID=%1 AND roleID=%2").arg(userID).arg(roleID);
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
    }
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }else
    {
        while (qry.next()) {
            id = qry.value("id").toInt();
            name = qry.value("Name").toString();
            ipAddress = qry.value("ipaddress").toString();
            roleName = qry.value("roleName").toString();
            roleID= qry.value("roleID").toInt();
            type = qry.value("type").toString();
            subType = qry.value("subType").toInt();
            databaseUser = qry.value("databaseUser").toString();
            databaseName = qry.value("databaseName").toString();
            databasePassword = qry.value("databasePassword").toString();
            socketPort = qry.value("socketPort").toInt();
            description = qry.value("description").toString();
            templateID = qry.value("templateID").toInt();
            emit appendNewClient(id, name, ipAddress, roleID , roleName, userID, type, subType, databaseUser, databaseName, databasePassword, uint16_t(socketPort), description, templateID);
        }
    }
    currentUserID = userID;
    db.close();
}
int Database::getDeviceIdInRole(int roleID)
{
    int deviceIdInRole = 0;
    QString query = QString("SELECT deviceIdInRole FROM role WHERE roleID=%1 ORDER BY deviceIdInRole DESC LIMIT 1").arg(roleID);
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
    }
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }else
    {
        while (qry.next())
        {
            deviceIdInRole = qry.value("deviceIdInRole").toInt();
        }
    }
    deviceIdInRole++;
 db.close();
 return  deviceIdInRole;
}

int Database::getNewRoleID()
{
    int roleID = 0;
    QString query = QString("SELECT roleID FROM role ORDER BY roleID DESC LIMIT 1");
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
    }
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }else
    {
        while (qry.next())
        {
            roleID = qry.value("roleID").toInt();
        }
    }
    roleID++;
 db.close();
 return  roleID;
}

void Database::deleteRoleID(int roleID)
{
    if (!db.open())
    {
        qWarning() << "c++: ERROR! "  << "database error! database can not open.";
        emit mysqlError();
        return;
    }
    QString query = QString("DELETE FROM role WHERE roleID=%1").arg(roleID);
    qDebug() << "deleteRoleID" << query;
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }
    db.close();
}

void Database::changeRoleName(int roleID, QString roleName)
{
    if (!db.open())
    {
        qWarning() << "c++: ERROR! "  << "database error! database can not open.";
        emit mysqlError();
        return;
    }
    QSqlQuery qry;
    qry.prepare("UPDATE role SET roleName = :roleName "
                "WHERE roleID = :roleID");
    qry.addBindValue(roleName);
    qry.addBindValue(roleID);
    if (!qry.exec()){
        qWarning() << "c++: ERROR! "  << qry.lastError();
    }
    db.close();
}
bool Database::checkSameDiviceInrole(int roleID, int deviceID)
{
    int _roleID = 0;
    int _deviceD = 0;
    QString query = QString("SELECT roleID, deviceID FROM role WHERE roleID=%1 AND deviceID=%2 LIMIT 1").arg(roleID).arg(deviceID);
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
    }
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }else
    {
        while (qry.next())
        {
            _roleID = qry.value("roleID").toInt();
            _deviceD = qry.value("deviceID").toInt();
        }
    }
 db.close();
 return ((_roleID == roleID) & (deviceID == _deviceD));
}
void Database::insertNewDeviceInRole(int roleID, QString roleName,int deviceID)
{
    if (checkSameDiviceInrole(roleID,deviceID)) return;
    int deviceIdInRole = getDeviceIdInRole(roleID);
    if (roleID == 0) roleID=getNewRoleID();

    if (!db.open())
    {
        qWarning() << "c++: ERROR! "  << "database error! database can not open.";
        emit mysqlError();
        return;
    }
    QSqlQuery qry;
    qry.prepare("INSERT INTO role (roleID , deviceID , roleName , userID, deviceIdInRole) "
                  "VALUES (:roleID , :deviceID , :roleName , :userID , :deviceIdInRole)");
    qry.bindValue(":roleID", roleID);
    qry.bindValue(":deviceID", deviceID);
    qry.bindValue(":roleName", roleName);
    qry.bindValue(":userID", currentUserID);
    qry.bindValue(":deviceIdInRole", deviceIdInRole);
    if (!qry.exec()){
        qWarning() << "c++: ERROR! "  << qry.lastError();
    }
    db.close();
}
void Database::getNewClientInDatabase(int userID)
{
    int id = 0;
    QString name = "";
    QString ipAddress = "";
    QString roleName = "";
    QString type = "";
    int subType = 0;
    QString databaseUser = "";
    QString databaseName = "";
    QString databasePassword = "";
    int socketPort = 0;
    QString description = "";
    int templateID = 0;
    int roleID = 0;
    QString query = QString("SELECT deviceList.id, deviceList.Name, deviceList.templateID, deviceList.ipaddress, deviceList.roleName,  deviceList.roleID, "
                            "deviceTemplate.type, deviceTemplate.subType, deviceTemplate.databaseUser, deviceTemplate.databasePassword, deviceTemplate.databaseName, deviceTemplate.socketPort, deviceTemplate.description "
                            "FROM deviceList INNER JOIN deviceTemplate ON deviceTemplate.id=deviceList.templateID WHERE userID=%1 AND roleID=%2 ORDER BY ID DESC LIMIT 1").arg(userID).arg(roleID);
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
    }
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }else
    {
        while (qry.next()) {
            id = qry.value("id").toInt();
            name = qry.value("Name").toString();
            ipAddress = qry.value("ipaddress").toString();
            roleName = qry.value("roleName").toString();
            roleID= qry.value("roleID").toInt();
            type = qry.value("type").toString();
            subType = qry.value("subType").toInt();
            databaseUser = qry.value("databaseUser").toString();
            databaseName = qry.value("databaseName").toString();
            databasePassword = qry.value("databasePassword").toString();
            socketPort = qry.value("socketPort").toInt();
            description = qry.value("description").toString();
            templateID = qry.value("templateID").toInt();
            emit appendNewClient(id, name, ipAddress, roleID , roleName, userID, type, subType, databaseUser, databaseName, databasePassword, uint16_t(socketPort), description, templateID);
        }
    }
    currentRoleName = roleName;
    db.close();
}

void Database::deleteClient(int id)
{
    QString query = QString("DELETE FROM deviceList WHERE id=%1").arg(id);
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        restartMysql();
    }
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }
    db.close();
}

void Database::insertClientInDatabase(int roleID, QString roleName, int userID, int templateID, QString Name, QString ipAddress, QString username, QString password)
{
    if (!db.open())
    {
        qWarning() << "c++: ERROR! "  << "database error! database can not open.";
        emit mysqlError();
        return;
    }
    QSqlQuery qry;
    qry.prepare("INSERT INTO deviceList (Name , templateID , ipaddress , roleID , roleName , userID, username, password) "
                  "VALUES (:Name, :templateID, :ipAddress, :roleID, :roleName, :userID, :username, :password)");
    qry.bindValue(":Name", Name);
    qry.bindValue(":templateID", templateID);
    qry.bindValue(":ipAddress", ipAddress);
    qry.bindValue(":roleID", roleID);
    qry.bindValue(":roleName", roleName);
    qry.bindValue(":userID", userID);
    qry.bindValue(":username", username);
    qry.bindValue(":password", password);
    if (!qry.exec()){
        qWarning() << "c++: ERROR! "  << qry.lastError();
    }
    db.close();

}

void Database::updateClientInDatabase(int roleID, QString roleName, int userID, int templateID, QString Name, QString ipAddress, int id, QString username, QString password)
{
    if (!db.open())
    {
        qWarning() << "c++: ERROR! "  << "database error! database can not open.";
        emit mysqlError();
        return;
    }
    QSqlQuery qry;
    qry.prepare("UPDATE deviceList SET Name = :Name , templateID = :templateID , ipaddress = :ipaddress , username = :username , password = :password "
                "WHERE id = :id");
    qry.addBindValue(Name);
    qry.addBindValue(templateID);
    qry.addBindValue(ipAddress);    
    qry.addBindValue(username);
    qry.addBindValue(password);
    qry.addBindValue(id);
    qDebug() << id << "updateClientInDatabase" << qry.executedQuery() << Name << templateID << ipAddress << roleID << roleName << userID << username << password;
    if (!qry.exec()){
        qWarning() << "c++: ERROR! "  << qry.lastError();
    }
    db.close();
}

void Database::insertDataLogger(int txIndex, double fwdPowerWatt, double fwdPowerDB, double maxPowerWatt, double rwdPowerWatt, double rwdPowerDB, double vswr, bool connectionStatus, QString stationName, int frequency, int duration, QString startLog, QString endLog)
{
    QString query = QString("INSERT INTO datalogger (txIndex , fwdPowerWatt , fwdPowerDB , maxPowerWatt , rwdPowerWatt , rwdPowerDB , vswr , connectionStatus , stationName , frequency , duration , startLog , endLog) "
                            "VALUES (%1, %2, %3, %4, %5, %6, %7, %8, '%9', %10, %11, '%12', '%13')"
                            ).arg(txIndex).arg(fwdPowerWatt).arg(fwdPowerDB).arg(maxPowerWatt).arg(rwdPowerWatt).arg(rwdPowerDB).arg(vswr).arg(connectionStatus).arg(stationName).arg(frequency).arg(duration).arg(startLog).arg(endLog);
    qDebug() << query;
    if (!db.open())
    {
        qWarning() << "c++: ERROR! "  << "database error! database can not open.";
        emit mysqlError();
        return;
    }
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qWarning() << "c++: ERROR! "  << qry.lastError();
    }
    db.close();
}


void Database::changeDeviceInRole(int roleindex, int newDeviceID)
{
    if (!db.open())
    {
        qWarning() << "c++: ERROR! "  << "database error! database can not open.";
        emit mysqlError();
        return;
    }
    QSqlQuery qry;
    qry.prepare("UPDATE role SET deviceID = :newDeviceID "
                "WHERE id = :roleindex");
    qry.addBindValue(newDeviceID);
    qry.addBindValue(roleindex);
    qDebug() << "changeDeviceInRole" << qry.executedQuery() << roleindex << newDeviceID;
    if (!qry.exec()){
        qWarning() << "c++: ERROR! "  << qry.lastError();
    }
    db.close();
}

void Database::deleteDeviceIdInRole(int roleindex)
{
    if (!db.open())
    {
        qWarning() << "c++: ERROR! "  << "database error! database can not open.";
        emit mysqlError();
        return;
    }
    QString query = QString("DELETE FROM role WHERE id=%1").arg(roleindex);
    QSqlQuery qry;
    qry.prepare(query);
    if (!qry.exec()){
        qDebug() << qry.lastError();
    }
    db.close();
}
