#include <RCMS.h>

void RCMS::myConfigurate()
{
    QSettings *settings;
    const QString cfgfile = FILESETTING;
    qDebug() << "Loading configuration from:" << cfgfile;
    if(QDir::isAbsolutePath(cfgfile)){
        settings = new QSettings(cfgfile,QSettings::IniFormat);
        userID     = settings->value("User/userID",1).toInt();
        roleID     = settings->value("User/roleID",1).toInt();
    }
    else{
        qDebug() << "Loading configuration from:" << cfgfile << " FILE NOT FOUND!";
    }
    qDebug() << "Loading configuration completed";
    delete settings;
}

void RCMS::updateUserActive()
{
    QSettings *settings;
    const QString cfgfile = FILESETTING;
    qDebug() << "Loading configuration from:" << cfgfile;
    if(QDir::isAbsolutePath(cfgfile))
    {
        settings = new QSettings(cfgfile,QSettings::IniFormat);
        qDebug() << "Configuration file:" << settings->fileName();
        settings->setValue("User/userID",userID);
        settings->setValue("User/roleID",roleID);
    }
    else
    {
        qDebug() << "Loading configuration from:" << cfgfile << " FILE NOT FOUND!";
    }
    delete settings;
}
