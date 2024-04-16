#include "kmath.h"
#include "kpack.h"
#include "kserialize.h"
#include "klocker.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QDateTime>
#include <QRegularExpression>

using namespace kmath;

namespace kmath
{
double deg2rad(double deg)
{
  return deg * M_PI / 180;
}
double rad2deg(double rad)
{
  return rad / M_PI * 180;
}
double sqr(double x)
{
  return x * x;
}
double getLength(QPoint p1, QPoint p2)
{
  return sqrt(sqr(p1.x() - p2.x()) + sqr(p1.y() - p2.y()));
}
double getAngle(QPoint p1, QPoint p2)
{
  return atan2(p2.y() - p1.y(), p2.x() - p1.x());
}
bool isNearPolyline(QPoint p0, QPolygon polyline, int tolerance_pix)
{
  QPoint p1 = polyline.first();
  if (getLength(p0, p1) < tolerance_pix)
    return true;
  int x0 = p0.x();
  int y0 = p0.y();
  for (int idx = -1; auto p2: polyline)
  {
    idx++;
    if (idx == 0)
      continue;

    if (getLength(p0, p2) < tolerance_pix)
      return true;

    int x1 = p1.x();
    int y1 = p1.y();
    int x2 = p2.x();
    int y2 = p2.y();

    p1 = p2;

    double l  = sqrt(sqr(x2 - x1) + sqr(y2 - y1));
    double pr = (x0 - x1) * (x2 - x1) + (y0 - y1) * (y2 - y1);
    double cf = pr / l;
    if (cf < 0 || cf > 1)
      continue;

    double d =
        fabs((x2 - x1) * (y1 - y0) - (x1 - x0) * (y2 - y1)) / l;

    if (d < tolerance_pix)
      return true;
  }
  return false;
}
}

double KGeoCoor::longitude() const
{
  return lon * 1E-7;
}

double KGeoCoor::latitude() const
{
  return -lat * 1E-7;
}

bool KGeoCoor::isValid()
{
  return !(lat == 0 && lon == 0);
}

bool KGeoCoor::needToWrap()
{
  return longitude() < wrap_longitude;
}

KGeoCoor KGeoCoor::wrapped() const
{
  if (lon > wrap_longitude * 1E+7)
    return {lat, lon};
  else
    return {lat, int(lon + 3600000000)};
}

KGeoCoor KGeoCoor::inc(KGeoCoor step) const
{
  KGeoCoor ret = *this;
  ret.lat += step.lat;
  ret.lon += step.lon;
  return ret;
}

KGeoCoor::KGeoCoor()
{
  lat = 0;
  lon = 0;
}

KGeoCoor::KGeoCoor(int _lat, int _lon)
{
  lat = _lat;
  lon = _lon;
}

KGeoCoor KGeoCoor::fromDegs(double lat, double lon)
{
  KGeoCoor ret;
  ret.lon = round(lon * 1E+7);
  ret.lat = -round(lat * 1E+7);
  return ret;
}
KGeoCoor KGeoCoor::fromMeters(QPointF m)
{
  double lon_deg = rad2deg(m.x() / earth_r);
  double lat_deg =
      -rad2deg(2 * atan(exp(m.y() / earth_r)) - M_PI / 2);
  return fromDegs(lat_deg, lon_deg);
}

QPointF KGeoCoor::toMeters() const
{
  double x = deg2rad(longitude()) * earth_r;
  double y = log(tan(deg2rad(-latitude()) / 2 + M_PI / 4)) * earth_r;
  return {x, y};
}

KGeoRect KGeoRect::united(const KGeoRect& v) const
{
  KGeoRect res;
  res.top_left.lon = std::min(top_left.lon, v.top_left.lon);
  res.top_left.lat = std::min(top_left.lat, v.top_left.lat);
  res.bottom_right.lon =
      std::max(bottom_right.lon, v.bottom_right.lon);
  res.bottom_right.lat =
      std::max(bottom_right.lat, v.bottom_right.lat);
  return res;
}

bool KGeoRect::isNull() const
{
  return top_left.lon == bottom_right.lon &&
         top_left.lat == bottom_right.lat;
}

QRectF KGeoRect::toMeters() const
{
  return {top_left.toMeters(), bottom_right.toMeters()};
}

QSizeF KGeoRect::getSizeMeters() const
{
  auto   top_left_m     = top_left.toMeters();
  auto   bottom_right_m = bottom_right.toMeters();
  double w              = bottom_right_m.x() - top_left_m.x();
  double h              = bottom_right_m.y() - top_left_m.y();
  return {w, h};
}

QRectF KGeoRect::toRectM() const
{
  return {top_left.toMeters(), bottom_right.toMeters()};
}

KGeoRect KGeoPolygon::getFrame()
{
  using namespace std;
  auto minx = numeric_limits<int>().max();
  auto miny = numeric_limits<int>().max();
  auto maxx = numeric_limits<int>().min();
  auto maxy = numeric_limits<int>().min();
  for (auto& p: *this)
  {
    minx = min(p.lon, minx);
    miny = min(p.lat, miny);
    maxx = max(p.lon, maxx);
    maxy = max(p.lat, maxy);
  }
  KGeoRect rect;
  rect.top_left.lon     = minx;
  rect.top_left.lat     = miny;
  rect.bottom_right.lon = maxx;
  rect.bottom_right.lat = maxy;
  return rect;
}

void KGeoPolygon::save(QByteArray& ba, int coor_precision_coef)
{
  using namespace KSerialize;
  write(ba, count());

  if (count() == 1)
  {
    write(ba, first().lat);
    write(ba, first().lon);
    return;
  }
  if (count() == 2)
  {
    write(ba, first().lat);
    write(ba, first().lon);
    write(ba, last().lat);
    write(ba, last().lon);
    return;
  }

  auto frame = getFrame();
  write(ba, frame.top_left);
  int span_lat =
      std::max(1, frame.bottom_right.lat - frame.top_left.lat);
  int span_lon =
      std::max(1, frame.bottom_right.lon - frame.top_left.lon);

  span_lat /= coor_precision_coef;
  span_lon /= coor_precision_coef;

  int span_type = 0;

  if (span_lat <= 0xff && span_lon <= 0xff)
    span_type = 0;
  else if (span_lat <= 0xffff && span_lon <= 0xffff)
    span_type = 1;
  else
    span_type = 2;

  if (span_lat == 0 || span_lon == 0)
    span_type = 2;

  write(ba, (uchar)span_type);

  if (span_type == 2)
  {
    for (auto& point: (*this))
      write(ba, point);
    return;
  }

  for (auto& point: (*this))
  {
    int dlat =
        1.0 * (point.lat - frame.top_left.lat) / coor_precision_coef;
    int dlon =
        1.0 * (point.lon - frame.top_left.lon) / coor_precision_coef;
    if (span_type == 0)
    {
      write(ba, (uchar)dlat);
      write(ba, (uchar)dlon);
    }
    else
    {
      write(ba, (ushort)dlat);
      write(ba, (ushort)dlon);
    }
  }
}

QPolygonF KGeoPolygon::toPolygonM()
{
  QPolygonF ret;
  for (auto p: *this)
    ret.append(p.toMeters());
  return ret;
}

void KGeoPolygon::load(const QByteArray& ba, int& pos,
                       int coor_precision_coef)
{
  using namespace KSerialize;
  int point_count;
  read(ba, pos, point_count);
  resize(point_count);

  if (count() <= 2)
  {
    for (auto& p: *this)
      read(ba, pos, p);
    return;
  }

  KGeoCoor top_left;
  read(ba, pos, top_left);

  uchar span_type;
  read(ba, pos, span_type);

  if (span_type == 2)
  {
    for (auto& point: (*this))
      read(ba, pos, point);
    return;
  }

  for (auto& point: (*this))
  {
    int dlat = 0;
    int dlon = 0;

    if (span_type == 0)
    {
      uchar _dlat = 0;
      uchar _dlon = 0;
      read(ba, pos, _dlat);
      read(ba, pos, _dlon);
      dlat = _dlat;
      dlon = _dlon;
    }
    else
    {
      ushort _dlat = 0;
      ushort _dlon = 0;
      read(ba, pos, _dlat);
      read(ba, pos, _dlon);
      dlat = _dlat;
      dlon = _dlon;
    }
    dlat *= coor_precision_coef;
    dlon *= coor_precision_coef;
    point.lat = top_left.lat + dlat;
    point.lon = top_left.lon + dlon;
  }
}

void KPackObject::load(QVector<KShape*>* shape_list, int& pos,
                       const QByteArray& ba)
{
  using namespace KSerialize;

  uchar has_name;
  read(ba, pos, has_name);
  if (has_name)
    read(ba, pos, name);

  read(ba, pos, attributes);

  int shape_idx;
  read(ba, pos, shape_idx);
  shape = (*shape_list)[shape_idx];

  uchar is_multi_polygon;
  read(ba, pos, is_multi_polygon);

  uchar is_point;
  read(ba, pos, is_point);

  if (is_point)
  {
    KGeoCoor p;
    memcpy((char*)&p, (char*)&ba.data()[pos], sizeof(p));
    pos += sizeof(p);
    auto polygon = new KGeoPolygon;
    polygon->append(p);
    polygons.append(polygon);
    frame.top_left     = p;
    frame.bottom_right = p;
    return;
  }

  if (is_multi_polygon)
  {
    int polygon_count;
    read(ba, pos, polygon_count);
    polygons.resize(polygon_count);
    for (std::size_t i = 0; auto& polygon: polygons)
    {
      polygon = new KGeoPolygon;
      polygon->load(ba, pos, shape->coor_precision_coef);
      if (i++ == 0)
        frame = polygon->getFrame();
      else
        frame = frame.united(polygon->getFrame());
    }
  }
  else
  {
    polygons.resize(1);
    auto polygon = new KGeoPolygon;
    polygon->load(ba, pos, shape->coor_precision_coef);
    polygons[0] = polygon;
    frame       = polygon->getFrame();
  }
}

KGeoCoor KPackObject::getCenter()
{
  if (polygons.isEmpty())
    return KGeoCoor();
  auto   polygon = polygons.first();
  auto   frame   = polygon->getFrame();
  auto   tl      = frame.top_left;
  auto   br      = frame.bottom_right;
  double lat     = (tl.latitude() + br.latitude()) * 0.5;
  double lon     = (tl.longitude() + br.longitude()) * 0.5;
  if (fabs(lon - tl.longitude()) > 90)
    lon += 180;
  return KGeoCoor().fromDegs(lat, lon);
}

void KPackObject::save(const QVector<KShape*>* shape_list,
                       QByteArray&             ba)
{
  using namespace KSerialize;
  write(ba, (uchar)(name.count() > 0));
  if (!name.isEmpty())
    write(ba, name);

  write(ba, attributes);

  auto shape_idx = shape_list->indexOf(shape);
  write(ba, shape_idx);
  write(ba, (uchar)(polygons.count() > 1));
  write(ba, (uchar)(frame.isNull()));

  if (frame.isNull())
  {
    write(ba, polygons.first()->first());
    return;
  }

  if (polygons.count() == 1)
    polygons[0]->save(ba, shape->coor_precision_coef);
  else
  {
    write(ba, polygons.count());
    for (auto& polygon: polygons)
      polygon->save(ba, shape->coor_precision_coef);
  }
}

KPackObject::~KPackObject()
{
  qDeleteAll(polygons);
  polygons.clear();
}

KPack::KPack(const QString& v)
{
  path = v;
}

KPack::~KPack()
{
  clear();
}

void KPack::setMainMip(double v)
{
  main_mip = v;
}

double KPack::getMainMip() const
{
  return main_mip;
}

void KPack::setTileMip(double v)
{
  tile_mip = v;
}

double KPack::getTileMip() const
{
  return tile_mip;
}

void KPack::setFrame(KGeoRect v)
{
  frame = v;
}

const KGeoRect& KPack::getFrame() const
{
  return frame;
}

const KObjectCollection& KPack::getMain() const
{
  return main;
}

const QVector<KObjectCollection*> KPack::getTiles() const
{
  return tiles;
}

bool KRenderMap::intersects(QPolygonF polygon_m) const
{
  if (borders_m.isEmpty())
    return polygon_m.intersects(frame.toRectM());
  for (auto border_m: borders_m)
    if (border_m.intersects(polygon_m))
      return true;
  return false;
}

void KPack::add(KPack* m)
{
  frame = frame.united(m->frame);
  for (auto new_obj: m->main)
  {
    auto new_sh = new_obj->shape;
    bool found  = false;
    for (auto sh: shapes)
      if (new_sh->id == sh->id)
      {
        new_obj->shape = sh;
        found          = true;
        break;
      }
    if (!found)
      qDebug() << "ERROR: shape" << new_sh->id
               << "not found in primary classifier!";
    main.append(new_obj);
  }
}

void KPack::clear()
{
  if (main.status != KObjectCollection::Loaded)
    return;
  if (main.status == KObjectCollection::Loading)
    return;

  qDeleteAll(main);
  main.clear();
  for (auto& tile: tiles)
  {
    if (tile)
    {
      qDeleteAll(*tile);
      delete tile;
    }
  }
  tiles.clear();
  shapes.clear();
  qDeleteAll(shapes);
  main.status = KObjectCollection::Null;
}

void KPack::save(QString new_path) const
{
  using namespace KSerialize;

  auto _path = path;
  if (!new_path.isEmpty())
    _path = new_path;

  QFile f(_path);
  if (!f.open(QIODevice::WriteOnly))
  {
    qDebug() << "write error:" << path;
    return;
  }

  write(&f, QString("kpack"));
  write(&f, frame);
  char has_borders = (borders.count() > 0);
  write(&f, has_borders);

  if (has_borders)
  {
    QByteArray ba;
    write(ba, borders.count());
    for (auto border: borders)
      border.save(ba, border_coor_precision_coef);
    ba = qCompress(ba, 9);
    write(&f, ba.count());
    f.write(ba.data(), ba.count());
  }

  write(&f, main_mip);
  write(&f, tile_mip);
  write(&f, shapes.count());
  for (auto& shape: shapes)
    shape->save(&f);

  QByteArray ba;
  ba = qCompress(ba, 9);
  write(&f, ba.count());
  f.write(ba.data(), ba.count());

  write(&f, main.count());
  ba.clear();

  for (auto& obj: main)
    obj->save(&shapes, ba);
  ba = qCompress(ba, 9);
  write(&f, ba.count());
  f.write(ba.data(), ba.count());
  write(&f, tiles.count());
  QList<qint64> small_part_pos_list;

  for (int part_idx = 0; auto& part: tiles)
  {
    small_part_pos_list.append(f.pos());
    if (part)
    {
      write(&f, part->count());
      ba.clear();
      for (auto& obj: *part)
        obj->save(&shapes, ba);
      ba = qCompress(ba, 9);
      write(&f, ba.count());
      f.write(ba.data(), ba.count());
    }
    else
      write(&f, 0);
    part_idx++;
  }
  auto small_idx_start_pos = f.pos();
  for (auto& pos: small_part_pos_list)
    write(&f, pos);
  write(&f, small_idx_start_pos);
}

void KPack::loadMain(bool load_objects)
{
  if (main.status == KObjectCollection::Loading)
    return;

  QElapsedTimer t;
  t.start();

  using namespace KSerialize;
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly))
  {
    qDebug() << "read error:" << path;
    return;
  }

  if (main.status != KObjectCollection::Null)
    return;

  QString format_id;
  read(&f, format_id);
  read(&f, frame);

  char has_borders = false;
  read(&f, has_borders);
  if (has_borders)
  {
    QByteArray ba;
    int        ba_count = 0;
    read(&f, ba_count);
    ba.resize(ba_count);
    f.read(ba.data(), ba_count);
    ba      = qUncompress(ba);
    int pos = 0;
    int borders_count;
    read(ba, pos, borders_count);
    borders.resize(borders_count);
    for (auto& border: borders)
      border.load(ba, pos, border_coor_precision_coef);
    borders_m.clear();
    for (auto border: borders)
    {
      QPolygonF border_m = border.toPolygonM();
      if (!border_m.isEmpty())
        borders_m.append(border_m);
    }
  }

  read(&f, main_mip);
  read(&f, tile_mip);

  if (!load_objects)
    return;

  qDebug() << "loading main from" << path;
  main.status = KObjectCollection::Loading;
  int shape_count;
  read(&f, shape_count);
  shapes.resize(shape_count);
  for (auto& shape: shapes)
  {
    shape = new KShape;
    shape->load(&f);
  }

  int ba_count = 0;
  read(&f, ba_count);
  QByteArray ba;
  ba.resize(ba_count);
  f.read(ba.data(), ba_count);
  ba = qUncompress(ba);

  int pos = 0;

  int big_obj_count;
  read(&f, big_obj_count);
  main.resize(big_obj_count);

  ba.clear();
  ba_count = 0;
  read(&f, ba_count);
  ba.resize(ba_count);
  f.read(ba.data(), ba_count);
  ba  = qUncompress(ba);
  pos = 0;
  for (auto& obj: main)
  {
    obj = new KPackObject;
    obj->load(&shapes, pos, ba);
  }
  int small_count;
  read(&f, small_count);
  tiles.resize(small_count);
  for (auto& part: tiles)
    part = nullptr;
}

void KPack::loadAll()
{
  loadMain(true);
  for (int i = 0; i < tiles.count(); i++)
    loadTile(i, QRect());
}

void KPack::loadTile(int tile_idx, QRectF tile_rect_m)
{
  if (main.status != KObjectCollection::Loaded)
    return;
  if (tiles[tile_idx] &&
      tiles[tile_idx]->status == KObjectCollection::Loading)
    return;

  qDebug() << "loading tile" << tile_idx << "from" << path;
  QElapsedTimer t;
  t.start();
  using namespace KSerialize;
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly))
  {
    qDebug() << "read error:" << path;
    return;
  }

  qint64 small_idx_start_pos;
  f.seek(f.size() - sizeof(qint64));
  read(&f, small_idx_start_pos);

  int calc_small_part_count =
      (f.size() - sizeof(qint64) - small_idx_start_pos) /
      sizeof(qint64);
  if (calc_small_part_count != tiles.count())
    return;

  if (tile_idx > tiles.count() - 1)
    return;

  f.seek(small_idx_start_pos + tile_idx * sizeof(qint64));
  qint64 part_pos;
  read(&f, part_pos);
  f.seek(part_pos);

  int part_obj_count;
  read(&f, part_obj_count);

  if (part_obj_count == 0)
  {
    if (!tiles[tile_idx])
      tiles[tile_idx] = new KObjectCollection;
    return;
  }

  if (!tiles[tile_idx])
    tiles[tile_idx] = new KObjectCollection;
  tiles[tile_idx]->resize(part_obj_count);

  int ba_count = 0;
  read(&f, ba_count);
  QByteArray ba;
  ba.resize(ba_count);
  f.read(ba.data(), ba_count);
  ba = qUncompress(ba);

  tiles[tile_idx]->status = KObjectCollection::Loading;
  int pos                 = 0;
  for (auto& obj: *tiles[tile_idx])
  {
    obj = new KPackObject;
    obj->load(&shapes, pos, ba);
    obj->tile_frame_m = tile_rect_m;
  }
}

KRenderMap::KRenderMap(const QString& path): KPack(path)
{
}

KRenderMap::~KRenderMap()
{
  clear();
}

void KRenderMap::clear()
{
  KLocker big_locker(&main_lock, KLocker::Write);
  if (!big_locker.hasLocked())
    return;
  KLocker small_locker(&tile_lock, KLocker::Write);
  if (!small_locker.hasLocked())
    return;
  KPack::clear();
  for (int i = 0; i < max_layer_count; i++)
    render_data[i].clear();
  render_object_count = 0;
  render_start_list.clear();
}

void KRenderMap::loadMain(bool load_objects)
{
  KPack::loadMain(load_objects);
  if (load_objects)
  {
    QWriteLocker big_locker(&main_lock);
    addCollectionToIndex(&main);
    main.status = KObjectCollection::Loaded;
    loaded();
  }
}

void KRenderMap::loadTile(int tile_idx, QRectF tile_rect_m)
{
  KPack::loadTile(tile_idx, tile_rect_m);
  QWriteLocker small_locker(&tile_lock);
  addCollectionToIndex(tiles[tile_idx]);
  tiles[tile_idx]->status = KObjectCollection::Loaded;
  loaded();
}

KEditablePack::KEditablePack(const QString& path): KPack(path)
{
}

void KEditablePack::setShapes(QVector<KShape*> v)
{
  shapes = v;
}

void KEditablePack::addBorder(KGeoPolygon v)
{
  borders.append(v);
}

void KEditablePack::addObjects(const QVector<KPackObject*>& obj_list,
                               int max_objects_per_tile)
{
  int tile_side_num =
      std::ceil(1.0 * obj_list.count() / max_objects_per_tile);
  int tile_num = pow(tile_side_num, 2);
  tiles.resize(tile_num);
  for (auto& tile: tiles)
    tile = nullptr;
  auto map_size_m     = getFrame().getSizeMeters();
  auto map_top_left_m = getFrame().top_left.toMeters();
  for (auto& obj: obj_list)
  {
    if (obj->shape->max_mip == 0 ||
        obj->shape->max_mip > getTileMip())
      main.append(obj);
    else
    {
      auto   obj_top_left_m = obj->frame.top_left.toMeters();
      double shift_x_m      = obj_top_left_m.x() - map_top_left_m.x();
      int    part_idx_x =
          1.0 * shift_x_m / map_size_m.width() * tile_side_num;
      int shift_y = obj_top_left_m.y() - map_top_left_m.y();
      int part_idx_y =
          1.0 * shift_y / map_size_m.height() * tile_side_num;
      int part_idx = part_idx_y * tile_side_num + part_idx_x;
      if (!tiles[part_idx])
        tiles[part_idx] = new KObjectCollection;
      tiles[part_idx]->append(obj);
    }
  }
}

void KRenderMap::addCollectionToIndex(
    const KObjectCollection* collection)
{
  for (auto& obj: *collection)
    render_data[obj->shape->layer].append(obj);

  int total_object_count = 0;
  for (auto layer: render_data)
    total_object_count += layer.count();

  render_object_count = total_object_count / render_count;

  int curr_obj_count = 0;
  render_start_list.clear();
  render_start_list += {0, 0};
  for (int layer_idx = -1; auto layer: render_data)
  {
    layer_idx++;
    for (int object_idx = -1; auto obj: layer)
    {
      object_idx++;
      Q_UNUSED(obj)
      curr_obj_count++;
      if (curr_obj_count >= render_object_count)
      {
        render_start_list += {layer_idx, object_idx};
        curr_obj_count = 0;
      }
    }
  }
}

KRenderMapCollection::~KRenderMapCollection()
{
  qDeleteAll(*this);
}
