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

    transferManager = new QXmppTransferManager;
    transferManager->setProxy(proxy);
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

void KXmppClient::sendFile(const QString &jid,
                           const QString &filePath,
                           const QString &description)
{
    qDebug() << "Sending " << filePath << " to " << jid << " with description " << description;
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
    :QObject{}, filePath{filePath}
{
    connect(job, SIGNAL(error(QXmppTransferJob::Error)),
            this, SLOT(slotError(QXmppTransferJob::Error)));
    connect(job, SIGNAL(finished()),
            this, SLOT(slotFinished()));
    connect(job, SIGNAL(progress(qinit64 done, qint64 total)),
            this, SLOT(slotProgress(qinit64 done, qint64 total)));
    connect(job, SIGNAL(stateChanged()),
            this, SLOT(slotState()));

    qDebug() << "Transfer method" << job->method();
    job->accept(filePath);
}
KXmppObjectReceiver::~KXmppObjectReceiver()
{
    //qDebug() << "KXmppObjectReceiver ends its life...";
}

void KXmppObjectReceiver::slotError(QXmppTransferJob::Error error)
{
    ///TODO: delete the file
    qWarning() << "Transmission failed:" << error;
}

void KXmppClient::slotFileReceived(QXmppTransferJob *job)
{
    qInfo() << "Got transfer request from:" << job->jid();

    // do not download file in case if it does not worth it
    if (!filesWorthToReceive.match(job->fileName()).hasMatch())
    {
        qDebug() << "File name" << job->fileName() <<
                    "does not match filesWorthToReceive. Aborting...";
        job->abort();
        return;
    }

    ///TODO: Append receivers to a list
    ///TODO: Delete receivers by timeout
    KXmppObjectReceiver* receiver = new KXmppObjectReceiver(job, generateReceivedFileName(job));

    connect(receiver, SIGNAL(finishedSucessfully(KPortableObject *)),
            this, SLOT(KXmppClient::slotFileReceived(KPortableObject*)));
}

void KXmppObjectReceiver::slotFinished()
{
    qDebug() << "Transmission finished! Object saved to" << this->filePath;
    KPortableObject* portableObject = nullptr;
    //KPortableObject* portableObject = new KPortableObject();
    //portableObject->load(this->filePath);
    ///TODO: Check have the object been loadded correctly
    emit finishedSucessfully(portableObject);
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
