#include "math.h"
#include "kobject.h"
#include "kserialize.h"
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

void KObjectManager::paintObject(QPainter* p, KObject obj,
                                 bool highlighted)
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
      auto pos =
          QPoint{pix.x() - s.width() / 2, pix.y() - s.height() / 2};
      if (highlighted)
      {
        p->setPen(Qt::NoPen);
        p->setBrush(Qt::yellow);
        p->drawEllipse(
            QRect{pos, obj.cl.image.size()}.adjusted(-5, -5, 5, 5));
      }
      p->drawImage(pos, obj.cl.image);
    }
    return;
  }

  if (obj.cl.type == KShape::Line)
  {
    for (auto polygon: obj.polygons)
    {
      int      w = obj.getWidthPix(pixel_size_mm);
      QPolygon polygon_pix;
      for (auto point: polygon)
        polygon_pix.append(kcoor2pix(point));
      if (highlighted)
        p->setPen(QPen(Qt::yellow, w * 2, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
      else
      {
        QPen pen = QPen(obj.cl.pen, w, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin);
        p->setPen(pen);
      }
      p->drawPolyline(polygon_pix);
    }
  }
  if (obj.cl.type == KShape::Polygon)
  {
    int    w     = obj.getWidthPix(pixel_size_mm);
    QPen   pen   = QPen(obj.cl.pen, w);
    QBrush brush = QBrush(obj.cl.brush);
    for (auto polygon: obj.polygons)
    {
      QPolygon polygon_pix;
      for (auto point: polygon)
        polygon_pix.append(kcoor2pix(point));

      if (highlighted)
        p->setPen(QPen(Qt::yellow, w * 2));
      else
      {
        p->setPen(pen);
        p->setBrush(brush);
      }
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
}

void KObjectManager::addPoint(KGeoCoor coor)
{
  if (active_object.cl.type == KShape::None)
  {
    auto coor_pix = kcoor2pix(coor);
    auto obj_idx  = getObjectIdxAt(coor_pix);
    selectObject(obj_idx);
    return;
  }
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
  if (selected_object_idx >= 0)
    paintObject(p, objects.at(selected_object_idx), true);
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
  updated();
}

void KObjectManager::startMovingPoint(QPoint p0)
{
  if (selected_object_idx < 0)
    return;
  selected_point_idx = getSelectedObjectPointIdxAt(p0);
}

int KObjectManager::getSelectedObjectPointIdxAt(QPoint p0)
{
  if (selected_object_idx < 0)
    return -1;
  auto obj           = objects.at(selected_object_idx);
  auto proximity_pix = proximity_mm / pixel_size_mm;
  for (int polygon_idx = -1; auto polygon: obj.polygons)
  {
    polygon_idx++;
    QPolygon polygon_pix;
    for (int point_idx = -1; auto p: polygon)
    {
      point_idx++;
      auto pix = kcoor2pix(p);
      auto d   = kmath::getDistance(pix, p0);
      if (d < proximity_pix)
        return point_idx;
    }
  }
  return -1;
}

int KObjectManager::getObjectIdxAt(QPoint p0)
{
  using namespace kmath;
  auto proximity_pix = proximity_mm / pixel_size_mm;
  for (int idx = -1; auto obj: objects)
  {
    idx++;
    if (obj.cl.type == KShape::Point)
    {
      auto point_pos = kcoor2pix(obj.polygons.first().first());
      if ((point_pos - p0).manhattanLength() < proximity_pix)
        return idx;
    }
    if (obj.cl.type == KShape::Line)
    {
      for (auto polygon: obj.polygons)
      {
        QPolygon polygon_pix;
        for (auto p: polygon)
          polygon_pix.append(kcoor2pix(p));
        if (isNearPolyline(p0, polygon_pix, proximity_pix))
          return idx;
      }
    }
    if (obj.cl.type == KShape::Polygon)
    {
      for (auto polygon: obj.polygons)
      {
        QPolygon polygon_pix;
        for (auto p: polygon)
          polygon_pix.append(kcoor2pix(p));
        if (polygon_pix.containsPoint(p0, Qt::OddEvenFill))
          return idx;
      }
    }
  }
  return -1;
}

int KObjectManager::getObjectIdxInsidePolygon(QPolygon polygon)
{
  return -1;
}

void KObjectManager::selectObject(int v)
{
  selected_object_idx = v;
  updated();
}
