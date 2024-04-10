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
    void fileDownloaded(QString path);

private:
    QXmppTransferManager *transferManager;
    QRegularExpression  filesWorthToReceive =
            QRegularExpression("[.]kpo$", QRegularExpression::CaseInsensitiveOption);

    Q_SLOT void slotFileReceived(QXmppTransferJob *job);
    QString generateReceivedFileName(QXmppTransferJob*);
};

///
/// \brief The KXmppObjectReceiver class provides transfer of a file
///	\note What for do we need this class,
/// 	there is the QXmppTransferJob which do the same job?
/// 	QXmppTransferJob does not flush the file befour emmiting the
/// 	QXmppTransferJob::finished signal.
///
class KXmppObjectReceiver : public QObject
{
    Q_OBJECT

public:
    KXmppObjectReceiver(QXmppTransferJob *job, QString filePath);
    ~KXmppObjectReceiver();

signals:
    /// Emitting of the signal guarantees that file was transferred correctly.
    void finishedSucessfully(QString path);
    /// Transmission failed, file removed.
    void failed();

private:
    QFile* file;
    QXmppTransferJob *job;

    Q_SLOT void slotFinished();
    Q_SLOT void slotProgress(qint64 done, qint64 total);
    Q_SLOT void slotState(QXmppTransferJob::State state);
    Q_SLOT void slotError(QXmppTransferJob::Error error);
};

#endif // KXMPPCLIENT_H
