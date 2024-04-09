#include "math.h"
#include "krender.h"
#include "klocker.h"
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QPainterPath>
#include <numeric>

using namespace kmath;

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

void KRender::addMap(QString path, bool load_now)
{
  stopAndWait();
  insertMap(maps.count(), path, load_now);
}

void KRender::insertMap(int idx, QString path, bool load_now)
{
  auto map = new KRenderMap(path);
  connect(map, &KRenderMap::loaded, this, &KRender::onLoaded,
          Qt::UniqueConnection);
  map->loadMain(load_now);
  maps.insert(idx, map);
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

void KRender::setPixelSizeMM(double v)
{
  pixel_size_mm = v;
}

void KRender::setUpdateIntervalMs(int v)
{
  update_interval_ms = v;
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

const KRenderMapCollection* KRender::getMaps() const
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
    renderMap();
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
    if (map->getMain().status == KObjectCollection::Loaded)
    {
      if (!needToLoadMap(map, draw_rect_m))
        if (loaded_count > 1)
          map->clear();
      loaded_count++;
    }
  }
}

bool KRender::needToLoadMap(const KRenderMap* map,
                            const QRectF&     draw_rect_m)
{
  if (map->getMainMip() > 0 && render_mip > map->getMainMip())
    return false;
  auto map_rect_m       = map->getFrame().toMeters();
  bool frame_intersects = draw_rect_m.intersects(map_rect_m);
  if (!frame_intersects)
    return false;

  auto top_left_m     = pix2meters({0, 0});
  auto bottom_right_m = pix2meters({(double)render_pixmap.width(),
                                    (double)render_pixmap.height()});
  auto frame_m = QRectF{top_left_m, bottom_right_m}.normalized();
  frame_m.adjust(-10000, -10000, 10000, 10000);

  QPolygonF rect;
  rect << frame_m.topLeft();
  rect << frame_m.topRight();
  rect << frame_m.bottomLeft();
  rect << frame_m.bottomRight();
  if (map->intersects(rect))
    return true;
  return false;
}

void KRender::checkLoad()
{
  auto draw_rect_m = getDrawRectM();
  for (auto& map: maps)
  {
    auto map_rect_m = map->getFrame().toMeters();

    if (!needToLoadMap(map, draw_rect_m))
      continue;

    if (map->getMain().status == KObjectCollection::Null)
    {
      QtConcurrent::run(
          [&map]()
          {
            map->loadMain(true);
          });
      continue;
    }
    if (map->getMain().status == KObjectCollection::Loaded)
    {
      if (needToLoadMap(map, draw_rect_m))
      {
        int    tile_side_count = sqrt(map->getTiles().count());
        QSizeF tile_size_m = {map_rect_m.width() / tile_side_count,
                              map_rect_m.height() / tile_side_count};
        for (int tile_idx = 0; auto& tile: map->getTiles())
        {
          int    tile_idx_y = tile_idx / tile_side_count;
          int    tile_idx_x = tile_idx - tile_idx_y * tile_side_count;
          double tile_left =
              map_rect_m.x() + tile_idx_x * tile_size_m.width();
          double tile_top =
              map_rect_m.y() + tile_idx_y * tile_size_m.height();
          QRectF tile_rect_m = {{tile_left, tile_top}, tile_size_m};
          if (!tile && tile_rect_m.intersects(draw_rect_m) &&
              render_mip < map->getTileMip())
            QtConcurrent::run(map, &KRenderMap::loadTile, tile_idx,
                              tile_rect_m);
          tile_idx++;
        }
      }
    }
  }
}

void KRender::paintOutlinedText(QPainter* p, const QString& text,
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

void KRender::paintOutlinedText(QPainter* p, const DrawTextEntry& dte)
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
}

void KRender::addDrawTextEntry(
    QVector<DrawTextEntry>& draw_text_array, DrawTextEntry new_dte)
{
  bool can_fit = true;
  for (auto dte: draw_text_array)
  {
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

void KRender::paintPointObject(QPainter* p, const KMapObject* obj)
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

  auto w = obj->shape->getWidthPix();
  if (w > 0)
  {
    p->save();
    p->translate(pos);
    auto f = p->font();
    f.setPixelSize(w);
    p->setFont(f);
    p->translate(QPoint(0, obj->shape->getWidthPix() / 2 + 2));
    for (auto str: str_list)
    {
      p->translate(QPoint(0, text_shift));
      paintOutlinedText(p, str, sh->tcolor);
    }
    p->restore();
  }
  if (sh->image.isNull())
    p->drawEllipse(pos, 5, 5);
  else
  {
    pos = {pos.x() - sh->image.width() / 2,
           pos.y() - sh->image.height() / 2};
    p->drawImage(pos, sh->image);
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

void KRender::paintPolygonObject(QPainter* p, const KMapObject* obj,
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

    auto pl = poly2pix(*polygon);

    if ((polygon_idx == 0 && !obj->name.isEmpty() &&
         obj_span_pix < std::min(pixmap_size.width(),
                                 pixmap_size.height() / 2) &&
         obj_span_pix >
             max_object_size_with_name_mm / pixel_size_mm) ||
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
                        Qt::AlignCenter});
    }

    if (obj->polygons.count() == 1)
    {
      p->drawPolygon(pl);
      continue;
    }

    if (polygon_idx == 0)
      p->drawPolygon(pl);
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
    p->drawImage(pos, sh->image);
  }
}

void KRender::paintLineObject(QPainter*         painter,
                              const KMapObject* obj, int render_idx)
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
        painter->font().pixelSize() * obj->name.count() * 0.3;

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

    if (!obj->name.isEmpty() && poly_idx == 0)
      for (int point_idx = -1; auto p: pl)
      {
        point_idx++;
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
    if (sizeable_w > 7 && w > 0)
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

void KRender::NameHolder::fix(const KMapObject* _obj,
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

bool KRender::checkMipRange(const KMapObject* obj)
{
  return (obj->shape->min_mip == 0 ||
          render_mip >= obj->shape->min_mip) &&
         (obj->shape->max_mip == 0 ||
          render_mip <= obj->shape->max_mip);
}

bool KRender::paintObject(QPainter* p, const KMapObject* obj,
                          int render_idx)
{
  switch (obj->shape->type)
  {
  case KShape::Point:
    paintPointObject(p, obj);
    break;
  case KShape::Line:
    paintLineObject(p, obj, render_idx);
    break;
  case KShape::Polygon:
    paintPolygonObject(p, obj, render_idx);
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

bool KRender::paintLineNames(QPainter* p)
{
  text_rect_array.clear();
  for (int render_idx = 0; render_idx < KRenderMap::render_count;
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
      paintOutlinedText(p, nh.obj->name, nh.obj->shape->tcolor);
      p->restore();
      if (!canContinue())
        return false;
    }
  return true;
}

bool KRender::paintPolygonNames(QPainter* p)
{
  for (int render_idx = 0; render_idx < KRenderMap::render_count;
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
      paintOutlinedText(p, dte);
      text_rect_array.append(dte.rect);
      if (!canContinue())
        return false;
    }
  return true;
}

void KRender::render(QPainter* p, QVector<KRenderMap*> render_maps,
                     int render_idx)
{
  for (auto map: render_maps)
  {
    KLocker main_locker(&map->main_lock, KLocker::Read);
    if (!main_locker.hasLocked())
      continue;
    KLocker tile_locker(&map->tile_lock, KLocker::Read);
    if (!tile_locker.hasLocked())
      continue;
    renderMap(p, map, render_idx);
  }
}

void KRender::renderMap(QPainter* p, KRenderMap* map, int render_idx)
{
  if (!map || render_idx > map->render_start_list.count() - 1)
    return;

  auto start        = map->render_start_list[render_idx];
  int  object_count = 0;

  auto render_frame_m = getDrawRectM();

  for (int layer_idx = start.layer_idx;
       layer_idx < KRenderMap::max_layer_count; layer_idx++)
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
        if (render_idx == KRenderMap::render_count - 1)
          paintUserObjects(p);
        return;
      }
      if (!obj)
        continue;
      if (!checkMipRange(obj))
        continue;
      if (obj->tile_frame_m.isValid())
        if (!obj->tile_frame_m.intersects(render_frame_m))
          continue;

      if (!paintObject(p, obj, render_idx))
      {
        if (render_idx == KRenderMap::render_count - 1)
          paintUserObjects(p);
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

  for (int i = 0; i < KRenderMap::render_count; i++)
  {
    draw_text_array[i].clear();
    name_holder_array[i].clear();
  }
  size_m            = {pixmap_size.width() * render_mip,
                       pixmap_size.height() * render_mip};
  render_top_left_m = {render_center_m.x() - size_m.width() / 2,
                       render_center_m.y() - size_m.height() / 2};
  render_frame_m    = {render_top_left_m, size_m};

  started(render_frame_m);

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

  double font_size =
      std::min((int)std::round(1.5 / pixel_size_mm / mip), 1);
  font_size =
      std::clamp(font_size, 1.5 / pixel_size_mm, 3.0 / pixel_size_mm);
  f.setPixelSize(font_size);
  f.setBold(true);
  p0.setFont(f);

  QVector<int> intersecting_maps;
  auto         draw_rect = getDrawRectM();
  for (int map_idx = -1; auto& map: maps)
  {
    if (map->getMainMip() > 0 && render_mip > map->getMainMip())
      continue;
    map_idx++;
    if (map_idx == 0)
      continue;

    if (needToLoadMap(map, draw_rect))
      intersecting_maps.append(map_idx);
  }

  QVector<KRenderMap*> render_maps;
  for (int map_idx = -1; auto& map: maps)
  {
    map_idx++;

    if (map_idx > 0)
      if (!intersecting_maps.contains(map_idx))
        continue;

    KLocker big_locker(&map->main_lock, KLocker::Read);
    if (!big_locker.hasLocked())
      continue;

    if (map_idx > 0 && !needToLoadMap(map, render_frame_m))
      continue;

    auto map_rect_m = map->getFrame().toMeters();
    if (map_idx > 0 && !render_frame_m.intersects(map_rect_m))
      continue;

    KLocker small_locker(&map->tile_lock, KLocker::Read);
    if (!small_locker.hasLocked())
      continue;

    if (map->render_start_list.isEmpty())
      continue;

    render_maps.append(map);
  }

  QList<RenderEntry*> render_list;
  for (int render_idx = 1; render_idx < KRenderMap::render_count;
       render_idx++)
  {
    auto render =
        new RenderEntry(render_idx, render_pixmap.size(), &f);
    *render->fut = QtConcurrent::run(
        this, &KRender::render, render->p, render_maps, render_idx);
    render_list.append(render);
  }

  render(&p0, render_maps, 0);

  for (auto render: render_list)
    render->fut->waitForFinished();

  for (auto render: render_list)
    p0.drawPixmap(0, 0, *render->pm);

  qDeleteAll(render_list);

  checkYieldResult();

  QElapsedTimer t;
  t.start();

  if (!paintLineNames(&p0))
  {
    emit rendered(0);
    return;
  }
  if (!paintPolygonNames(&p0))
  {
    emit rendered(0);
    return;
  }
  qDebug() << "mip" << render_mip << ",total render time elapsed"
           << total_render_time.elapsed();

  getting_pixmap_enabled = true;
  main_pixmap            = render_pixmap;
  emit rendered(0);
  if (loading_enabled)
    checkLoad();
}

void KRender::renderUserObjects()
{
  render_pixmap = main_pixmap;
  QPainter p(&render_pixmap);
  paintUserObjects(&p);
  rendered(0);
}

void KRender::renderMap()
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
