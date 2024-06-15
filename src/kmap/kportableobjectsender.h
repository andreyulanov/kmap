#ifndef KPORTABLEOBJECTSENDER_H
#define KPORTABLEOBJECTSENDER_H

#include <QObject>
#ifdef BUILD_WITH_XMPP
  #include <kxmppclient.h>
#endif
class KPortableObjectSender: public QObject
{
  Q_OBJECT
public:
  explicit KPortableObjectSender(QObject* parent = nullptr);
  ~KPortableObjectSender();

public slots:
  void turnOnSendOnReady();
  void turnOffSendOnReady();
  void setJid(QList<QString> jids);
  void setFilename(QString);
  void clear();
  /// Sends file if jid and file_path have been set.
  /// @return Returns true if file is ready to send.
  bool sendIfReady();

signals:
  void send(QString jid, QString filePath, QString description);

private:
  bool     send_if_ready = false;
  QString* jid           = nullptr;
  QString* file_path     = nullptr;

  bool isReady();
};

#endif  // KPORTABLEOBJECTSENDER_H
