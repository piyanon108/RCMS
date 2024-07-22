#include "RCMS.h"

RCMS::RCMS(QObject *parent) : QObject(parent)
{
    socketServer = new ChatServer(8081);
    connectToClientTimer = new QTimer;
    myDatabase = new Database("rcms","rcms","Ifz8zean6868**","192.168.55.1",this);
    connect(myDatabase,SIGNAL(appendNewClient(int , QString , QString , int , QString , int , QString , int , QString , QString , QString , uint16_t , QString, int )),
            this,SLOT(appendNewClient(int , QString , QString , int , QString , int , QString , int , QString , QString , QString , uint16_t , QString, int )));
    connect(myDatabase,SIGNAL(appendNewActiveClient (int ,int ,int , QString , QString , int , QString , int , QString , int , QString , QString , QString , uint16_t , QString, int )),
            this,SLOT(appendNewActiveClient(int ,int ,int , QString , QString , int , QString , int , QString , int , QString , QString , QString , uint16_t , QString, int )));
    myConfigurate();
    myDatabase->getAllClientInDatabase(userID);
    myDatabase->getActiveClientInDatabase(userID,roleID);

    connectToClientTimer->start(5000);
    connect(connectToClientTimer,SIGNAL(timeout()),this,SLOT(connectToClient()));
    connect(socketServer,SIGNAL(newCommandProcess(QJsonObject , QWebSocket *)),this,SLOT(newCommandProcess(QJsonObject , QWebSocket *)));
}

void RCMS::insertClientInDatabase(int roleID, QString roleName, int userID,  int templateID, QString name, QString ipAddress, QString username, QString password)
{

    qDebug() << "insertClientInDatabase" << roleID << roleName << userID << templateID << name << ipAddress;
    Q_FOREACH (clientNode *client, client_list)
    {
        qDebug() << "insertClientInDatabase" << client->deviceIndex  << client->roleID << client->roleName << client->userID << client->templateID << client->name << client->ipAddress;
        if ((client->ipAddress == ipAddress) & (client->templateID == templateID))
        {            
            return;
        }
    }
    myDatabase->insertClientInDatabase(roleID, roleName, userID,  templateID, name, ipAddress, username, password);
    myDatabase->getNewClientInDatabase(userID);
}

void RCMS::insertNewDeviceInRole(int roleID, QString roleName, int deviceID)
{

    qDebug() << "insertNewDeviceInRole" << roleID << roleName << deviceID;
    myDatabase->insertNewDeviceInRole(roleID, roleName, deviceID);
    if (roleID == myDatabase->currentRoleID){
        myDatabase->getActiveClientInDatabase(userID,roleID);
    }
}

void RCMS::changeRoleName(int roleID, QString roleName)
{
    myDatabase->changeRoleName(roleID, roleName);
}

void RCMS::newCommandProcess(QJsonObject command, QWebSocket *pSender)
{
    QString getCommand =  QJsonValue(command["menuID"]).toString();

    if (getCommand == "editDevice")
    {
        int deviceTemplateID =  QJsonValue(command["deviceTemplateID"]).toInt();
        int id =  QJsonValue(command["currentID"]).toInt();
        QString name =  QJsonValue(command["name"]).toString();
        QString username =  QJsonValue(command["username"]).toString();
        QString password =  QJsonValue(command["password"]).toString();
        QString ipaddress =  QJsonValue(command["ipaddress"]).toString();
        updateClientInDatabase(myDatabase->currentRoleID,myDatabase->currentRoleName,myDatabase->currentUserID,deviceTemplateID,name,ipaddress,id, username, password);
    }
    else if (getCommand == "insertClientInDatabase")
    {
        int deviceTemplateID =  QJsonValue(command["deviceTemplateID"]).toInt();
//        int id =  QJsonValue(command["currentID"]).toInt();
        QString name =  QJsonValue(command["name"]).toString();
        QString username =  QJsonValue(command["username"]).toString();
        QString password =  QJsonValue(command["password"]).toString();
        QString ipaddress =  QJsonValue(command["ipaddress"]).toString();
        insertClientInDatabase(myDatabase->currentRoleID,myDatabase->currentRoleName,myDatabase->currentUserID,deviceTemplateID,name,ipaddress, username, password);
    }
    else if (getCommand == "insertNewDeviceInRole")
    {
        int roleID =  QJsonValue(command["roleID"]).toInt();
        QString roleName =  QJsonValue(command["roleName"]).toString();
        int deviceID =  QJsonValue(command["deviceID"]).toInt();
        insertNewDeviceInRole(roleID,roleName,deviceID);
    }
    else if (getCommand == "changeRoleName")
    {
        int roleID =  QJsonValue(command["roleID"]).toInt();
        QString roleName =  QJsonValue(command["roleName"]).toString();
        changeRoleName(roleID,roleName);
    }
    else if (getCommand == "deleteDevice")
    {
        int id =  QJsonValue(command["currentID"]).toInt();
        deleteClient(id);
    }
    else if (getCommand == "changeDeviceInRole")
    {
        int roleIndex =  QJsonValue(command["roleIndex"]).toInt();
        int changed2Id =  QJsonValue(command["changed2Id"]).toInt();
        changeDeviceInRole(roleIndex,changed2Id);
    }
    else if (getCommand == "deleteDeviceIdInRole")
    {
        int roleIndex =  QJsonValue(command["currentID"]).toInt();
        deleteDeviceIdInRole(roleIndex);
    }
    else if (getCommand == "deleteRoleID")
    {
        int roleIndex =  QJsonValue(command["roleID"]).toInt();
        deleteRoleID(roleIndex);
    }
    else if (getCommand == "ChangeActiveRoleID")
    {
        int ActiveRoleID =  QJsonValue(command["roleID"]).toInt();
        changeActiveRoleID(ActiveRoleID);
    }
    else if (getCommand == "getRolesPage")
    {
        QString message = QString("{\"menuID\":\"currentRoleIdActive\",\"roleID\":%1,\"roleName\":\"%2\"}").arg(myDatabase->currentRoleID).arg(myDatabase->currentRoleName);
        pSender->sendTextMessage(message);
        broadcastConnStatus();
    }
    else if (getCommand == "sendCommandToDevice")
    {

        int roleIndex =  QJsonValue(command["roleIndex"]).toInt();
        QString clientCommand =  QJsonValue(command["command"]).toString();
        QJsonObject clientCommandObj =  QJsonValue(command["command"]).toObject();
        qDebug() << "sendCommandToDevice" << clientCommand;// << command["command"];
        sendCommandToDevice(clientCommand,roleIndex,clientCommandObj);
    }
    qDebug() << "newCommandProcess" << getCommand;

}
void RCMS::broadcastConnStatus()
{
    Q_FOREACH (clientNode *client, client_active_list){
        QJsonObject DeviceConnected;
        if (client->socketClient->isConnected)
            DeviceConnected.insert("menuID", QJsonValue::fromVariant("DeviceConnected"));
        else
            DeviceConnected.insert("menuID", QJsonValue::fromVariant("DeviceDisconnected"));
        DeviceConnected.insert("roleIndex", QJsonValue::fromVariant(client->roleIndex));
        socketServer->broadcastMessage(QJsonDocument(DeviceConnected).toJson(QJsonDocument::Compact));
    }
}
int RCMS::ClientActiveIDCheck(int roleIndex)
{
    int i = 0;
    Q_FOREACH (clientNode *client, client_active_list){
        if (client->roleIndex == roleIndex){
            return i;
        }
        i++;
    }
    return i;
}
int RCMS::ClientIDCheck(int clientID)
{
    int i = 0;
    Q_FOREACH (clientNode *client, client_list){
        if (client->deviceIndex == clientID){
            return i;
        }
        i++;
    }
    return i;
}
void RCMS::appendNewActiveClient(int roleIndex, int deviceIndex, int deviceIdInRole, QString name, QString ipAddress, int roleID, QString roleName, int userID, QString type, int subType, QString databaseUser,
                                 QString databaseName, QString databasePassword, uint16_t socketPort, QString description, int templateID)
{
    int i = (ClientActiveIDCheck(roleIndex));
    qDebug() << "appendNewActiveClient" << roleIndex << name << ipAddress;
    if (i == client_active_list.size())
        client_active_list.append(new clientNode); //Add
    else if(i > client_active_list.size()) return;
    //Update
    client_active_list.at(i)->roleIndex = roleIndex;
    client_active_list.at(i)->deviceIndex = deviceIndex;
    client_active_list.at(i)->deviceIdInRole = deviceIdInRole;
    client_active_list.at(i)->name = name;
    client_active_list.at(i)->ipAddress = ipAddress;
    client_active_list.at(i)->roleID = roleID;
    client_active_list.at(i)->roleName = roleName;
    client_active_list.at(i)->userID = userID;
    client_active_list.at(i)->type = type;
    client_active_list.at(i)->subType = subType;
    client_active_list.at(i)->databaseName = databaseName;
    client_active_list.at(i)->databaseUser = databaseUser;
    client_active_list.at(i)->databasePassword = databasePassword;
    client_active_list.at(i)->socketPort = socketPort;
    client_active_list.at(i)->description = description;
    client_active_list.at(i)->templateID = templateID;

    client_active_list.at(i)->socketClient = new SocketClient;
    connect(client_active_list.at(i)->socketClient,SIGNAL(TextMessageReceived(QString ,int , QString ))
            ,this,SLOT(TextMessageReceivedFromClient(QString, int, QString)));
    connect(client_active_list.at(i)->socketClient,SIGNAL(closed(int , QString ))
            ,this,SLOT(socketClientClosed(int, QString)));
    connect(client_active_list.at(i)->socketClient,SIGNAL(onDeviceConnected(int , QString ))
            ,this,SLOT(onDeviceConnected(int, QString)));
}
void RCMS::appendNewClient(int id, QString name, QString ipAddress, int roleID, QString roleName, int userID, QString type, int subType, QString databaseUser,
                           QString databaseName, QString databasePassword, uint16_t socketPort, QString description, int templateID)
{
    int i = (ClientIDCheck(id));
    qDebug() << "appendNewClient" << id << name << ipAddress;
    if (i == client_list.size())
        client_list.append(new clientNode); //Add
    else if(i > client_list.size()) return;
    //Update
    client_list.at(i)->deviceIndex = id;
    client_list.at(i)->name = name;
    client_list.at(i)->ipAddress = ipAddress;
    client_list.at(i)->roleID = roleID;
    client_list.at(i)->roleName = roleName;
    client_list.at(i)->userID = userID;
    client_list.at(i)->type = type;
    client_list.at(i)->subType = subType;
    client_list.at(i)->databaseName = databaseName;
    client_list.at(i)->databaseUser = databaseUser;
    client_list.at(i)->databasePassword = databasePassword;
    client_list.at(i)->socketPort = socketPort;
    client_list.at(i)->description = description;
    client_list.at(i)->templateID = templateID;

    client_list.at(i)->socketClient = nullptr;
//    connect(client_list.at(i)->socketClient,SIGNAL(TextMessageReceived(QString ,int , QString ))
//            ,this,SLOT(TextMessageReceivedFromClient(QString, int, QString)));
//    connect(client_list.at(i)->socketClient,SIGNAL(closed(int , QString ))
//            ,this,SLOT(socketClientClosed(int, QString)));
//    connect(client_list.at(i)->socketClient,SIGNAL(onDeviceConnected(int , QString ))
//            ,this,SLOT(onDeviceConnected(int, QString)));
}

void RCMS::deleteClient(int id)
{
    myDatabase->deleteClient(id);
    Q_FOREACH (clientNode *client, client_active_list)
    {
        if (client->deviceIndex == id){
            if (client->socketClient->isConnected)
                client->socketClient->m_webSocket.close();
            client_active_list.removeOne(client);
            break;
        }
    }
    Q_FOREACH (clientNode *client, client_list)
    {
        if (client->deviceIndex == id){
            client_list.removeOne(client);
            break;
        }
    }
}
void RCMS::changeDeviceInRole(int roleindex, int newDeviceID)
{
    myDatabase->changeDeviceInRole(roleindex, newDeviceID);
    Q_FOREACH (clientNode *client, client_active_list){
        if (client->deviceIndex == roleindex)
        {
            if (client->socketClient->isConnected)
                client->socketClient->m_webSocket.close();
            client_active_list.removeOne(client);
            //Update client_active_data
            break;
        }
    }
    if (roleindex == roleID)
        myDatabase->getActiveClientInDatabase(userID,roleID);
}
void RCMS::deleteDeviceIdInRole(int roleIndex)
{
    myDatabase->deleteDeviceIdInRole(roleIndex);
    Q_FOREACH (clientNode *client, client_active_list){
        if (client->roleIndex == roleIndex)
        {
            if (client->socketClient->isConnected)
                client->socketClient->m_webSocket.close();
            client_active_list.removeOne(client);
            //Update client_active_data
            break;
        }
    }
}
void RCMS::deleteRoleID(int roleID)
{
    myDatabase->deleteRoleID(roleID);
    Q_FOREACH (clientNode *client, client_active_list){
        if (client->roleIndex == roleID)
        {
            if (client->socketClient->isConnected)
                client->socketClient->m_webSocket.close();
            client_active_list.removeOne(client);
        }
    }
}
void RCMS::changeActiveRoleID(int newRoleID)
{
    Q_FOREACH (clientNode *client, client_active_list)
    {
        if (client->socketClient->isConnected)
            client->socketClient->m_webSocket.close();
        client_active_list.removeOne(client);
    }
    roleID = newRoleID;
    myDatabase->getActiveClientInDatabase(userID,roleID);
}
void RCMS::updateClientInDatabase(int roleID, QString roleName, int userID, int templateID, QString Name, QString ipAddress, int id, QString username, QString password)
{
    myDatabase->updateClientInDatabase(roleID, roleName, userID, templateID, Name, ipAddress, id, username, password);
    Q_FOREACH (clientNode *client, client_active_list)
    {
        if (client->deviceIndex == id)
        {
            if (client->socketClient->isConnected)
                client->socketClient->m_webSocket.close();
            myDatabase->getActiveClientInDatabase(myDatabase->currentUserID,myDatabase->currentRoleID);
            break;
        }
    }

    Q_FOREACH (clientNode *client, client_list)
    {
        if (client->deviceIndex == id)
        {
            if (client->socketClient->isConnected)
            client->name = Name;
            client->templateID = templateID;
            client->ipAddress = ipAddress;
            break;
        }
    }

}

void RCMS::disconnectFromClient(int id)
{
    Q_FOREACH (clientNode *client, client_active_list){
        if (client->deviceIndex == 0)
        {
            if (client->socketClient->isConnected)
                client->socketClient->m_webSocket.close();
        }
        else if (client->deviceIndex == id)
        {
            if (client->socketClient->isConnected)
                client->socketClient->m_webSocket.close();
            break;
        }
    }
}

void RCMS::getMessageFromClient()
{
    Q_FOREACH (clientNode *client, client_active_list)
    {
        if (client->templateID == 12)
        {
            QJsonObject recordObject;
            recordObject.insert("menuID", QJsonValue::fromVariant("getStatusSwitch"));
            client->socketClient->m_webSocket.sendTextMessage(QJsonDocument(recordObject).toJson(QJsonDocument::Compact));
            client->clientCountGetInfo += 1;
            if (client->clientCountGetInfo > client->timeout)
            {
                QJsonObject DeviceConnected;
                DeviceConnected.insert("menuID", QJsonValue::fromVariant("DeviceDisconnected"));
                DeviceConnected.insert("roleIndex", QJsonValue::fromVariant(client->roleIndex));
                socketServer->broadcastMessage(QJsonDocument(DeviceConnected).toJson(QJsonDocument::Compact));
            }
        }
    }
}

void RCMS::connectToClient()
{
    Q_FOREACH (clientNode *client, client_active_list)
    {
        if (client->socketClient->isConnected == false)
        {
            client->socketClient->createConnection(client->roleIndex,client->ipAddress, client->socketPort);
            QJsonObject DeviceConnected;
            DeviceConnected.insert("menuID", QJsonValue::fromVariant("DeviceDisconnected"));
            DeviceConnected.insert("roleIndex", QJsonValue::fromVariant(client->roleIndex));
            socketServer->broadcastMessage(QJsonDocument(DeviceConnected).toJson(QJsonDocument::Compact));
        }
        else
        {

        }
    }
}

void RCMS::sendCommandToDevice(QString message,int roleIndex,QJsonObject obj)
{
    Q_FOREACH (clientNode *client, client_active_list)
    {
        if (client->roleIndex == roleIndex)
        {
            if (message != "")
                client->socketClient->sendTextMessage(message);
            else{
                client->socketClient->sendTextMessage(QJsonDocument(obj).toJson(QJsonDocument::Compact));
            }
            break;
        }
    }
}

void RCMS::TextMessageReceivedFromClient(QString message,int roleIndex, QString ipaddress)
{
    QJsonDocument d = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject command = d.object();
    QString getCommand =  QJsonValue(command["menuID"]).toString();

    QJsonObject recordObject;
    int templateID = 0;
    Q_FOREACH (clientNode *client, client_active_list)
    {
        if (client->roleIndex == roleIndex)
        {
            templateID = client->templateID;
            client->clientCountGetInfo = 0;
            break;
        }
    }
    recordObject.insert("menuID", QJsonValue::fromVariant("messageFromClient"));
    recordObject.insert("templateID", QJsonValue::fromVariant(templateID));
    recordObject.insert("messageRoleID", QJsonValue::fromVariant(roleIndex));
    recordObject.insert("message", command);
    socketServer->broadcastMessage(QJsonDocument(recordObject).toJson(QJsonDocument::Compact));
    qDebug() << "getCommand" << getCommand << roleIndex << ipaddress << (QJsonDocument(recordObject).toJson(QJsonDocument::Compact));
}

void RCMS::socketClientClosed(int roleIndex, QString ipaddress)
{
    qDebug() << "socketClientClosed" << roleIndex << ipaddress;
    QJsonObject DeviceConnected;
    DeviceConnected.insert("menuID", QJsonValue::fromVariant("DeviceDisconnected"));
    DeviceConnected.insert("roleIndex", QJsonValue::fromVariant(roleIndex));
    socketServer->broadcastMessage(QJsonDocument(DeviceConnected).toJson(QJsonDocument::Compact));
}

void RCMS::onDeviceConnected(int roleIndex, QString ipaddress)
{
    qDebug() << "onDeviceConnected" << roleIndex << ipaddress;
    Q_FOREACH (clientNode *client, client_active_list)
    {
        if ((client->socketClient->isConnected == true) & (roleIndex == client->roleIndex))
        {
            qDebug() << "onDeviceConnected" << client->type << client->socketClient->isConnected ;
            QJsonObject DeviceConnected;
            DeviceConnected.insert("menuID", QJsonValue::fromVariant("DeviceConnected"));
            DeviceConnected.insert("roleIndex", QJsonValue::fromVariant(roleIndex));
            socketServer->broadcastMessage(QJsonDocument(DeviceConnected).toJson(QJsonDocument::Compact));
//            recordObject.insert("statusMessage", QJsonValue::fromVariant(""));
            if (client->templateID == 12)
            {
                QJsonObject recordObject;
                recordObject.insert("menuID", QJsonValue::fromVariant("getStatusSwitch"));
                client->socketClient->m_webSocket.sendTextMessage(QJsonDocument(recordObject).toJson(QJsonDocument::Compact));
                QJsonObject getNewName;
                getNewName.insert("menuID", QJsonValue::fromVariant("getNewName"));
                client->socketClient->m_webSocket.sendTextMessage(QJsonDocument(getNewName).toJson(QJsonDocument::Compact));
            }
        }
    }
}
