#ifndef KPOSITIONLABEL_H
#define KPOSITIONLABEL_H

#include <QLabel>
#include "kpack.h"
#include <QGeoPositionInfo>

class KPositionLabel: public QLabel
{
  Q_OBJECT

  KGeoCoor pos_info;
  double   heading = 0;

signals:
  QPoint deg2pix(const KGeoCoor& deg);

public:
  KPositionLabel(QWidget* parent);
  void update();
  void updatePosition(const KGeoCoor&);
  void updateGeoPosition(const QGeoPositionInfo&);
  void updateHeading(double);
  void setPos(const KGeoCoor&);
};

#endif  // KPOSITIONLABEL_H
