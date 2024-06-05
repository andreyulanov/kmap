#ifndef KXMPPCLIENT_H
#define KXMPPCLIENT_H

#include <QXmppQt5/QXmppClient.h>
#include <QXmppQt5/QXmppTransferManager.h>
#include <QXmppQt5/QXmppUploadRequestManager.h>
#include <QXmppQt5/QXmppHttpUploadManager.h>
#include <QXmppQt5/QXmppDiscoveryManager.h>
#include <QRegularExpression>
#include "kfreeobjectmanager.h"
#include "kmucmanager.h"

class KXmppDiscoveryManager : public QXmppDiscoveryManager
{
    Q_OBJECT
public:
    explicit KXmppDiscoveryManager();
public slots:
    void slotItemsReceived (const QXmppDiscoveryIq &);
};

class KXmppClient: public QXmppClient
{
  Q_OBJECT

public:
  KXmppClient(QString object_dir, QString proxy,
              QObject* parent = nullptr);
  ~KXmppClient();

public slots:
  void sendFile(QString jid, QString filePath,
                QString description = QString());
  void uploadAndSendFile(QString jid, QString file_path);
  void reconnectToServer(const QString& jid, const QString& password);

signals:
  void needConnection();
  void fileDownloaded(QString path);

private:
  QString objects_dir;
  QXmppTransferManager 		transfer_manager;
  KXmppDiscoveryManager 	discovery_manager;
  KMUCManager				muc_manager;
  QXmppUploadRequestManager	upload_req_manager;
  QXmppHttpUploadManager	upload_manager;
  QRegularExpression 		filesWorthToReceive = QRegularExpression(
         "[.]kpo$", QRegularExpression::CaseInsensitiveOption);

  bool notConnected();
  QString     generateReceivedFileName(QXmppTransferJob*);

private slots:
  void slotFileReceived(QXmppTransferJob* job);
  void slotConnected();
  void messageReceived(const QXmppMessage&);
};


///
/// \brief The KXmppObjectReceiver class provides transfer of a file
///	\note What for do we need this class,
/// 	there is the QXmppTransferJob which do the same job?
/// 	QXmppTransferJob does not flush the file befour emmiting the
/// 	QXmppTransferJob::finished signal.
///
class KXmppObjectReceiver: public QObject
{
  Q_OBJECT

public:
  KXmppObjectReceiver(QXmppTransferJob* job, QString filePath);
  ~KXmppObjectReceiver();

signals:
  /// Emitting of the signal guarantees that file was transferred
  /// correctly.
  void finishedSucessfully(QString path);
  /// Transmission failed, file removed.
  void failed();

private:
  QFile*            file;
  QXmppTransferJob* job;

private slots:
  void slotFinished();
  void slotProgress(qint64 done, qint64 total);
  void slotState(QXmppTransferJob::State state);
  void slotError(QXmppTransferJob::Error error);
};

#endif  // KXMPPCLIENT_H
