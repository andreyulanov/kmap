#include <QDebug>
#include "ksettings.h"

KSettings::KSettings(QObject *parent)
    : QObject{parent},
    q_settings("ksys", "knav")
{
    qDebug() << "Load settings from" << q_settings.fileName();
}

QString KSettings::jid()
{
    return q_settings.value("account/login").toString();
}

QString KSettings::password()
{
    return q_settings.value("account/password").toString();
}

void KSettings::saveJid(QString jid)
{
    return q_settings.setValue("account/login", jid);
}

void KSettings::savePassword(QString password)
{
    q_settings.setValue("account/password", password);
}

void KSettings::saveAccount(QString jid, QString password)
{
    saveJid(jid);
    savePassword(password);
    q_settings.sync();
}
