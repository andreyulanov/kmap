#include "math.h"
#include "kobject.h"
#include "kserialize.h"
#include <QDir>
#include <QDebug>
#include <QUuid>

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
  return round(KObjectClass::default_width_mm / pixel_size_mm);
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
    obj.guid = fi.fileName().remove(".kobject");
    objects.append(obj);
  }
  objects_dir   = _objects_dir;
  pixel_size_mm = _pixel_size_mm;
}

QString KObjectManager::getObjectPath(QUuid object_guid)
{
  return objects_dir + "/" + object_guid.toString() + ".kobject";
}

void KObjectManager::removeObject()
{
  if (selected_object_idx >= 0)
  {
    QFile().remove(
        getObjectPath(objects.at(selected_object_idx).guid));
    objects.remove(selected_object_idx);
    selected_object_idx = -1;
  }
  finishEdit();
  updated();
}

void KObjectManager::createObject(KShape sh)
{
  KObject obj;
  obj.cl.type         = sh.type;
  obj.cl.style        = sh.style;
  obj.cl.pen_width_mm = sh.width_mm;
  obj.cl.pen          = sh.pen;
  obj.cl.brush        = sh.brush;
  obj.cl.image        = sh.image;
  objects.append(obj);
  selected_object_idx    = objects.count() - 1;
  is_creating_new_object = true;
}

void KObjectManager::paintObject(QPainter* p, KObject obj,
                                 bool highlighted)
{
  if (obj.polygons.isEmpty())
    return;

  if (obj.cl.type == KShape::Point)
  {
    auto& img = obj.cl.image;
    auto  pix = deg2pix(obj.polygons.first().first());
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
        polygon_pix.append(deg2pix(point));
      if (highlighted)
        p->setPen(QPen(Qt::yellow, w * 2, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
      else
        p->setPen(QPen(obj.cl.pen, w, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
      p->drawPolyline(polygon_pix);
      if (highlighted)
      {
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
  if (obj.cl.type == KShape::Polygon)
  {
    int    w     = obj.getWidthPix(pixel_size_mm);
    QPen   pen   = QPen(obj.cl.pen, w);
    QBrush brush = QBrush(obj.cl.brush);
    for (auto polygon: obj.polygons)
    {
      QPolygon polygon_pix;
      for (auto point: polygon)
        polygon_pix.append(deg2pix(point));

      if (highlighted)
        p->setPen(QPen(Qt::yellow, w * 2));
      else
        p->setPen(pen);
      p->setBrush(brush);
      if (polygon_pix.count() == 2)
        p->drawPolyline(polygon_pix);
      else
        p->drawPolygon(polygon_pix);

      if (highlighted)
      {
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
}

void KObjectManager::onTapped(KGeoCoor coor)
{
  if (selected_object_idx >= 0)
  {
    auto& obj = objects[selected_object_idx];
    if (obj.polygons.isEmpty())
    {
      obj.name = "object1";
      KGeoPolygon polygon;
      polygon.append(coor);
      obj.polygons.append(polygon);
      updated();
      return;
    }
  }

  auto p0            = deg2pix(coor);
  auto proximity_pix = proximity_mm / pixel_size_mm;
  if (selected_object_idx < 0)
  {
    selected_object_idx    = getObjectIdxAt(p0);
    is_creating_new_object = false;
    startEdit();
    updated();
    return;
  }
  if (selected_object_idx < 0)
    return;
  auto& obj  = objects[selected_object_idx];
  auto  type = obj.cl.type;
  if (type == KShape::Point)
  {
    obj.name = "object1";
    KGeoPolygon poly;
    poly.append(coor);
    obj.polygons.append(poly);
    acceptObject();
    return;
  }
  else
  {
    if (obj.isEmpty())
    {
      obj.name = "object1";
      KGeoPolygon polygon;
      polygon.append(coor);
      obj.polygons.append(polygon);
    }
    else
    {
      for (int polygon_idx = -1; auto& polygon: obj.polygons)
      {
        polygon_idx++;
        if (polygon.count() < 4 || type == KShape::Line ||
            is_creating_new_object)
        {
          polygon.append(coor);
          updated();
          return;
        }
        QPolygon polygon_pix;
        for (auto& p: polygon)
          polygon_pix.append(deg2pix(p));
        polygon_pix.append(deg2pix(polygon.first()));
        auto point_idx = kmath::getPolylinePointIdxAt(p0, polygon_pix,
                                                      proximity_pix);
        if (point_idx >= 0)
        {
          polygon.insert(point_idx, coor);
          break;
        }
      }
    }
  }
  updated();
}

void KObjectManager::paint(QPainter* p)
{
  for (int idx = -1; auto& obj: objects)
  {
    idx++;
    bool highligted = idx == selected_object_idx;
    paintObject(p, obj, highligted);
  }
}

void KObjectManager::acceptObject()
{
  if (selected_object_idx < 0)
    return;
  auto& obj = objects[selected_object_idx];
  if (obj.guid.isNull())
    obj.guid = QUuid::createUuid();
  obj.save(getObjectPath(obj.guid.toString()));
  is_creating_new_object = false;
  selected_object_idx    = -1;
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
  moving_point_idx = getSelectedObjectPointIdxAt(p0);
}

void KObjectManager::stopMovingPoint()
{
  moving_point_idx = {-1, -1};
}

bool KObjectManager::canScroll()
{
  return moving_point_idx == QPair{-1, -1};
}

void KObjectManager::movePoint(QPoint p)
{
  if (selected_object_idx < 0 ||
      selected_object_idx >= objects.count())
    return;

  auto& obj = objects[selected_object_idx];
  if (obj.isEmpty())
    obj = objects[selected_object_idx];

  if (moving_point_idx.first < 0 ||
      moving_point_idx.first >= obj.polygons.count())
    return;

  auto& polygon = obj.polygons[moving_point_idx.first];

  if (moving_point_idx.second < 0 ||
      moving_point_idx.second >= polygon.count())
    return;

  polygon[moving_point_idx.second] = scr2deg(p);
  updated();
}

QPair<int, int> KObjectManager::getSelectedObjectPointIdxAt(QPoint p0)
{
  if (selected_object_idx < 0)
    return {-1, -1};
  auto& obj           = objects[selected_object_idx];
  auto  proximity_pix = proximity_mm / pixel_size_mm;
  for (int polygon_idx = -1; auto polygon: obj.polygons)
  {
    polygon_idx++;
    QPolygon polygon_pix;
    for (int point_idx = -1; auto p: polygon)
    {
      point_idx++;
      auto pix = deg2scr(p);
      auto d   = kmath::getDistance(pix, p0);
      if (d < proximity_pix)
        return {polygon_idx, point_idx};
    }
  }
  return {-1, -1};
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
      auto point_pos = deg2pix(obj.polygons.first().first());
      if ((point_pos - p0).manhattanLength() < proximity_pix)
        return idx;
    }
    if (obj.cl.type == KShape::Line)
    {
      for (auto polygon: obj.polygons)
      {
        QPolygon polygon_pix;
        for (auto p: polygon)
          polygon_pix.append(deg2pix(p));
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
          polygon_pix.append(deg2pix(p));
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
