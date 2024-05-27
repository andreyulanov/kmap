#include "math.h"
#include "kfreeobject.h"
#include "kserialize.h"
#include <QDir>
#include <QDebug>
#include <QUuid>

void KFreeObject::save(QString path)
{
  QFile f(path);
  if (!f.open(QIODevice::WriteOnly))
  {
    qDebug() << "ERROR: unable to write to" << path;
    return;
  }

  using namespace KSerialize;

  shape.save(&f);
  write(&f, polygons.count());
  for (auto polygon: polygons)
  {
    write(&f, polygon.count());
    for (auto point: polygon)
      write(&f, point);
  }
  write(&f, attributes.count());
  for (auto attr: attributes)
  {
    write(&f, attr.type);
    write(&f, attr.name);
    write(&f, attr.min_mip);
    write(&f, attr.max_mip);
    write(&f, attr.data);
  }
}

void KFreeObject::load(QString path, double pixel_size_mm)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly))
  {
    qDebug() << "ERROR: unable to write to" << path;
    return;
  }

  using namespace KSerialize;

  shape.load(&f, pixel_size_mm);
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
  read(&f, n);
  attributes.resize(n);
  for (auto& attr: attributes)
  {
    read(&f, attr.type);
    read(&f, attr.name);
    read(&f, attr.min_mip);
    read(&f, attr.max_mip);
    read(&f, attr.data);
  }
}

bool KFreeObject::isEmpty()
{
  return polygons.isEmpty();
}

int KFreeObject::getWidthPix(double pixel_size_mm)
{
  return round(shape.width_mm / pixel_size_mm);
}

KFreeObjectManager::KFreeObjectManager(QString _objects_dir,
                                       double  _pixel_size_mm)
{
  pixel_size_mm = _pixel_size_mm;
  QDir dir(_objects_dir);
  if (!dir.exists())
    dir.mkdir(_objects_dir);
  auto fi_list = dir.entryInfoList(QDir::Files, QDir::Name);
  for (auto fi: fi_list)
  {
    KFreeObject obj;
    obj.load(fi.absoluteFilePath(), pixel_size_mm);
    obj.guid = fi.fileName().remove(".kfree");
    objects.append(obj);
  }
  objects_dir   = _objects_dir;
  pixel_size_mm = _pixel_size_mm;
}

QString KFreeObjectManager::getObjectPath(QUuid object_guid)
{
  return objects_dir + "/" + object_guid.toString() + ".kfree";
}

void KFreeObjectManager::removeObject()
{
  if (edited_object_idx >= 0)
  {
    QFile().remove(getObjectPath(objects.at(edited_object_idx).guid));
    objects.remove(edited_object_idx);
    edited_object_idx = -1;
  }
  for (auto guid: selected_guids)
  {
    for (int obj_idx = -1; auto& obj: objects)
    {
      obj_idx++;
      if (obj.guid == guid)
      {
        QFile().remove(getObjectPath(objects.at(obj_idx).guid));
        objects.remove(obj_idx);
        break;
      }
    }
  }
  selected_guids.clear();
  finishEdit();
  updated();
}

void KFreeObjectManager::createObject(KShape sh)
{
  KFreeObject obj;
  obj.shape = sh;
  objects.append(obj);
  edited_object_idx      = objects.count() - 1;
  is_creating_new_object = true;
}

void KFreeObjectManager::paintObject(QPainter* p, KFreeObject obj,
                                     PaintMode paint_mode)
{
  if (obj.polygons.isEmpty())
    return;

  auto w = obj.getWidthPix(pixel_size_mm);
  if (obj.shape.type == KShape::Point)
  {
    auto& img = obj.shape.image;
    auto  pix = deg2pix(obj.polygons.first().first());
    if (img.isNull())
      p->drawEllipse(pix, w, w);
    else
    {
      auto s = img.size();
      auto pos =
          QPoint{pix.x() - s.width() / 2, pix.y() - s.height() / 2};
      if (paint_mode != PaintMode::Normal)
      {
        p->setPen(Qt::NoPen);
        p->setBrush(Qt::yellow);
        p->drawEllipse(QRect{pos, obj.shape.image.size()}.adjusted(
            -w / 2, -w / 2, w / 2, w / 2));
      }
      p->drawImage(pos, obj.shape.image);
    }
    return;
  }

  if (obj.shape.type == KShape::Line)
  {
    for (auto polygon: obj.polygons)
    {
      int      w = obj.getWidthPix(pixel_size_mm);
      QPolygon polygon_pix;
      for (auto point: polygon)
        polygon_pix.append(deg2pix(point));
      if (paint_mode != PaintMode::Normal)
        p->setPen(QPen(Qt::yellow, w * 2, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
      else
        p->setPen(QPen(obj.shape.pen, w, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
      p->drawPolyline(polygon_pix);
      if (paint_mode == PaintMode::Edited)
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
  if (obj.shape.type == KShape::Polygon)
  {
    int    w     = obj.getWidthPix(pixel_size_mm);
    QPen   pen   = QPen(obj.shape.pen, w);
    QBrush brush = QBrush(obj.shape.brush);
    for (auto polygon: obj.polygons)
    {
      QPolygon polygon_pix;
      for (auto point: polygon)
        polygon_pix.append(deg2pix(point));

      if (paint_mode != PaintMode::Normal)
        p->setPen(QPen(Qt::yellow, w * 2));
      else
        p->setPen(pen);
      p->setBrush(brush);
      if (polygon_pix.count() == 2)
        p->drawPolyline(polygon_pix);
      else
        p->drawPolygon(polygon_pix);

      if (paint_mode == PaintMode::Edited)
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

void KFreeObjectManager::onTapped(KGeoCoor coor)
{
  auto p0 = deg2pix(coor);

  if (!selected_guids.isEmpty())
  {
    auto object_idx    = getObjectIdxAt(p0);
    auto selected_guid = objects.at(object_idx).guid;
    if (!selected_guids.contains(selected_guid))
      selected_guids.append(selected_guid);
    updated();
    return;
  }

  if (edited_object_idx >= 0)
  {
    auto& obj = objects[edited_object_idx];
    if (obj.polygons.isEmpty())
    {
      KGeoPolygon polygon;
      polygon.append(coor);
      obj.polygons.append(polygon);
      updated();
      return;
    }
    auto new_selected_object_idx = getObjectIdxAt(p0);
    if (new_selected_object_idx >= 0 &&
        new_selected_object_idx != edited_object_idx)
    {
      selected_guids.clear();
      selected_guids.append(objects.at(edited_object_idx).guid);
      selected_guids.append(objects.at(new_selected_object_idx).guid);
      edited_object_idx = -1;
      updated();
      return;
    }
  }

  auto proximity_pix = proximity_mm / pixel_size_mm;
  if (edited_object_idx < 0 && selected_guids.isEmpty())
  {
    edited_object_idx      = getObjectIdxAt(p0);
    is_creating_new_object = false;
    if (edited_object_idx >= 0)
    {
      startEdit();
      updated();
    }
    return;
  }
  if (edited_object_idx < 0)
    return;
  auto& obj  = objects[edited_object_idx];
  auto  type = obj.shape.type;
  if (type == KShape::Point)
  {
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

void KFreeObjectManager::paint(QPainter* p)
{
  for (int idx = -1; auto& obj: objects)
  {
    idx++;
    PaintMode mode = PaintMode::Normal;
    if (idx == edited_object_idx)
      mode = PaintMode::Edited;
    else if (selected_guids.contains(obj.guid))
      mode = PaintMode::Selected;
    paintObject(p, obj, mode);
  }
}

void KFreeObjectManager::acceptObject()
{
  selected_guids.clear();
  if (edited_object_idx >= 0)
  {
    auto& obj = objects[edited_object_idx];
    if (obj.guid.isNull())
      obj.guid = QUuid::createUuid();
    QString file_name = getObjectPath(obj.guid.toString());
    obj.save(file_name);
    saved(file_name);
    is_creating_new_object = false;
    edited_object_idx      = -1;
  }
  updated();
  finishEdit();
}

void KFreeObjectManager::loadFileWithoutUpdate(QString path)
{
  KFreeObject object;
  object.load(path, pixel_size_mm);
  objects.append(object);
}

void KFreeObjectManager::loadFileWithoutUpdate(QFileInfo file_info)
{
  loadFile(file_info.absoluteFilePath());
}

void KFreeObjectManager::loadFile(QString path)
{
  loadFileWithoutUpdate(path);
  updated();
}

void KFreeObjectManager::startMovingPoint(QPoint p0)
{
  if (edited_object_idx < 0)
    return;
  moving_point_idx = getSelectedObjectPointIdxAt(p0);
}

void KFreeObjectManager::stopMovingPoint()
{
  moving_point_idx = {-1, -1};
}

bool KFreeObjectManager::canScroll()
{
  return moving_point_idx == QPair{-1, -1};
}

void KFreeObjectManager::movePoint(QPoint p)
{
  if (edited_object_idx < 0 || edited_object_idx >= objects.count())
    return;

  auto& obj = objects[edited_object_idx];
  if (obj.isEmpty())
    obj = objects[edited_object_idx];

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

QPair<int, int>
KFreeObjectManager::getSelectedObjectPointIdxAt(QPoint p0)
{
  if (edited_object_idx < 0)
    return {-1, -1};
  auto& obj           = objects[edited_object_idx];
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

int KFreeObjectManager::getObjectIdxAt(QPoint p0)
{
  using namespace kmath;
  auto proximity_pix = proximity_mm / pixel_size_mm;
  for (int idx = -1; auto obj: objects)
  {
    idx++;
    if (obj.shape.type == KShape::Point)
    {
      auto point_pos = deg2pix(obj.polygons.first().first());
      if ((point_pos - p0).manhattanLength() < proximity_pix)
        return idx;
    }
    if (obj.shape.type == KShape::Line)
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
    if (obj.shape.type == KShape::Polygon)
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

int KFreeObjectManager::getObjectIdxInsidePolygon(QPolygon polygon)
{
  return -1;
}
