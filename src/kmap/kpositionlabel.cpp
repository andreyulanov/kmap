#include <QPainter>
#include <QDebug>
#include "kpositionlabel.h"

KPositionLabel::KPositionLabel(QWidget* w)
{
  setParent(w);
  setFixedSize(100, 100);
  setAttribute(Qt::WA_TranslucentBackground);
}

void KPositionLabel::setPos(const KGeoCoor& v)
{
  pos_info = v;
  update();
}

void KPositionLabel::updatePosition(const KGeoCoor& v)
{
  pos_info = v;
  update();
}

void KPositionLabel::updateGeoPosition(const QGeoPositionInfo& v)
{
  updatePosition(KGeoCoor::fromDegs(v.coordinate().latitude(),
                                    v.coordinate().longitude()));
}

void KPositionLabel::updateHeading(double v)
{
  heading = v;
  update();
}

void KPositionLabel::update()
{
  double   radius = 40;
  QPolygon pos_poly;
  pos_poly += QPoint(0, -radius);
  pos_poly += QPoint(radius * 0.55, radius * 0.45);
  pos_poly += QPoint(0, radius * 0.2);
  pos_poly += QPoint(-radius * 0.55, radius * 0.45);

  QPixmap pm(size());
  pm.fill(Qt::transparent);
  QPainter p(&pm);
  p.translate(pm.width() / 2, pm.height() / 2);
  p.rotate(heading);
  p.setPen(QPen(Qt::black, 5));
  p.setBrush(Qt::white);
  p.drawConvexPolygon(pos_poly);
  setPixmap(pm);
  auto pos_pix = deg2pix(pos_info);
  move(pos_pix - QPoint{width() / 2, height() / 2});
}
