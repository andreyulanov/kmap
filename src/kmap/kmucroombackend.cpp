#include "kmucroombackend.h"

KMucRoomBackEnd::KMucRoomBackEnd(QObject *parent)
    : QObject{parent}
{
    // nothing to do
}

void KMucRoomBackEnd::setRoomJid(QString _room_jid)
{
    if (room_jid != _room_jid)
    {
        room_jid = _room_jid;
        roomJidChanged();
    }
}
QString KMucRoomBackEnd::roomJid()
{
    return room_jid;
}
