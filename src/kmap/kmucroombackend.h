#ifndef KMUCROOMBACKEND_H
#define KMUCROOMBACKEND_H

#include <QObject>
#include <QString>
#include <QtQml/qqml.h>
#include <QRegularExpression>

class KMucRoomBackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString room_jid READ roomJid WRITE setRoomJid NOTIFY roomJidChanged)
    QML_ELEMENT

public:
    explicit KMucRoomBackEnd(QObject *parent = nullptr);
    QString roomJid();
    void setRoomJid(QString _room_jid);
signals:
    void roomJidChanged();
private:
    QString room_jid;
    inline static const QRegularExpression jid_regexp{"[^@/<>'\"]*@[^@/<>'\"]*(/[^@/<>'\"])*"}; // FIXME
    // this regex far from perfect for more
    // see https://datatracker.ietf.org/doc/html/rfc6122#section-2
};


#endif // KMUCROOMBACKEND_H
