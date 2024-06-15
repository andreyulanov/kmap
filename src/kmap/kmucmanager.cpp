#include "kmucmanager.h"

KMUCManager::KMUCManager(KMUCManager::InvitationReaction _invitation_reaction, AddingReaction _adding_reaction)
{
   setInvitationReaction(_invitation_reaction);
   setAddingReaction(_adding_reaction);
   connect(this, &QXmppMucManager::invitationReceived,
           this, &KMUCManager::slotInvitationReaction);
   connect(this, &QXmppMucManager::roomAdded,
           this, &KMUCManager::slotRoomAdded);
}

void KMUCManager::setInvitationReaction(InvitationReaction reaction)
{
    invitation_reaction = reaction;
}
void KMUCManager::setAddingReaction(AddingReaction reaction)
{
    adding_reacton = reaction;
}

void KMUCManager::slotInvitationReaction(const QString &roomJid, const QString &inviter, const QString &reason)
{
    qDebug() << inviter << "invites to the room" << roomJid << "for the reason:" << reason;
    if (invitation_reaction == KMUCManager::InvitationReaction::Add)
    {
        addRoom(roomJid);
    }
}
void KMUCManager::slotRoomAdded(QXmppMucRoom *room)
{
    if (room == nullptr) return;
    if (adding_reacton == KMUCManager::AddingReaction::Join)
    {
        qDebug() << "Muc room " << room->jid() << "added, join...";
        room->join();
    }
}

QXmppMucRoom*  KMUCManager::getRoomByJid(const QString& jid)
{
    QList< QXmppMucRoom * > _rooms;
    for (QXmppMucRoom* room : _rooms)
    {
        if (room->jid() == jid) return room;
    }
    return nullptr;
}
