#include "kxmppclient.h"
#include <QXmppQt5/QXmppMessage.h>
#include <QXmppQt5/QXmppTransferManager.h>
#include <QXmppQt5/QXmppGlobal.h>
#include <QXmppQt5/QXmppDiscoveryManager.h>
#include <QXmppQt5/QXmppDiscoveryIq.h>
#include <QFile>

KXmppClient::KXmppClient(QString objects_dir, QString proxy, QObject *parent)
    : QXmppClient{parent}
{
    connect(this, &QXmppClient::messageReceived,
            this, &KXmppClient::messageReceived);

    this->objects_dir = objects_dir;
    configuration().setResource("knav");

    transfer_manager.setProxy(proxy);
    //transfer_manager.proxyOnly();
    //transfer_Manager.setSupportedMethods(QXmppTransferJob::Method::InBandMethod);
    addExtension(&transfer_manager);
    QXmppDiscoveryManager* d_manager = findExtension<QXmppDiscoveryManager>();
    if (d_manager)
        removeExtension(d_manager);
    addExtension(&discovery_manager);
    addExtension(&upload_req_manager);
    addExtension(&upload_manager);

    connect(&transfer_manager, &QXmppTransferManager::fileReceived,
            this, &KXmppClient::slotFileReceived);
    connect(this, &QXmppClient::connected,
            this, &KXmppClient::slotConnected);
}

KXmppClient::~KXmppClient()
{
    //nothing to do...
}

void KXmppClient::messageReceived(const QXmppMessage &message)
{
    QString from = message.from();
    sendPacket(QXmppMessage("", from, "The functionality of text messages has not yet been implemented"));
}

void KXmppClient::sendFile( QString jid,
                            QString filePath,
                            QString description)
{
    qDebug() << "Sending" << filePath << "to" << jid << "with description" << description;
    if (notConnected()) return;
    transfer_manager.sendFile(jid, filePath, description);
    //uploadAndSendFile(jid, filePath);
}

void KXmppClient::uploadAndSendFile(QString jid, QString file_path)
{
    qDebug() << "Uploading file" << file_path << "to send to" << jid;
    if (notConnected()) return;
    QFileInfo file_info(file_path);
    std::shared_ptr< QXmppHttpUpload > upload = upload_manager.uploadFile(file_info);
    std::weak_ptr<QXmppHttpUpload> upload_ptr = upload;
    // this connections cry for refactoring
    connect(upload.get(), &QXmppHttpUpload::progressChanged, this, [upload_ptr]() {
        auto upload = upload_ptr.lock();
        if (upload != nullptr)
        {
            qDebug() << "Upload progress:" << upload->bytesSent() << "/" << upload->bytesTotal() << "bytes ("
                     << upload->progress() * 100 << "% )";
        }
    });
    connect(upload.get(), &QXmppHttpUpload::finished, this, [this, upload_ptr, jid](const QXmppHttpUpload::Result & result) {
        auto upload = upload_ptr.lock();
        if (upload != nullptr)
        {
            try
            {
                qDebug() << "Send to " << jid << "the link" << std::get<QUrl>(result).toString();
                sendMessage(jid, std::get<QUrl>(result).toString());
            }
            catch (const std::bad_variant_access& ex)
            {
                qWarning() << "Failed to upload file";
            }
        }
    });
}

bool KXmppClient::notConnected()
{
    bool not_connected = !isConnected();
    if (not_connected)
    {
        qWarning() << "Attempt do something with disconnected client, aborting...";
        emit needConnection();
    }
    return not_connected;
}

void KXmppClient::reconnectToServer(const QString& jid, const QString& password)
{
    if (isConnected())
    {
        disconnectFromServer();
        auto conn = std::make_shared<QMetaObject::Connection>();
        *conn = connect(this, &QXmppClient::disconnected,
                [this, jid, password, conn]()
        {
            QObject::disconnect(*conn);
            this->connectToServer(jid, password);
        });
    }
    else
        connectToServer(jid, password);
}

QString KXmppClient::generateReceivedFileName(QXmppTransferJob *job)
{
  QString receivedFilename = job->fileName();
  return objects_dir + "/" +
         receivedFilename.replace(".kpo","-") +
         QDateTime::currentDateTime().toString("yyyyMMd-hhmmss") +
         ".kpo";
}

KXmppObjectReceiver::KXmppObjectReceiver(QXmppTransferJob *job, QString filePath)
    :QObject{}
{
    connect(job, qOverload<QXmppTransferJob::Error>(&QXmppTransferJob::error),
            this, &KXmppObjectReceiver::slotError);
    connect(job, &QXmppTransferJob::finished,
            this, &KXmppObjectReceiver::slotFinished);
    connect(job, &QXmppTransferJob::finished,
            this, &QXmppTransferJob::deleteLater);
    connect(job, &QXmppTransferJob::progress,
            this, &KXmppObjectReceiver::slotProgress);
    connect(job, &QXmppTransferJob::stateChanged,
            this, &KXmppObjectReceiver::slotState);

    qDebug() << "Transfer method" << job->method();
    file = new QFile(filePath, this);
    if (file->open(QIODevice::WriteOnly))
        job->accept(file);
    else
    {
        qWarning() << "Failed to open file" << filePath << "\n"
                 << file->errorString() << "\n"
                 << "Aborting transmission...";
        emit failed();
        job->abort();
        this->deleteLater();
    }
}
KXmppObjectReceiver::~KXmppObjectReceiver()
{
    if (file->isOpen())
        file->close();
}

void KXmppObjectReceiver::slotError(QXmppTransferJob::Error error)
{
    qWarning() << "Receiving failed:" << error;
    file->remove();
    emit failed();
}

void KXmppClient::slotFileReceived(QXmppTransferJob *job)
{
    qInfo() << "Got transfer request from:" << job->jid();

    // do not download file in case if it does not worth it
    if (!filesWorthToReceive.match(job->fileName()).hasMatch())
    {
        qDebug() << "File name" << job->fileName() <<
                    "does not match filesWorthToReceive regex. Aborting...";
        job->abort();
        return;
    }

    ///TODO: Append receivers to a list
    ///TODO: Delete receivers by timeout
    KXmppObjectReceiver* receiver = new KXmppObjectReceiver(job, generateReceivedFileName(job));

    connect(receiver, &KXmppObjectReceiver::finishedSucessfully,
            this, &KXmppClient::fileDownloaded);
}

void KXmppClient::slotConnected()
{
    QString domain = configuration().domain();
    QString items = discovery_manager.requestItems(domain);
    qDebug() << "items:" << items;
}

KXmppDiscoveryManager::KXmppDiscoveryManager():
    QXmppDiscoveryManager{}
{

    connect(this, &QXmppDiscoveryManager::itemsReceived,
            this, &KXmppDiscoveryManager::slotItemsReceived);
}

void KXmppDiscoveryManager::slotItemsReceived(const QXmppDiscoveryIq& iq)
{
    for (auto item : iq.items())
    {
        qDebug() << "request info of" << item.jid();
        requestInfo(item.jid());
    }
}

void KXmppObjectReceiver::slotFinished()
{
    qDebug() << "Transmission finished! Object saved to" << file->fileName();
    ///TODO: Check have the object been loadded correctly
    if (file->flush())
        emit finishedSucessfully(file->fileName());
    else
        qWarning() << "Filed to flush file" << file->fileName() << "\n"
                   << file->errorString();
    this->deleteLater();
}

void KXmppObjectReceiver::slotProgress(qint64 done, qint64 total)
{
    qDebug() << "Transmission progress:" << done << "/" << total;
}

void KXmppObjectReceiver::slotState(QXmppTransferJob::State state)
{
    qDebug() << "Transmission state:" << state;
}
