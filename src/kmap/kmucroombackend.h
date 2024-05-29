#ifndef KMUCROOMBACKEND_H
#define KMUCROOMBACKEND_H

#include <QObject>
#include <QString>
#include <QAbstractListModel>
#include <QtQml/qqml.h>
#include <QXmppQt5/QXmppMucManager.h>
#include <QRegularExpression>

class KMucRoomsController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString room_jid READ roomJid WRITE setRoomJid NOTIFY roomJidChanged)

public:
    explicit KMucRoomsController(QObject *parent = nullptr);
    QString roomJid();
    void setRoomJid(QString _room_jid);
    Q_INVOKABLE void add();
public slots:
    void showRoom(QXmppMucRoom*);
signals:
    void roomJidChanged();
    /// User entered incorrect Jid
    void invalidJid();
    /// User adds a MUC room
    void addRoom(QString room_jid);
private:
    QString room_jid;
    bool isJidValid();
    inline static const QRegularExpression jid_regexp{"[^@/<>'\"]*@[^@/<>'\"]*(/[^@/<>'\"])*"}; // FIXME
    // this regex far from the perfect one.
    // For more see https://datatracker.ietf.org/doc/html/rfc6122#section-2
};


/// \brief Model that contains chat rooms.
///
/// TODO: update rooms if their subject or name changed.
class KMucRoomsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        JidRole = Qt::UserRole + 1,
        NameRole,
        SubjectRole
    };

    KMucRoomsModel(QXmppMucManager* _manager, QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void setManager(QXmppMucManager* _manager);
private:
    QXmppMucManager* manager = nullptr;
private slots:
    void roomAddedSlot(QXmppMucRoom*);
};


#endif // KMUCROOMBACKEND_H
