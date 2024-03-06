﻿#include "math.h"
#include "krender.h"
#include "klocker.h"
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QPainterPath>
#include <numeric>

using namespace kmap;

QPoint KRender::deg2pix(const KGeoCoor& deg) const
{
  return meters2pix(deg.toMeters());
}

KGeoCoor KRender::pix2deg(QPoint pix) const
{
  return KGeoCoor::fromMeters(pix2meters(pix));
}

QPoint KRender::meters2pix(QPointF coor_m) const
{
  auto rectm = getDrawRectM();
  int  pix_x = (coor_m.x() - rectm.left()) / mip;
  int  pix_y = (coor_m.y() - rectm.top()) / mip;
  return {pix_x, pix_y};
}
QPointF KRender::pix2meters(QPointF pix) const
{
  auto   rectm = getDrawRectM();
  double xm    = pix.x() * mip + rectm.left();
  double ym    = pix.y() * mip + rectm.top();
  return {xm, ym};
}

KRender::~KRender()
{
  QThreadPool().globalInstance()->waitForDone();
  stopAndWait();
}

const KMap* KRender::appendMap(QString path, double min_mip,
                               double max_mip, bool load_now)
{
  return insertMap(maps.count(), path, min_mip, max_mip, load_now);
}

const KMap* KRender::insertMap(int idx, QString path, double min_mip,
                               double max_mip, bool load_now)
{
  auto map = new KMap(path, min_mip, max_mip);
  connect(map, &KMap::loaded, this, &KRender::onLoaded,
          Qt::UniqueConnection);
  map->loadGlobal(load_now);
  maps.insert(idx, map);
  return map;
}

void KRender::setMip(double v)
{
  mip = v;
}

double KRender::getMip() const
{
  return mip;
}

void KRender::setCenterM(QPointF v)
{
  center_m = v;
}

QPointF KRender::getCenterM() const
{
  return center_m;
}

QPointF KRender::getRenderCenterM() const
{
  return render_center_m;
}

void KRender::setPixmapSize(QSize v)
{
  pixmap_size = v * render_window_size_coef;
}

void KRender::setMinObjectSizePix(int v)
{
  min_object_size_pix = v;
}

void KRender::setBackgroundColor(QColor v)
{
  background_color = v;
}

double KRender::getRenderWindowSizeCoef() const
{
  return render_window_size_coef;
}

void KRender::setRenderWindowSizeCoef(double v)
{
  render_window_size_coef = v;
}

const QPixmap* KRender::getPixmap() const
{
  return getting_pixmap_enabled ? &render_pixmap : nullptr;
}

const KMapCollection* KRender::getMaps() const
{
  return &maps;
}

QRectF KRender::getDrawRectM() const
{
  QSizeF size_m      = {pixmap_size.width() * mip,
                        pixmap_size.height() * mip};
  QRectF draw_rect_m = {center_m.x() - size_m.width() / 2,
                        center_m.y() - size_m.height() / 2,
                        size_m.width(), size_m.height()};
  return draw_rect_m;
}

void KRender::onLoaded()
{
  if (rendering_enabled)
    start();
}

bool KRender::needToLoadSmall(const KMap* map)
{
  return mip < map->local_load_mip;
}

void KRender::checkUnload()
{
  auto draw_rect_m  = getDrawRectM();
  int  loaded_count = 0;
  for (int i = -1; auto& map: maps)
  {
    i++;
    if (i == 0)
      continue;
    if (map->global_tile.status == KObjectCollection::Loaded)
    {
      if (!needToLoadMap(map, draw_rect_m))
        if (loaded_count > 1)
          map->clear();
      loaded_count++;
    }
  }
}

bool KRender::needToLoadMap(const KMap*   map,
                            const QRectF& draw_rect_m)
{
  if (map->min_mip > 0 && render_mip < map->min_mip)
    return false;
  if (map->max_mip > 0 && render_mip > map->max_mip)
    return false;
  auto map_rect_m       = map->frame.toMeters();
  bool frame_intersects = draw_rect_m.intersects(map_rect_m);
  if (!frame_intersects)
    return false;

  auto top_left_pix = pix2deg({0, 0});
  auto bottom_right_pix =
      pix2deg({render_pixmap.width(), render_pixmap.height()});
  auto top_left_deg =
      QPointF{top_left_pix.longitude(), top_left_pix.latitude()};
  auto bottom_right_deg = QPointF{bottom_right_pix.longitude(),
                                  bottom_right_pix.latitude()};
  auto frame_deg =
      QRectF{top_left_deg, bottom_right_deg}.normalized();

  QVector<QPointF> corners;
  corners << frame_deg.topLeft();
  corners << frame_deg.topRight();
  corners << frame_deg.bottomLeft();
  corners << frame_deg.bottomRight();
  for (auto corner: corners)
    if (map->containsPoint(corner))
      return true;
  return false;
}

void KRender::checkLoad()
{
  auto draw_rect_m = getDrawRectM();
  for (auto& map: maps)
  {
    auto map_rect_m = map->frame.toMeters();

    if (!needToLoadMap(map, draw_rect_m))
      continue;

    if (map->global_tile.status == KObjectCollection::Null)
    {
      QtConcurrent::run(
          [&map]()
          {
            map->loadGlobal(true);
          });
      continue;
    }
    if (map->global_tile.status == KObjectCollection::Loaded)
    {
      if (needToLoadMap(map, draw_rect_m))
      {
        int    tile_side_count = sqrt(map->local_tiles.count());
        QSizeF tile_size_m = {map_rect_m.width() / tile_side_count,
                              map_rect_m.height() / tile_side_count};
        for (int tile_idx = 0; auto& tile: map->local_tiles)
        {
          int    tile_idx_y = tile_idx / tile_side_count;
          int    tile_idx_x = tile_idx - tile_idx_y * tile_side_count;
          double tile_left =
              map_rect_m.x() + tile_idx_x * tile_size_m.width();
          double tile_top =
              map_rect_m.y() + tile_idx_y * tile_size_m.height();
          QRectF tile_rect_m = {{tile_left, tile_top}, tile_size_m};
          if (!tile && tile_rect_m.intersects(draw_rect_m) &&
              render_mip < map->local_load_mip)
            QtConcurrent::run(map, &KMap::loadLocal, tile_idx,
                              tile_rect_m);
          tile_idx++;
        }
      }
    }
  }
}

void KRender::drawOutlinedText(QPainter* p, const QString& text,
                               const QColor& tcolor)
{
  p->setPen(Qt::white);
  auto shifts = {-2, 0, 2};
  for (auto x: shifts)
    for (auto y: shifts)
      p->drawText(x, y, text);

  p->setPen(tcolor);
  p->drawText(0, 0, text);
}

void KRender::drawOutlinedText(QPainter* p, const DrawTextEntry& dte)
{
  p->setPen(Qt::white);
  auto shifts = {-2, 0, 2};
  for (auto x: shifts)
    for (auto y: shifts)
    {
      p->save();
      p->translate(x, y);
      p->drawText(dte.rect,
                  dte.alignment | Qt::TextWordWrap | Qt::TextDontClip,
                  dte.text);
      p->restore();
    }

  p->setPen(dte.shape->pen);
  p->drawText(dte.rect,
              dte.alignment | Qt::TextWordWrap | Qt::TextDontClip,
              dte.text);

  if (dte.is_selected)
  {
    auto pos = dte.rect.center();
    pos -= QPoint(dte.shape->large_image.width() / 2,
                  dte.shape->large_image.height() + 10);
    p->drawImage(pos, dte.shape->large_image);
  }
}

void KRender::addDrawTextEntry(
    QVector<DrawTextEntry>& draw_text_array, DrawTextEntry new_dte)
{
  bool can_fit = true;
  for (auto dte: draw_text_array)
  {
    if (new_dte.shape->id != selected_category_name)
      if (dte.actual_rect.intersects(new_dte.actual_rect))
      {
        can_fit = false;
        break;
      }
  }
  if (can_fit)
    draw_text_array.append(new_dte);
};

QPoint KRender::kcoor2pix(KGeoCoor kp) const
{
  auto m = kp.toMeters();
  return {int((m.x() - render_top_left_m.x()) / render_mip),
          int((m.y() - render_top_left_m.y()) / render_mip)};
}

void KRender::drawPointObject(QPainter* p, const KObject* obj)
{
  auto& frame = obj->frame;

  auto coor_m = frame.top_left.toMeters();

  if (!render_frame_m.contains(coor_m))
    return;

  auto sh = obj->shape;
  p->setPen(QPen(sh->pen, 2));
  p->setBrush(sh->brush);
  auto        kpos       = obj->polygons.first()->first();
  QPoint      pos        = kcoor2pix(kpos);
  auto        text_shift = obj->shape->getWidthPix();
  int         max_length = 0;
  QStringList str_list;
  if (!obj->name.isEmpty())
  {
    str_list += obj->name;
    max_length = obj->name.count();
  }

  for (auto attr: obj->shape->attributes)
    if (attr.visible && render_mip < attr.max_mip)
    {
      auto str = obj->attributes.value(attr.name);
      if (str == obj->name)
        continue;
      str_list += str;
      max_length = std::max(max_length, str.count());
    }

  auto rect =
      QRect{pos.x(), pos.y(), max_length * obj->shape->getWidthPix(),
            str_list.count() * obj->shape->getWidthPix()};

  bool intersects = false;
  for (auto r: point_object_text_rects)
  {
    if (rect.intersects(r))
    {
      intersects = true;
      break;
    }
  }
  if (intersects)
    return;

  point_object_text_rects.append(rect);

  p->save();
  p->translate(pos);
  auto f = p->font();
  f.setPixelSize(obj->shape->getWidthPix());
  p->setFont(f);
  p->translate(QPoint(0, obj->shape->getWidthPix() / 2 + 2));
  for (auto str: str_list)
  {
    p->translate(QPoint(0, text_shift));
    drawOutlinedText(p, str, sh->tcolor);
  }
  p->restore();
  if (sh->image.isNull())
    p->drawEllipse(pos, 5, 5);
  else
  {
    QImage img = obj->shape->id == selected_category_name ?
                     sh->large_image :
                     sh->image;
    pos = {pos.x() - img.width() / 2, pos.y() - img.height() / 2};
    p->drawImage(pos, img);
  }
}

QPolygon KRender::poly2pix(const KGeoPolygon& polygon)
{
  QPoint   prev_point_pix = kcoor2pix(polygon.first());
  QPolygon pl;
  pl.append(prev_point_pix);
  for (int i = 1; i < polygon.count(); i++)
  {
    auto kpoint    = polygon.at(i);
    auto point_pix = kcoor2pix(kpoint);
    auto d         = point_pix - prev_point_pix;
    if (d.manhattanLength() > 2 || i == polygon.count() - 1)
    {
      pl.append(point_pix);
      prev_point_pix = point_pix;
    }
  }
  return pl;
}

void KRender::drawPolygonObject(QPainter* p, const KObject* obj,
                                int render_idx)
{
  auto& frame = obj->frame;
  QRect obj_frame_pix;

  auto   top_left_m     = frame.top_left.toMeters();
  auto   bottom_right_m = frame.bottom_right.toMeters();
  QRectF obj_frame_m    = {top_left_m, bottom_right_m};

  if (!obj_frame_m.intersects(render_frame_m))
    return;

  double obj_span_m   = sqrt(pow(obj_frame_m.width(), 2) +
                             pow(obj_frame_m.height(), 2));
  int    obj_span_pix = obj_span_m / render_mip;
  auto   size_pix     = frame.getSizeMeters() / render_mip;
  if (obj->shape->id != selected_category_name &&
      !obj->shape->id.contains("admin"))
    if (size_pix.width() < min_object_size_pix ||
        size_pix.height() < min_object_size_pix)
      return;

  auto sh = obj->shape;
  if (sh->getWidthPix() == 0 || sh->pen == Qt::black)
    p->setPen(Qt::NoPen);
  else
    p->setPen(sh->pen);
  if (sh->style == KShape::Hatch)
    p->setBrush(QBrush(sh->brush, Qt::HorPattern));
  else if (sh->style == KShape::BDiag)
    p->setBrush(QBrush(sh->brush, Qt::BDiagPattern));
  else if (sh->style == KShape::FDiag)
    p->setBrush(QBrush(sh->brush, Qt::FDiagPattern));
  else if (sh->style == KShape::Horiz)
    p->setBrush(QBrush(sh->brush, Qt::HorPattern));
  else if (sh->style == KShape::Vert)
    p->setBrush(QBrush(sh->brush, Qt::VerPattern));
  else
    p->setBrush(sh->brush);

  QPainterPath path;
  for (int polygon_idx = -1; auto polygon: obj->polygons)
  {
    polygon_idx++;

    if (polygon_idx == 0)
    {
      auto polygon_size_m = polygon->getFrame().getSizeMeters();
      if (!obj->shape->id.contains("admin"))
        if (obj->shape->id != selected_category_name)
          if (polygon_size_m.width() / mip < min_object_size_pix &&
              polygon_size_m.height() / mip < min_object_size_pix)
            continue;
    }

    auto pl = poly2pix(*polygon);

    if ((polygon_idx == 0 && !obj->name.isEmpty() &&
         obj_span_pix < std::min(pixmap_size.width(),
                                 pixmap_size.height() / 2)) ||
        !obj->shape->image.isNull())
    {

      QPoint top_left_pix     = kcoor2pix(obj->frame.top_left);
      QPoint bottom_right_pix = kcoor2pix(obj->frame.bottom_right);
      obj_frame_pix           = {top_left_pix, bottom_right_pix};

      auto  c = obj_frame_pix.center();
      QRect actual_rect;
      int   w = pixmap_size.width() / 32;
      actual_rect.setTopLeft(c);
      actual_rect.setSize({w, w});
      actual_rect.translate({-w / 2, -w / 2});

      addDrawTextEntry(draw_text_array[render_idx],
                       {obj->name, sh, obj_frame_pix, actual_rect,
                        Qt::AlignCenter,
                        obj->shape->id == selected_category_name});
    }

    if (obj->polygons.count() == 1)
    {
      p->drawPolygon(pl);
      continue;
    }

    //    if (polygon_idx == 0)
    //      path.addPolygon(pl);
    //    else
    //    {
    //      QPainterPath inner_path;
    //      inner_path.addPolygon(pl);
    //      path = path.subtracted(inner_path);
    //    }

    //    if (polygon_idx == obj->polygons.count() - 1)
    //      p->drawPath(path);
    if (polygon_idx == 0)
    {
      p->drawPolygon(pl);
    }
    else
    {
      p->setBrush(Qt::white);
      p->drawPolygon(pl);
    }
  }
  if (!obj->shape->image.isNull() &&
      obj_frame_pix.width() > sh->image.width() * 2 &&
      obj_frame_pix.height() > sh->image.height() * 2)
  {
    auto obj_center = obj_frame_pix.center();
    auto pos        = QPoint{obj_center.x() - sh->image.width() / 2,
                      obj_center.y() - sh->image.height() / 2};
    if (!obj->name.isEmpty())
      pos -= QPoint(0, obj->shape->getWidthPix() + 5);
    if (obj->shape->id != selected_category_name)
      p->drawImage(pos, sh->image);
  }
}

void KRender::drawLineObject(QPainter* painter, const KObject* obj,
                             int render_idx)
{
  auto& frame = obj->frame;

  auto top_left_m     = frame.top_left.toMeters();
  auto bottom_right_m = frame.bottom_right.toMeters();

  QRectF obj_frame_m = {top_left_m, bottom_right_m};

  if (!obj_frame_m.intersects(render_frame_m))
    return;

  auto sh = obj->shape;

  Qt::PenStyle style = Qt::SolidLine;
  if (sh->style == KShape::Dash)
    style = Qt::DashLine;
  int obj_name_width = 0;
  if (!obj->name.isEmpty())
    obj_name_width =
        painter->font().pixelSize() * obj->name.count() * 0.6;

  auto         fixed_w    = sh->getWidthPix();
  int          sizeable_w = 0;
  int          w          = fixed_w;
  bool         one_way    = false;
  QMapIterator it(obj->attributes);
  while (it.hasNext())
  {
    it.next();
    if (it.key() == "oneway")
      one_way = true;

    if (it.key() == "lanes")
    {
      sizeable_w = 4 * it.value().toInt() / mip;
      w          = std::max((int)fixed_w, sizeable_w);
    }
  }

  painter->setPen(QPen(sh->pen, w, style));
  painter->setBrush(Qt::NoBrush);

  for (int poly_idx = -1; auto polygon: obj->polygons)
  {
    poly_idx++;
    NameHolder nh;
    QPoint     p0;
    double     a0 = 0;

    auto polygon_size_m   = polygon->getFrame().getSizeMeters();
    auto polygon_size_pix = polygon_size_m / mip;
    if (!obj->shape->id.contains("admin"))
    {
      if (polygon_size_pix.width() < 2 &&
          polygon_size_pix.height() < 2)
        continue;
      if (polygon_size_pix.width() < min_object_size_pix &&
          polygon_size_pix.height() < min_object_size_pix)
      {
        QPoint p1 = kcoor2pix(polygon->first());
        QPoint p2 = kcoor2pix(polygon->last());
        painter->drawLine(p1, p2);
        continue;
      }
    }

    auto pl = poly2pix(*polygon);

    auto size_m       = polygon->getFrame().getSizeMeters();
    auto size_pix     = (size_m.width() + size_m.height()) / mip;
    auto hatch_length = size_pix * 0.05;
    if (hatch_length > 5)
      if (sh->style == KShape::Hatch)
      {
        if (hatch_length > 5)
          hatch_length = 5;
        auto p0 = pl.first();
        for (int c = -1; auto p: pl)
        {
          c++;
          double a = 0;
          if (c == 0)
            a = getAngle(p0, pl.at(1));
          else
            a = getAngle(p0, p);
          painter->save();
          painter->translate(p);
          painter->rotate(rad2deg(a));
          int length = getLength(p0, p);
          int step   = 10;
          for (int l = 0; l < length; l += step)
          {
            painter->drawLine(0, 0, 0, hatch_length);
            painter->translate(-step, 0);
          }
          painter->restore();
          p0 = p;
        }
      }

    for (int point_idx = -1; auto p: pl)
    {
      point_idx++;
      if (obj->name.isEmpty())
        continue;
      if (nh.point_count == 0)
      {
        p0 = p;
        nh.point_count++;
        continue;
      }
      auto a = getAngle(p0, p);

      if (nh.point_count == 1)
        a0 = a;
      auto da = a0 - a;
      if (da > M_PI)
        da -= 2 * M_PI;
      else if (da < -M_PI)
        da += 2 * M_PI;
      da = fabs(da);
      if (da > deg2rad(5) || point_idx == polygon->count() - 1)
      {
        if (nh.length_pix > obj_name_width)
        {
          nh.fix(obj, pl.at(nh.start_idx), pl.at(nh.end_idx));
          name_holder_array[render_idx].append(nh);
        }
        nh           = NameHolder();
        nh.start_idx = point_idx;
        p0           = p;
        continue;
      }
      auto length_pix = getLength(p0, p);
      nh.length_pix += length_pix;
      p0 = p;
      nh.point_count++;
      nh.end_idx = point_idx;
    }
    painter->drawPolyline(pl);
    if (sizeable_w > 7)
    {
      painter->setPen(Qt::black);
      auto orig_f = painter->font();
      auto f      = orig_f;
      f.setPixelSize(w);
      painter->setFont(f);
      if (pl.count() > 1 && sizeable_w > 7 && one_way)
      {
        auto p0 = pl.first();
        for (int i = 1; i < pl.count(); i++)
        {
          auto   p1  = pl.at(i);
          auto   a   = getAngle(p0, p1);
          QPoint mid = {(p0.x() + p1.x()) / 2, (p0.y() + p1.y()) / 2};
          painter->save();
          painter->translate(mid);
          painter->rotate(rad2deg(a));
          painter->drawText(0, w * 0.3, "→");
          painter->restore();
          p0 = p1;
        }
      }
      painter->setFont(orig_f);
    }
  }
}

void KRender::NameHolder::fix(const KObject* _obj,
                              const QPoint& start, const QPoint& end)
{
  obj       = _obj;
  mid_point = {(start.x() + end.x()) / 2, (start.y() + end.y()) / 2};
  angle_deg = rad2deg(getAngle(start, end));
  if (angle_deg > 90)
    angle_deg -= 180;
  if (angle_deg < -90)
    angle_deg += 180;
}

bool KRender::isCluttering(const QRect& rect)
{
  bool clutter_flag = false;
  for (auto ex_rect: text_rect_array)
    if (ex_rect.intersects(rect))
    {
      clutter_flag = true;
      break;
    }
  return clutter_flag;
}

bool KRender::checkMipRange(const KObject* obj)
{
  return (obj->shape->min_mip == 0 ||
          render_mip >= obj->shape->min_mip) &&
         (obj->shape->max_mip == 0 ||
          render_mip <= obj->shape->max_mip);
}

bool KRender::drawObject(QPainter* p, const KObject* obj,
                         int render_idx)
{
  switch (obj->shape->type)
  {
  case KShape::Point:
    drawPointObject(p, obj);
    break;
  case KShape::Line:
    drawLineObject(p, obj, render_idx);
    break;
  case KShape::Polygon:
    drawPolygonObject(p, obj, render_idx);
    break;
  default:
    break;
  }
  return canContinue();
}

bool KRender::canContinue()
{
  if (!rendering_enabled)
    emit rendered(0);
  return rendering_enabled;
}

void KRender::checkYieldResult()
{
  if (auto el = yield_timer.elapsed(); el > update_interval_ms)
  {
    getting_pixmap_enabled = true;
    emit rendered(el);
    yield_timer.restart();
  }
}

bool KRender::drawLineNames(QPainter* p)
{
  text_rect_array.clear();
  for (int render_idx = 0; render_idx < KMap::render_count;
       render_idx++)
    for (auto nh: name_holder_array[render_idx])
    {
      p->save();
      QRect text_rect;
      text_rect.setSize(
          {int(p->font().pixelSize() * nh.obj->name.count() * 0.6),
           p->font().pixelSize()});

      QTransform tr;
      tr.translate(nh.mid_point.x(), nh.mid_point.y());
      tr.rotate(nh.angle_deg);
      tr.translate(-text_rect.width() / 2, 0);

      QRect mapped_rect = tr.mapRect(text_rect);
      if (isCluttering(mapped_rect))
      {
        p->restore();
        continue;
      }

      p->setTransform(tr);
      text_rect_array.append(mapped_rect);
      drawOutlinedText(p, nh.obj->name, nh.obj->shape->tcolor);
      p->restore();
      if (!canContinue())
        return false;
    }
  return true;
}

bool KRender::drawPolygonNames(QPainter* p)
{
  for (int render_idx = 0; render_idx < KMap::render_count;
       render_idx++)
    for (auto& dte: draw_text_array[render_idx])
    {
      QFontMetrics fm(p->font());
      auto         actual_rect = fm.boundingRect(
                  dte.actual_rect,
                  Qt::AlignLeft | Qt::TextWordWrap | Qt::TextDontClip,
                  dte.text);
      if (actual_rect.width() > dte.actual_rect.width() * 1.5 &&
          actual_rect.height() > dte.actual_rect.height() * 1.5)
        continue;

      if (isCluttering(actual_rect))
        continue;
      drawOutlinedText(p, dte);
      text_rect_array.append(dte.rect);
      if (!canContinue())
        return false;
    }
  return true;
}

void KRender::render(QPainter* p, KMap* map, int render_idx)
{
  if (!map || render_idx > map->render_start_list.count() - 1)
    return;

  auto start        = map->render_start_list[render_idx];
  int  object_count = 0;

  auto render_frame_m = getDrawRectM();

  for (int layer_idx = start.layer_idx;
       layer_idx < KMap::max_layer_count; layer_idx++)
  {
    int start_obj_idx = 0;
    if (layer_idx == start.layer_idx)
      start_obj_idx = start.obj_idx;

    for (int obj_idx = start_obj_idx;
         obj_idx < map->render_data[layer_idx].count(); obj_idx++)
    {
      auto obj = map->render_data[layer_idx][obj_idx];
      object_count++;
      if (object_count == map->render_object_count)
      {
        if (render_idx == KMap::render_count - 1)
          paintObjects(p);
        return;
      }
      if (!obj)
        continue;
      if (obj->shape->id != selected_category_name)
        if (!checkMipRange(obj))
          continue;
      if (obj->tile_frame_m.isValid())
        if (!obj->tile_frame_m.intersects(render_frame_m))
          continue;

      if (!drawObject(p, obj, render_idx))
      {
        if (render_idx == KMap::render_count - 1)
          paintObjects(p);
        emit rendered(0);
        return;
      }
    }
  }
}

struct RenderEntry
{
  int            idx;
  QPixmap*       pm;
  QPainter*      p;
  QElapsedTimer* t;
  QFuture<void>* fut;
  RenderEntry(int idx, QSize s, QFont* f);
  ~RenderEntry();
};

RenderEntry::RenderEntry(int _idx, QSize size, QFont* f)
{
  idx = _idx;
  pm  = new QPixmap(size);
  pm->fill(Qt::transparent);
  p = new QPainter(pm);
  p->setFont(*f);
  t = new QElapsedTimer;
  t->start();
  fut = new QFuture<void>;
}

RenderEntry::~RenderEntry()
{
  delete p;
  delete pm;
  delete t;
}

void KRender::run()
{
  render_center_m = center_m;
  render_mip      = mip;
  point_object_text_rects.clear();

  for (int i = 0; i < KMap::render_count; i++)
  {
    draw_text_array[i].clear();
    name_holder_array[i].clear();
  }
  size_m            = {pixmap_size.width() * render_mip,
                       pixmap_size.height() * render_mip};
  render_top_left_m = {render_center_m.x() - size_m.width() / 2,
                       render_center_m.y() - size_m.height() / 2};
  render_frame_m    = {render_top_left_m, size_m};

  if (loading_enabled)
    checkLoad();

  yield_timer.start();

  getting_pixmap_enabled = false;

  QElapsedTimer total_render_time;
  total_render_time.start();

  if (render_pixmap.size() != pixmap_size)
    render_pixmap = QPixmap(pixmap_size);
  render_pixmap.fill(background_color);
  QPainter p0(&render_pixmap);
  QFont    f = p0.font();

  double font_size = 2.0 / KShape::pixel_size_mm / mip;
  font_size = std::clamp(font_size, 2.0 / KShape::pixel_size_mm,
                         3.0 / KShape::pixel_size_mm);
  f.setPixelSize(font_size);
  f.setBold(true);
  p0.setFont(f);

  KMap* exclusive_map = nullptr;

  auto top_left_pix = pix2deg({0, 0});
  auto bottom_right_pix =
      pix2deg({render_pixmap.width(), render_pixmap.height()});
  auto top_left_deg =
      QPointF{top_left_pix.longitude(), top_left_pix.latitude()};
  auto bottom_right_deg = QPointF{bottom_right_pix.longitude(),
                                  bottom_right_pix.latitude()};
  auto frame_deg =
      QRectF{top_left_deg, bottom_right_deg}.normalized();

  if (render_mip < KMap::only_global_mip)
  {
    QVector<QPointF> corners;
    corners << frame_deg.topLeft();
    corners << frame_deg.topRight();
    corners << frame_deg.bottomLeft();
    corners << frame_deg.bottomRight();
    for (auto& map: maps)
    {
      bool all_corners_inside = true;
      for (auto corner: corners)
        if (!map->containsPoint(corner))
        {
          all_corners_inside = false;
          break;
        }
      if (all_corners_inside)
      {
        exclusive_map = map;
        break;
      }
    }
  }

  for (int i = -1; auto& map: maps)
  {
    i++;
    if (exclusive_map)
      if (map != exclusive_map)
        continue;

    KLocker big_locker(&map->global_lock, KLocker::Read);
    if (!big_locker.hasLocked())
      continue;

    if (i > 0 && !needToLoadMap(map, render_frame_m))
      continue;

    auto map_rect_m = map->frame.toMeters();
    if (i > 0 && !render_frame_m.intersects(map_rect_m))
      continue;

    KLocker small_locker(&map->local_lock, KLocker::Read);
    if (!small_locker.hasLocked())
      continue;

    if (map->render_start_list.isEmpty())
      continue;

    QList<RenderEntry*> render_list;
    for (int render_idx = 1; render_idx < KMap::render_count;
         render_idx++)
    {
      auto render =
          new RenderEntry(render_idx, render_pixmap.size(), &f);
      *render->fut = QtConcurrent::run(this, &KRender::render,
                                       render->p, map, render_idx);
      render_list.append(render);
    }

    render(&p0, map, 0);

    for (auto render: render_list)
      render->fut->waitForFinished();

    for (auto render: render_list)
      p0.drawPixmap(0, 0, *render->pm);

    qDeleteAll(render_list);

    checkYieldResult();
  }

  QElapsedTimer t;
  t.start();

  if (!drawLineNames(&p0))
  {
    emit rendered(0);
    return;
  }
  if (!drawPolygonNames(&p0))
  {
    emit rendered(0);
    return;
  }
  qDebug() << "mip" << render_mip << ",total render time elapsed"
           << total_render_time.elapsed();

  getting_pixmap_enabled = true;
  emit rendered(0);
  if (loading_enabled)
    checkLoad();
}

void KRender::selectCategory(const QString& v)
{
  selected_category_name = v;
}

void KRender::start()
{
  if (isRunning())
    return;
  rendering_enabled = true;
  if (QThreadPool::globalInstance()->activeThreadCount() == 0)
    checkUnload();
  QThread::start();
}

void KRender::stopAndWait()
{
  rendering_enabled = false;
  wait();
}

void KRender::enableLoading(bool v)
{
  loading_enabled = v;
}

void KRender::pan(QPoint shift_pix)
{
  enableLoading(true);
  QPointF shift_m = shift_pix * mip;
  setCenterM(center_m + shift_m);
}

void KRender::zoom(double coef)
{
  mip *= coef;
}