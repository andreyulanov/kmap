#ifndef KHEADING_H
#define KHEADING_H
#include <QMagnetometer>

class KHeadingProvider: public QObject
{
  Q_OBJECT

  const QMagnetometer* magnetometer;

signals:
  void headingChanged(double);

public:
  KHeadingProvider(const QMagnetometer*);
  void update();
};

#endif  // KHEADING_H
