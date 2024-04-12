#ifndef KSCALELABEL_H
#define KSCALELABEL_H

#include <QLabel>

class KScaleLabel: public QLabel
{
  Q_OBJECT

  QList<double> scales;

signals:
  double getMip();

public:
  KScaleLabel(QWidget* parent);
  void update();
};

#endif  // KSCALELABEL_H
