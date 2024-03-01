#ifndef KCHANNELMANAGER_H
#define KCHANNELMANAGER_H

#include <QUuid>
#include "kvisualobject.h"

struct KChannel
{
  enum Direction
  {
    In,
    Out
  } dir;
  bool                 enabled = false;
  QUuid                id;
  QVector<KPackObject> objects;
  KChannel(QUuid _id, Direction _dir);
};

class KChannelManager: public QObject
{
  Q_OBJECT

  QStringList       user_name_list;
  QVector<QUuid>    user_id_list;
  QVector<KChannel> channels;

public:
  KChannelManager();
  QVector<KChannel> getChannels();
  QUuid             addUser(QString name);
  void              addChannel(QUuid id, KChannel::Direction);
  QString           getUserNameById(QUuid);
};

#endif  // KCHANNELMANAGER_H
