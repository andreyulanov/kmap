#ifndef KPOSGENERATOR_H
#define KPOSGENERATOR_H

#include <QTimer>
#include "kmap.h"

class KPosGenerator: public QObject
{
  Q_OBJECT

  QTimer   timer;
  KGeoCoor coor;
  float    altitude;
  KGeoCoor step;
  void     onTimer();

signals:
  void generated_pos(KPosition);
  void generated_coor(KGeoCoor);
  void generated_angle(double deg);

public:
  KPosGenerator(KGeoCoor start_coor, KGeoCoor step_coor);
};

#endif  // KPOSGENERATOR_H
