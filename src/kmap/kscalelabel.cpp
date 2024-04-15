#include <QPainter>
#include <math.h>
#include "kscalelabel.h"

KScaleLabel::KScaleLabel(QWidget* w)
{
  setParent(w);
  setAttribute(Qt::WA_TranslucentBackground);
  for (int i = 0; i < 7; i++)
  {
    scales.append(pow(1.0, i));
    scales.append(pow(2.0, i));
    scales.append(pow(5.0, i));
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
  setPixmap(pm);
}
