#ifndef KNEWOBJECTWIDGET_H
#define KNEWOBJECTWIDGET_H

#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include "kshape.h"

class KNewObjectWidget: public QWidget
{
  Q_OBJECT

  QVBoxLayout                 root_layout;
  QMap<QPushButton*, QString> button_id_map;
  QCheckBox* send_chack_box;
  void addItem(int& posy, QImage icon, QString text);
  void onSelected();

signals:
  KShapeList getUserShapeList();
  KShape     getShapeById(QString);
  void       selectedShape(KShape);
  void		 sendObject();

public:
  KNewObjectWidget(QSize s);
  void show();
};

#endif  // KNEWOBJECTWIDGET_H
