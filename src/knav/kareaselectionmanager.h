#ifndef KAREASELECTIONMANAGER_H
#define KAREASELECTIONMANAGER_H

#include <QPainter>

class KAreaSelectionManager: public QObject
{
  Q_OBJECT

  bool   active = false;
  QPoint start_point;
  QPoint finish_point;

signals:
  void setScrollingEnabled(bool);
  void updateMap();

public:
  void start();
  void finish();
  void setStartPoint(QPoint);
  void setFinishPoint(QPoint);
  void select();
  void paint(QPainter*);
};

#endif  // KAREASELECTIONMANAGER_H
