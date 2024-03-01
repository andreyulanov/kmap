#ifndef KAUTOSCROLL_H
#define KAUTOSCROLL_H

#include <QList>
#include <QPointF>
#include <QTimer>
#include <QElapsedTimer>

class KAutoScroll: public QObject
{
  Q_OBJECT

  QList<QPointF> pos_list;
  QTimer         timer;
  QPointF        speed;
  QElapsedTimer  mouse_pos_timer;

  void onTimer();

signals:
  void scroll(QPoint);

public:
  KAutoScroll();
  void    stop();
  QPointF getSpeed();
  void    accumulate(QPoint);
  void    start();
};

#endif  // KAUTOSCROLL_H
