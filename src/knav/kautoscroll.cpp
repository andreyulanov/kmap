#include "kautoscroll.h"

KAutoScroll::KAutoScroll()
{
  timer.setInterval(10);
  connect(&timer, &QTimer::timeout, this, &KAutoScroll::onTimer);
}

void KAutoScroll::stop()
{
  timer.stop();
  pos_list.clear();
}

QPointF KAutoScroll::getSpeed()
{
  return speed;
}

void KAutoScroll::accumulate(QPoint pos)
{
  if (pos_list.count() == 5)
    pos_list.removeFirst();
  pos_list.append(pos);
  mouse_pos_timer.start();
}

void KAutoScroll::start()
{
  if (pos_list.count() >= 5 && mouse_pos_timer.elapsed() < 100)
  {
    speed = (pos_list.first() - pos_list.last()) / pos_list.count();
    timer.start();
  }
  else
  {
    speed = QPointF();
    pos_list.clear();
  }
}

void KAutoScroll::onTimer()
{
  speed *= 0.94;
  if (speed.toPoint().manhattanLength() < 2)
    timer.stop();
  else
    scroll(speed.toPoint());
}
