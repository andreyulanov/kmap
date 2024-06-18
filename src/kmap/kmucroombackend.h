#ifndef KMUCROOMBACKEND_H
#define KMUCROOMBACKEND_H

#include <QObject>
#include <QString>
#include <QAbstractListModel>
#include <QtQml/qqml.h>
#include <QXmppQt5/QXmppMucManager.h>
#include <QRegularExpression>
#include <QtSql/QSqlDatabase>

/// \brief Contorller of MUC rooms.
class KMucRoomsController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString room_jid READ roomJid WRITE setRoomJid NOTIFY roomJidChanged)

public:
    explicit KMucRoomsController(QObject *parent = nullptr);
    QString roomJid();
    void setRoomJid(QString _room_jid);
    /// \brief Add a new room.
    /// Do not use this function if a room with _room_jid is already in QXmppMucManager::rooms()
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

    /// \brief Constructs a new KMucRoomsModel.
    ///
    /// _manager and _databse must be ready for work
    KMucRoomsModel(QXmppMucManager* _manager,
                   QSqlDatabase* _database = nullptr,
                   QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

    void setManager(QXmppMucManager* _manager);
    /// Sets the databse.
    ///
    ///	Database mast be ready for openning.
    /// If database == nullptr or database can not be opened
    ///  couthen the model will not store data in a database.
    void setDatabase(QSqlDatabase*);
    /// Loads chats from database and adds them to the manager.
    bool loadFromDatabase();
    /// Create table of chats in the database.
    bool createTable();
private:
    QXmppMucManager* manager = nullptr;
    /// if database == nullptr then the model will not store data in a database
    QSqlDatabase* database = nullptr;
    const QString table_name = "MUC_rooms";
private slots:
    /// Adds room to the database
    void roomAddedSlot(QXmppMucRoom*);
};


#endif // KMUCROOMBACKEND_H
