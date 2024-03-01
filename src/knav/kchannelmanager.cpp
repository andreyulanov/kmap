#include "kchannelmanager.h"

QUuid KChannelManager::addUser(QString name)
{
  user_name_list.append(name);
  auto id = QUuid::createUuid();
  user_id_list.append(id);
  return id;
}

KChannel::KChannel(QUuid _id, Direction _dir)
{
  id  = _id;
  dir = _dir;
}
KChannelManager::KChannelManager()
{
}

QVector<KChannel> KChannelManager::getChannels()
{
  return channels;
}

void KChannelManager::addChannel(QUuid id, KChannel::Direction dir)
{
  channels.append(KChannel(id, dir));
}

QString KChannelManager::getUserNameById(QUuid id)
{
  for (int i = -1; auto user_id: user_id_list)
  {
    i++;
    if (user_id == id)
      return user_name_list.at(i);
  }
  return QString();
}
