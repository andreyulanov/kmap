#ifndef KEDITWIDGET_H
#define KEDITWIDGET_H

#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>

class KEditWidget: public QWidget
{
  Q_OBJECT

  QVBoxLayout root_layout;
  QHBoxLayout button_layout;
  QLineEdit   edit;
  QPushButton accept_button;
  QPushButton cancel_button;

  void onAccept();

signals:
  void saveTrack(QString);

public:
  KEditWidget(QSize s);
  void show(QString title, QString default_track_name);
};

#endif  // KEDITWIDGET_H
