#ifndef KMUCMANAGER_H
#define KMUCMANAGER_H
#include <QXmppQt5/QXmppMucManager.h>

class KMUCManager : public QXmppMucManager
{
public:
    enum InvitationReaction {Add = 0x0001,// AddFromRoster = 0x0002, // add room if the inviter presents in roster
                             IgnoreInvitaion = 0x0000};
    enum AddingReaction {Join = 0x0001, IgnoreAdding = 0x0000};
    KMUCManager(InvitationReaction _invitation_reaction = IgnoreInvitaion,
                AddingReaction _adding_reaction = IgnoreAdding);
    void setInvitationReaction(InvitationReaction);
    void setAddingReaction(AddingReaction);
private:
    InvitationReaction invitation_reaction;
    AddingReaction adding_reacton;
private slots:
    void slotInvitationReaction(const QString &roomJid, const QString &inviter, const QString &reason);
    void slotRoomAdded(QXmppMucRoom *room);
};

#endif // KMUCMANAGER_H
