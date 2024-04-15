#include "kobject.h"
#include "kserialize.h"
#include <math.h>
#include <QDir>
#include <QDebug>

void KObject::save(QString path)
{
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly))
  {
    qDebug() << "ERROR: unable to write to" << path;
    return;
  }

  using namespace KSerialize;

  write(&f, cl.name);
  write(&f, cl.type);
  write(&f, cl.pen);
  write(&f, cl.brush);
  write(&f, cl.image);

  write(&f, name);
  write(&f, polygons.count());
  for (auto polygon: polygons)
  {
    write(&f, polygon.count());
    for (auto point: polygon)
      write(&f, point);
  }
  write(&f, text_attr);
  write(&f, data_attr);
}

void KObject::load(QString path, double pixel_size_mm)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly))
  {
    qDebug() << "ERROR: unable to write to" << path;
    return;
  }

  using namespace KSerialize;

  read(&f, cl.name);
  read(&f, cl.type);
  read(&f, cl.pen);
  read(&f, cl.brush);
  QImage img;
  read(&f, img);
  if (!img.isNull())
    cl.image = img.scaledToWidth(getWidthPix(pixel_size_mm),
                                 Qt::SmoothTransformation);
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
  read(&f, text_attr);
  read(&f, data_attr);
}

bool KObject::isEmpty()
{
  return polygons.isEmpty();
}

int KObject::getWidthPix(double pixel_size_mm)
{
  return round(KObjectClass::default_image_size_mm / pixel_size_mm);
}

KObjectManager::KObjectManager(QString _objects_dir,
                               double  _pixel_size_mm)
{
  pixel_size_mm = _pixel_size_mm;
  QDir dir(_objects_dir);
  if (!dir.exists())
    dir.mkdir(_objects_dir);
  auto fi_list = dir.entryInfoList(QDir::Files, QDir::Name);
  for (auto fi: fi_list)
  {
    KObject obj;
    obj.load(fi.absoluteFilePath(), pixel_size_mm);
    objects.append(obj);
  }
  objects_dir   = _objects_dir;
  pixel_size_mm = _pixel_size_mm;
}

QString KObjectManager::generateObjectFileName()
{
  return objects_dir + "/" +
         QDateTime::currentDateTime().toString("yyyyMMd-hhmmss") +
         ".kobject";
}

void KObjectManager::createObject(KShape sh)
{
  active_object.cl.type         = sh.type;
  active_object.cl.style        = sh.style;
  active_object.cl.pen_width_mm = sh.width_mm;
  active_object.cl.pen          = sh.pen;
  active_object.cl.brush        = sh.brush;
  active_object.cl.image        = sh.image;
}

void KObjectManager::paintObject(QPainter* p, KObject obj)
{
  if (obj.polygons.isEmpty())
    return;

  if (obj.cl.type == KShape::Point)
  {
    auto& img = obj.cl.image;
    auto  pix = kcoor2pix(obj.polygons.first().first());
    if (img.isNull())
      p->drawEllipse(pix, 5, 5);
    else
    {
      auto s = img.size();
      p->drawImage(
          QPoint{pix.x() - s.width() / 2, pix.y() - s.height() / 2},
          obj.cl.image);
    }
    return;
  }

  if (obj.cl.type == KShape::Line)
  {
    QPen pen = QPen(obj.cl.pen, obj.getWidthPix(pixel_size_mm));
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
  if (obj.cl.type == KShape::Polygon)
  {
    QPen   pen   = QPen(obj.cl.pen, obj.getWidthPix(pixel_size_mm));
    QBrush brush = QBrush(obj.cl.brush);
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

void KObjectManager::addPoint(KGeoCoor coor)
{
  if (active_object.cl.type == KShape::None)
    return;
  auto type = active_object.cl.type;
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

void KObjectManager::paint(QPainter* p)
{
  for (auto& obj: objects)
    paintObject(p, obj);
  paintObject(p, active_object);
}

void KObjectManager::acceptObject()
{
  active_object.save(generateObjectFileName());
  objects.append(active_object);
  active_object.polygons.clear();
  active_object.cl.type = KShape::None;
  updated();
  finishEdit();
}

void KObjectManager::loadFileWithoutUpdate(QString path)
{
  KObject object;
  object.load(path, pixel_size_mm);
  objects.append(object);
}

void KObjectManager::loadFileWithoutUpdate(QFileInfo file_info)
{
  loadFile(file_info.absoluteFilePath());
}

void KObjectManager::loadFile(QString path)
{
  loadFileWithoutUpdate(path);
  emit updated();
}
