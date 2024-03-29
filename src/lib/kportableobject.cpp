#include "kportableobject.h"
#include "kserialize.h"
#include <math.h>
#include <QDir>
#include <QDebug>

void KPortableObject::save(QString path)
{
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly))
  {
    qDebug() << "ERROR: unable to write to" << path;
    return;
  }

  using namespace KSerialize;
  write(&f, type);
  write(&f, style);
  write(&f, (uchar)pen.red());
  write(&f, (uchar)pen.green());
  write(&f, (uchar)pen.blue());
  write(&f, (uchar)pen.alpha());
  write(&f, pen_width_mm);
  write(&f, (uchar)brush.red());
  write(&f, (uchar)brush.green());
  write(&f, (uchar)brush.blue());
  write(&f, (uchar)brush.alpha());
  write(&f, (uchar)text_color.red());
  write(&f, (uchar)text_color.green());
  write(&f, (uchar)text_color.blue());
  write(&f, (uchar)text_color.alpha());
  write(&f, text_size_mm);
  write(&f, image);
  write(&f, name);
  write(&f, polygons.count());
  for (auto polygon: polygons)
  {
    write(&f, polygon.count());
    for (auto point: polygon)
      write(&f, point);
  }
  if (type == KShape::Polygon)
  {
    write(&f, inner_polygon_idx_list.count());
    for (auto idx: inner_polygon_idx_list)
      write(&f, idx);
  }

  write(&f, text_attr);
  write(&f, file_attr);
}

void KPortableObject::load(QString path, double pixel_size_mm)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly))
  {
    qDebug() << "ERROR: unable to write to" << path;
    return;
  }

  using namespace KSerialize;
  read(&f, type);
  read(&f, style);

  uchar red, green, blue, alpha;
  read(&f, red);
  read(&f, green);
  read(&f, blue);
  read(&f, alpha);

  pen = QColor(red, green, blue, alpha);
  read(&f, pen_width_mm);

  read(&f, red);
  read(&f, green);
  read(&f, blue);
  read(&f, alpha);
  brush = QColor(red, green, blue, alpha);
  read(&f, red);
  read(&f, green);
  read(&f, blue);
  read(&f, alpha);
  text_color = QColor(red, green, blue, alpha);
  read(&f, text_size_mm);
  QImage img;
  read(&f, img);
  if (!img.isNull())
  {
    image = img.scaledToWidth(getWidthPix(pixel_size_mm),
                              Qt::SmoothTransformation);
  }
  read(&f, name);
  int n;
  read(&f, n);
  polygons.resize(n);
  for (auto& polygon: polygons)
  {
    read(&f, n);
    polygon.resize(n);
    for (auto& point: polygon)
      read(&f, point);
  }
  if (type == KShape::Polygon)
  {
    read(&f, n);
    inner_polygon_idx_list.resize(n);
    for (auto idx: inner_polygon_idx_list)
      read(&f, idx);
  }
  read(&f, text_attr);
  read(&f, file_attr);
}

bool KPortableObject::isEmpty()
{
  return polygons.isEmpty();
}

int KPortableObject::getWidthPix(double pixel_size_mm)
{
  return round(pen_width_mm / pixel_size_mm);
}

KPortableObjectManager::KPortableObjectManager(QString _objects_dir,
                                               double  _pixel_size_mm)
{
  QDir dir(_objects_dir);
  if (!dir.exists())
    dir.mkdir(_objects_dir);
  auto fi_list = dir.entryInfoList(QDir::Files, QDir::Name);
  for (auto fi: fi_list)
  {
    KPortableObject obj;
    obj.load(fi.absoluteFilePath(), pixel_size_mm);
    objects.append(obj);
  }
  objects_dir   = _objects_dir;
  pixel_size_mm = _pixel_size_mm;
}

QString KPortableObjectManager::generateObjectFileName()
{
  return objects_dir + "/" +
         QDateTime::currentDateTime().toString("yyyyMMd-hhmmss") +
         ".kpo";
}

void KPortableObjectManager::createObject(KShape sh)
{
  active_object.type         = sh.type;
  active_object.style        = sh.style;
  active_object.pen_width_mm = sh.width_mm;
  active_object.pen          = sh.pen;
  active_object.brush        = sh.brush;
  active_object.text_color   = sh.tcolor;
  active_object.image        = sh.image;
}

void KPortableObjectManager::paintObject(QPainter*       p,
                                         KPortableObject obj)
{
  if (obj.polygons.isEmpty())
    return;

  if (obj.type == KShape::Point)
  {
    auto& img = obj.image;
    auto  pix = kcoor2pix(obj.polygons.first().first());
    if (img.isNull())
      p->drawEllipse(pix, 5, 5);
    else
    {
      auto s = img.size();
      p->drawImage(
          QPoint{pix.x() - s.width() / 2, pix.y() - s.height() / 2},
          obj.image);
    }
    return;
  }

  if (obj.type == KShape::Line)
  {
    QPen pen = QPen(obj.pen, obj.getWidthPix(pixel_size_mm));
    p->setPen(pen);
    QPoint prev_pix;
    QPoint pix;
    for (int i = -1; auto point: obj.polygons.first())
    {
      i++;
      pix = kcoor2pix(point);
      if (i > 0)
        p->drawLine(prev_pix, pix);
      prev_pix = pix;
    }
  }
  if (obj.type == KShape::Polygon)
  {
    QPen   pen   = QPen(obj.pen, obj.getWidthPix(pixel_size_mm));
    QBrush brush = QBrush(obj.brush);
    p->setPen(pen);
    p->setBrush(brush);

    QPolygon polygon_pix;
    for (auto point: obj.polygons.first())
      polygon_pix.append(kcoor2pix(point));

    if (polygon_pix.count() == 2)
      p->drawPolyline(polygon_pix);
    else
      p->drawPolygon(polygon_pix);

    p->setPen(Qt::white);
    p->setBrush(Qt::black);
    for (auto point: polygon_pix)
    {
      int w = 1.0 / pixel_size_mm;
      p->drawEllipse(point, w, w);
    }
  }
}

void KPortableObjectManager::addPoint(KGeoCoor coor)
{
  if (active_object.type == KShape::None)
    return;
  auto type = active_object.type;
  if (type == KShape::Point)
  {
    active_object.name = "object1";
    KGeoPolygon poly;
    poly.append(coor);
    active_object.polygons.append(poly);
    acceptObject();
    return;
  }
  else
  {
    if (active_object.isEmpty())
    {
      active_object.name = "object1";
      KGeoPolygon polygon;
      polygon.append(coor);
      active_object.polygons.append(polygon);
    }
    else
    {
      auto& polygon = active_object.polygons[0];
      polygon.append(coor);
    }
  }
  updated();
}

void KPortableObjectManager::paint(QPainter* p)
{
  for (auto& obj: objects)
    paintObject(p, obj);
  paintObject(p, active_object);
}

void KPortableObjectManager::acceptObject()
{
  active_object.save(generateObjectFileName());
  objects.append(active_object);
  active_object.polygons.clear();
  active_object.type = KShape::None;
  updated();
  finishEdit();
}
