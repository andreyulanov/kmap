#include <QPainter>
#include <math.h>
#include "kscalelabel.h"

KScaleLabel::KScaleLabel(QWidget* w)
{
  setParent(w);
  setAttribute(Qt::WA_TranslucentBackground);
  for (int i = 0; i < 7; i++)
  {
    scales.append(1.0 * pow(10, i));
    scales.append(2.0 * pow(10, i));
    scales.append(5.0 * pow(10, i));
  }
}

void KScaleLabel::update()
{
  auto   mip                  = getMip();
  double nearest_scale        = 0;
  int    nearest_scale_length = 0;
  for (auto scale: scales)
  {
    auto length_pix = scale / mip;
    if (length_pix < width())
    {
      nearest_scale        = scale;
      nearest_scale_length = length_pix;
    }
    else
      break;
  }

  QPixmap pm(size());
  pm.fill(Qt::transparent);
  QPainter p(&pm);
  p.translate(pm.width() / 2, pm.height() / 2);
  p.setPen(QPen(Qt::darkGray, 3));
  p.drawLine(-nearest_scale_length / 2, 0, nearest_scale_length / 2,
             0);
  setPixmap(pm);
}
