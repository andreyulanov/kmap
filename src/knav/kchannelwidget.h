#ifndef KCHANNELWIDGET_H
#define KCHANNELWIDGET_H

#include <QVBoxLayout>
#include <QWidget>
#include "kchannelmanager.h"

class KChannelWidget: public QWidget
{
  Q_OBJECT

  QVBoxLayout root_layout;
  QHBoxLayout item_layout;

  void addItem(int& posy, QString, bool enabled);
  void show(KChannel::Direction dir);

signals:
  QVector<KChannel> getChannels();
  QString           getUserNameById(QUuid);

public:
  KChannelWidget(QSize s);
  void showIn();
  void showOut();
};

#endif  // KCHANNELWIDGET_H
