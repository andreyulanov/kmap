#include "kxmppclient.h"
#include <qxmpp/QXmppMessage.h>
#include <qxmpp/QXmppTransferManager.h>
#include <QFile>

KXmppClient::KXmppClient(QString objects_dir, QString proxy, QObject *parent)
    : QXmppClient{parent}
{
    connect(this, &QXmppClient::messageReceived,
            this, &KXmppClient::messageReceived);

    this->objects_dir = objects_dir;
    configuration().setResource("knav");

    transferManager = new QXmppTransferManager;
    transferManager->setProxy(proxy);
    //transferManager->proxyOnly();
    //transferManager->setSupportedMethods(QXmppTransferJob::Method::InBandMethod);
    addExtension(transferManager);

    connect(transferManager, &QXmppTransferManager::fileReceived,
            this, &KXmppClient::slotFileReceived);
}

KXmppClient::~KXmppClient()
{
    delete transferManager;
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
    if (!isConnected())
    {
        qWarning() << "Attempt to send a file by disconnected client, aborting...";
        emit needConnection();
        return;
    }
    transferManager->sendFile(jid, filePath, description);
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
