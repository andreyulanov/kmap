#ifndef KNEWOBJECTWIDGET_H
#define KNEWOBJECTWIDGET_H

#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "kshape.h"

class KNewObjectWidget: public QWidget
{
  Q_OBJECT

  QVBoxLayout                 root_layout;
  QMap<QPushButton*, QString> button_id_map;
  void addItem(int& posy, QImage icon, QString text);
  void onSelected();

signals:
  KShapeList getUserShapeList();
  KShape     getShapeById(QString);
  void       selectedShape(KShape);

public:
  KNewObjectWidget(QSize s);
  void show();
};

#endif  // KNEWOBJECTWIDGET_H
