#ifndef KROSTERWIDGET_H
#define KROSTERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QAbstractItemView>
#include <QFlags>
#include <QPushButton>
#include <qxmpp/QXmppRosterManager.h>

const int jidColumn = 0;
const int resourceColumn = 0;
const int statusColumn = 1;

class KRosterJid;
class KRosterResource;

class KRosterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KRosterWidget(QXmppRosterManager* rosterManager,
                           QWidget *parent = nullptr);
public slots:
    void initialise();
    void changePresence(const QString &bareJid, const QString &resource);
    void submitButtonPushed();
    void cancelButtonPushed();
    void clear();

private:
    void addJid(const QString& bareJid);
    KRosterJid* findJid(const QString& bareJid);


signals:
    void jidSelected(QList<QString> Jids);
    void cancel();

protected:
    QVBoxLayout* rootLayout;
    QHBoxLayout* controlsLayout;
    QTreeWidget* jidsTree;
    QXmppRosterManager* rosterManager;
    QPushButton* submitButton;
    QPushButton* cancelButton;
    QList<QString> selectedJids();
};

class KRosterJid : public QTreeWidgetItem
{
public:
    explicit KRosterJid(const QString& bareJid,
                        const QMap<QString, QXmppPresence> instancesAndPresences,
                        QTreeWidget* parrent = nullptr);
    void addResource(const QString&, const QXmppPresence&);
    KRosterResource* findResource(const QString& bareJid);
};

class KRosterResource : public QTreeWidgetItem
{
public:
    explicit KRosterResource(const QString& resource,
                             const QXmppPresence& presence,
                             QTreeWidgetItem* parrent = nullptr);
    void updatePresence(const QXmppPresence&);
private:
    QString presenceText(const QXmppPresence&);
};

#endif // KROSTERWIDGET_H
