#include <QListWidget>
#include <QDebug>
#include "krosterwidget.h"
#include <qxmpp/QXmppRosterManager.h>
#include <qxmpp/QXmppPresence.h>

KRosterWidget::KRosterWidget(QXmppRosterManager* rosterManager,
                             QWidget *parent)
    : QWidget{parent}
{
    this->rosterManager = rosterManager;
    rootLayout 		= new QVBoxLayout(this);
    controlsLayout 	= new QHBoxLayout();
    submitButton 	= new QPushButton("Submit");
    cancelButton	= new QPushButton("Cancel");
    jidsTree 		= new QTreeWidget();
    jidsTree->setColumnCount(2);

    rootLayout->addWidget(jidsTree);
    rootLayout->addLayout(controlsLayout);
    controlsLayout->addWidget(cancelButton);
    controlsLayout->addWidget(submitButton);

    connect(rosterManager, &QXmppRosterManager::rosterReceived,
            this, &KRosterWidget::initialise);
    connect(rosterManager, &QXmppRosterManager::presenceChanged,
            this, &KRosterWidget::changePresence);
    connect(submitButton, &QPushButton::clicked,
            this, &KRosterWidget::submitButtonPushed);
    connect(cancelButton, &QPushButton::clicked,
            this, &KRosterWidget::cancelButtonPushed);
}

void KRosterWidget::initialise()
{
    auto gottenJids = rosterManager->getRosterBareJids();
    qDebug() << "Roster received:" << gottenJids;

    for(QString& bareJid : gottenJids)
        addJid(bareJid);
}
void KRosterWidget::addJid(const QString& bareJid)
{
    KRosterJid* rosterItem = new KRosterJid(bareJid,
                                rosterManager->getAllPresencesForBareJid(bareJid),
                                jidsTree);
    jidsTree->addTopLevelItem(rosterItem);
}

KRosterJid* KRosterWidget::findJid(const QString &bareJid)
{
    for (int i = 0; i < jidsTree->topLevelItemCount(); i++)
    {
        if (jidsTree->topLevelItem(i)->text(jidColumn) == bareJid)
            return (KRosterJid*) jidsTree->topLevelItem(i);
    }
    return nullptr;
}

void KRosterWidget::changePresence(const QString &bareJid, const QString &resource)
{
    KRosterJid* kRosterJid = findJid(bareJid);
    if (kRosterJid == nullptr) addJid(bareJid);
    else
    {
        KRosterResource* kRosterResource = kRosterJid->findResource(resource);
        if (kRosterResource == nullptr)
            kRosterJid->addResource(resource, rosterManager->getPresence(bareJid, resource));
        else
            kRosterResource->updatePresence(rosterManager->getPresence(bareJid, resource));
    }
}

QList<QString> KRosterWidget::selectedJids()
{
    QList<QTreeWidgetItem*> selectedItems = jidsTree->selectedItems();
    QList<QString> jids;
    for (QTreeWidgetItem* i : selectedItems)
    {
        if (i->parent() == nullptr) // selected top level item i.e Jid
            jids.append(i->text(jidColumn)); // no resource specified
        else
            jids.append(i->parent()->text(jidColumn) + "/" + i->text(resourceColumn));
    }
    return jids;
}

void KRosterWidget::submitButtonPushed()
{
    this->hide();
    emit jidSelected(selectedJids());
}

void KRosterWidget::cancelButtonPushed()
{
    this->hide();
    emit cancel();
}

void KRosterWidget::clear()
{
    jidsTree->clear();
}

KRosterJid::KRosterJid(const QString& bareJid,
                       const QMap<QString, QXmppPresence> resourcesAndPresences,
                       QTreeWidget* parrent)
    :QTreeWidgetItem{parrent}
{
    setText(jidColumn, bareJid);
    // if there are no resources, the user is offline, disable his/her jid
    if (resourcesAndPresences.isEmpty()) this->setDisabled(true);
    else // add all resources for online user
    {
        QMapIterator<QString, QXmppPresence> it(resourcesAndPresences);
        while(it.hasNext())
        {
            it.next();
            addResource(it.key(),it.value());
        }
    }
}

void KRosterJid::addResource(const QString& resource, const QXmppPresence& presence)
{
    qDebug() << "Added resource" << resource << "for" << this->text(jidColumn);
    addChild(new KRosterResource(resource, presence, this));
}

KRosterResource* KRosterJid::findResource(const QString &bareJid)
{
    for (int i = 0; i < childCount(); i++)
    {
        if (takeChild(i)->text(jidColumn) == bareJid)
            return (KRosterResource*) takeChild(i);
    }
    return nullptr;
}

KRosterResource::KRosterResource(const QString& resource,
                                 const QXmppPresence& presence,
                                 QTreeWidgetItem* parrent )
    :QTreeWidgetItem{parrent}
{
    setText(resourceColumn, resource);
    updatePresence(presence);
}

void KRosterResource::updatePresence(const QXmppPresence& presence)
{
    qDebug() << "Presence of" << parent()->text(jidColumn) + "/" + text(resourceColumn) << "updated: " << presenceText(presence);
    setText(statusColumn, presenceText(presence));
    switch (presence.type()) {
        case QXmppPresence::Type::Available : parent()->setDisabled(false); setDisabled(false); break;
        case QXmppPresence::Type::Unavailable : setDisabled(true); break;
        // TODO: disable jid if all its resources disabled
        default: break; //Do nothng
    }
}

QString KRosterResource::presenceText(const QXmppPresence& presence)
{
    QXmppPresence::Type type = presence.type();
    if (type == QXmppPresence::Unavailable)
        return "Unavailable";
    if(type == QXmppPresence::Available)
    {
        switch (presence.availableStatusType()) {
            case QXmppPresence::AvailableStatusType::Online		: return "Online";
            case QXmppPresence::AvailableStatusType::Away		: return "Away";
            case QXmppPresence::AvailableStatusType::XA 		: return "XA";
            case QXmppPresence::AvailableStatusType::DND		: return "Do not Disturb";
            case QXmppPresence::AvailableStatusType::Chat 		: return "Chat";
            case QXmppPresence::AvailableStatusType::Invisible 	: return "Invisible";
        }
    }
    return "Error: Unknown status";
}
