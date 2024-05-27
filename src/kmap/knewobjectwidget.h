#ifndef KNEWOBJECTWIDGET_H
#define KNEWOBJECTWIDGET_H

#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include "kclass.h"

class KNewObjectWidget: public QWidget
{
  Q_OBJECT

  QVBoxLayout                 root_layout;
  QGroupBox                   options_group_box;
  QMap<QPushButton*, QString> button_id_map;
  QCheckBox*                  send_check_box;
  void addItem(int& posy, QImage icon, QString text);
  void onSelected();

signals:
  KShapeImageList getUserShapeImageList();
  KClass          getShapeById(QString);
  void            selectedShape(KClass);
  void            sendObject();
  void            doNotSendObject();

public:
  KNewObjectWidget();
  void show();
};

#endif  // KNEWOBJECTWIDGET_H
