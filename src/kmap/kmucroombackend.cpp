#include "kmucroombackend.h"
#include <QSqlQuery>
#include <QSqlError>

KMucRoomsController::KMucRoomsController(QObject *parent)
    : QObject{parent}
{
    // nothing to do
}

void KMucRoomsController::setRoomJid(QString _room_jid)
{
    if (room_jid != _room_jid)
    {
        room_jid = _room_jid;
        roomJidChanged();
    }
    if (!isJidValid())
    {
        invalidJid();
    }
}
QString KMucRoomsController::roomJid()
{
    return room_jid;
}
bool KMucRoomsController::isJidValid()
{
    return jid_regexp.match(room_jid).hasMatch();
}
void KMucRoomsController::add()
{
    if (!isJidValid())
    {
        invalidJid();
        return;
    }
    addRoom(room_jid);
}
void KMucRoomsController::showRoom(QXmppMucRoom* room)
{
    //FIXME
    qDebug() << "Going to show MUC room" << room->jid();
    return;
}

KMucRoomsModel::KMucRoomsModel(QXmppMucManager* _manager,
                   QSqlDatabase* _database,
                   QObject *parent):
    QAbstractListModel(parent)
{
    setManager(_manager);
    setDatabase(_database);
}

int KMucRoomsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || manager == nullptr) {
        qWarning() << "KMucRoomsModel: Invalid parent of manager == nullptr";
        return 0;
    }
    return manager->rooms().size();
}

QVariant KMucRoomsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || manager == nullptr) {
        qWarning() << "KMucRoomsModel: Invalid index";
        return QVariant();
    }

    QXmppMucRoom* room = manager->rooms()[index.row()];
    if (room == nullptr)
    {
        qWarning() << "KMucRoomsModel: room = nullptr";
        return QVariant();
    }

    switch (role) {
    case NameRole:
        return room->name();
    case JidRole:
        return room->jid();
    case SubjectRole:
        return room->subject();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> KMucRoomsModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[JidRole] = "jid";
    roles[NameRole] = "name";
    roles[SubjectRole] = "subject";

    return roles;
}

void KMucRoomsModel::setManager(QXmppMucManager* _manager)
{
    if (manager != nullptr)
    {
        disconnect(manager, &QXmppMucManager::roomAdded,
                   this, &KMucRoomsModel::roomAddedSlot);
    }
    manager = _manager;
    if (manager != nullptr)
    {
        connect(manager, &QXmppMucManager::roomAdded,
                this, &KMucRoomsModel::roomAddedSlot);
    }
}
void KMucRoomsModel::setDatabase(QSqlDatabase* _database)
{
    if (_database == nullptr)
    {
        qCritical() << "KMucRoomsModel: database points to null";
        database = nullptr;
        return;
    }
    database = _database;
    if (!database->isOpen())
        database->open();
    if (database->isOpenError())
    {
        qCritical() << "KMucRoomsModel: database openned with error:"
                    << database->lastError().text();
        database = nullptr;
    }
}

void KMucRoomsModel::roomAddedSlot(QXmppMucRoom* room)
{
    /// FIXME: if two or more rooms added it works bad
    int number_of_rows = rowCount(QModelIndex());
    int row_to_add = number_of_rows == 0 ? 0 : number_of_rows - 1;
    qDebug() << "KMucRoomsModel: adding row number" << row_to_add;
    beginInsertRows(QModelIndex(), row_to_add, row_to_add);
    // saving to the database if there is one
    if (database != nullptr)
    {
        QSqlQuery query(*database);
        query.prepare("INSERT INTO " + table_name + "(jid, nickname, password)"
                      "VALUES (:jid, :nickname, :password)");
        query.bindValue(":jid", room->jid());
        query.bindValue(":nickname", room->nickName());
        query.bindValue(":password", room->password()); /// FIXME: We should store passwords in more secure way...
        if (!query.exec())
            qCritical() << "Unable to save MUC room:"
                        << query.lastError().text();
    }
    endInsertRows();
    qDebug() << "rowCount:" << rowCount(QModelIndex());
}

bool KMucRoomsModel::loadFromDatabase()
{
    if (database == nullptr) return false;
    QSqlQuery query(*database);
    query.prepare("SELECT jid, nickname, password from" + table_name);
    if (query.lastError().isValid())
    {
        qDebug() << "Failed to load rooms:" << query.lastError();
        return false;
    }
    while (query.next())
    {
        QXmppMucRoom* room = manager->addRoom(query.value(0).toString());
        if (!query.value(1).isNull()) room->setNickName(query.value(1).toString());
        if (!query.value(2).isNull()) room->setPassword(query.value(2).toString());
    }
    return true;
}
