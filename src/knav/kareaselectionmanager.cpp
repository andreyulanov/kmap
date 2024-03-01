#include "kareaselectionmanager.h"

void KAreaSelectionManager::start()
{
  active = true;
}
void KAreaSelectionManager::finish()
{
  active = false;
}

void KAreaSelectionManager::setStartPoint(QPoint v)
{
  if (active)
  {
    start_point = v;
    setScrollingEnabled(false);
  }
}

void KAreaSelectionManager::setFinishPoint(QPoint v)
{
  if (active)
  {
    finish_point = v;
    updateMap();
  }
}

void KAreaSelectionManager::select()
{
  active = false;
  setScrollingEnabled(true);
}

void KAreaSelectionManager::paint(QPainter* p)
{
  if (!active)
    return;
  p->save();
  p->setPen(QPen(Qt::white, 5, Qt::DashLine));
  p->setBrush(Qt::NoBrush);
  auto left   = std::min(start_point.x(), finish_point.x());
  auto top    = std::min(start_point.y(), finish_point.y());
  auto right  = std::max(start_point.x(), finish_point.x());
  auto bottom = std::max(start_point.y(), finish_point.y());
  p->drawRect(QRect{QPoint{left, top}, QPoint{right, bottom}});
  p->restore();
}
