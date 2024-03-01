#include "kheading.h"
#include "math.h"
#include <QDebug>

KHeadingProvider::KHeadingProvider(const QMagnetometer* v)
{
  magnetometer = v;
}

void KHeadingProvider::update()
{
  auto v     = magnetometer->reading();
  auto angle = -atan2(v->x(), v->y()) / M_PI * 180;
  headingChanged(angle);
}
