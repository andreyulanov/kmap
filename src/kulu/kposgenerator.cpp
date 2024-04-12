#include "kposgenerator.h"

KPosGenerator::KPosGenerator(KGeoCoor start_coor, KGeoCoor step_coor)
{
  connect(&timer, &QTimer::timeout, this, &KPosGenerator::onTimer);
  coor     = start_coor;
  step     = step_coor;
  altitude = 0;
  timer.start(1000);
}

void KPosGenerator::onTimer()
{
  coor = coor.inc(step);
  altitude += 1;
  generated_pos(
      {coor, altitude, KDateTime(QDateTime::currentDateTime())});
  generated_coor(coor);
  generated_angle(30);
}
