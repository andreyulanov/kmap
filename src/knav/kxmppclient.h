#ifndef KXMPPCLIENT_H
#define KXMPPCLIENT_H

#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppTransferManager.h>
#include <QRegularExpression>
#include "kportableobject.h"

class KXmppClient : public QXmppClient
{
    Q_OBJECT

public:
    KXmppClient(QString object_dir, QString proxy, QObject *parent = nullptr);
    ~KXmppClient();

    QString objects_dir;

    void messageReceived(const QXmppMessage &);

public slots:
    void sendFile(QString jid,
                  QString filePath,
                  QString description = QString());
    void reconnectToServer(const QString& jid, const QString& password);

signals:
    void needConnection();

private:
    QXmppTransferManager *transferManager;
    QRegularExpression  filesWorthToReceive =
            QRegularExpression("[.]kpo$", QRegularExpression::CaseInsensitiveOption);

    Q_SLOT void slotFileReceived(QXmppTransferJob *job);
    QString generateReceivedFileName(QXmppTransferJob*);
};

class KXmppObjectReceiver : public QObject
{
    Q_OBJECT

public:
    KXmppObjectReceiver(QXmppTransferJob *job, QString filePath);
    ~KXmppObjectReceiver();

    Q_SIGNAL void finishedSucessfully(KPortableObject *);

private:
    QString filePath;
    QXmppTransferJob *job;

    Q_SLOT void slotFinished();
    Q_SLOT void slotProgress(qint64 done, qint64 total);
    Q_SLOT void slotState(QXmppTransferJob::State state);
    Q_SLOT void slotError(QXmppTransferJob::Error error);
};

#endif // KXMPPCLIENT_H
