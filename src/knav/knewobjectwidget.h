#ifndef KNEWOBJECTWIDGET_H
#define KNEWOBJECTWIDGET_H

#include <QListWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QCheckBox>
#include "kshape.h"

class KNewObjectWidget: public QWidget
{
  Q_OBJECT

  QVBoxLayout                 root_layout;
  QGroupBox					  options_group_box;
  QMap<QPushButton*, QString> button_id_map;
  QCheckBox* send_check_box;
  void addItem(int& posy, QImage icon, QString text);
  void onSelected();

signals:
  KShapeList getUserShapeList();
  KShape     getShapeById(QString);
  void       selectedShape(KShape);
  void		 sendObject();
  void		 doNotSendObject();

public:
  KNewObjectWidget(QSize s);
  void show();
};

#endif  // KNEWOBJECTWIDGET_H
