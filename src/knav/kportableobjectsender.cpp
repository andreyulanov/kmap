#include "kportableobjectsender.h"
#include <QList>

KPortableObjectSender::KPortableObjectSender(QObject *parent)
    : QObject{parent}
{}
KPortableObjectSender::~KPortableObjectSender()
{
    clear();
}

void KPortableObjectSender::turnOnSendOnReady()
{
    send_if_ready = true;
}

void KPortableObjectSender::setJid(QList<QString> jids)
{
    if (jids.isEmpty())
    {
        qWarning() << "No jid seleted, abourting...";
        clear();
        return;
    }
    jid = new QString(jids.first());
    sendIfReady();
}
void KPortableObjectSender::setFilename(QString file_path)
{
    this->file_path = new QString(file_path);
    sendIfReady();
}

bool KPortableObjectSender::sendIfReady()
{
    if (send_if_ready && isReady())
    {
        emit send(*jid, *file_path, QString());
        clear();
        return true;
    }
    return false;
}

void KPortableObjectSender::clear()
{
    delete file_path;
    delete jid;

    file_path = nullptr;
    jid = nullptr;
    send_if_ready = false;
}

bool KPortableObjectSender::isReady()
{
    return (file_path != nullptr) && (jid != nullptr);
}
