#include "math.h"
#include "kmap.h"
#include "kserialize.h"
#include "klocker.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QDateTime>
#include <QRegularExpression>

namespace kmap
{
double deg2rad(double deg)
{
  return deg * M_PI / 180;
}
double rad2deg(double rad)
{
  return rad / M_PI * 180;
}
double getLength(QPoint p1, QPoint p2)
{
  return sqrt(pow(p1.x() - p2.x(), 2) + pow(p1.y() - p2.y(), 2));
}
double getAngle(QPoint p1, QPoint p2)
{
  return atan2(p2.y() - p1.y(), p2.x() - p1.x());
}
}

using namespace kmap;

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

KGeoCoor KGeoCoor::wrapped() const
{
  if (lon > KMap::wrap_longitude * 1E+7)
    return {lat, lon};
  else
    return {lat, int(lon + 3'600'000'000)};
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
    double y =
        log(tan(deg2rad(-latitude()) / 2 + M_PI / 4)) * earth_r;
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

  void KGeoPolygon::save(QByteArray & ba, int coor_precision_coef)
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
      int dlat = 1.0 * (point.lat - frame.top_left.lat) /
                 coor_precision_coef;
      int dlon = 1.0 * (point.lon - frame.top_left.lon) /
                 coor_precision_coef;
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

  void KObject::load(QVector<KShape*> * shape_list, int& pos,
                     const QByteArray& ba)
  {
    using namespace KSerialize;

    uchar has_name;
    read(ba, pos, has_name);
    if (has_name)
      read(ba, pos, name);
    uchar has_name_en;
    read(ba, pos, has_name_en);
    if (has_name_en)
      read(ba, pos, name_en);

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

  KGeoCoor KObject::getCenter()
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

  void KObject::save(const QVector<KShape*>* shape_list,
                     QByteArray&             ba)
  {
    using namespace KSerialize;
    write(ba, (uchar)(name.count() > 0));
    if (!name.isEmpty())
      write(ba, name);
    write(ba, (uchar)(name_en.count() > 0));
    if (!name_en.isEmpty())
      write(ba, name_en);

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

  KObject::~KObject()
  {
    qDeleteAll(polygons);
    polygons.clear();
  }

  KMap::KMap(const QString& v, double _min_mip, double _max_mip)
  {
    path    = v;
    min_mip = _min_mip;
    max_mip = _max_mip;
  }

  KMap::~KMap()
  {
    clear();
  }

  bool KMap::containsPoint(QPointF deg) const
  {
    for (auto border_deg: borders_deg)
      if (border_deg.containsPoint(deg, Qt::OddEvenFill))
        return true;
    return false;
  }

  void KMap::add(KMap * m)
  {
    frame = frame.united(m->frame);
    for (auto new_obj: m->global_tile)
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
      global_tile.append(new_obj);
    }
  }

  void KMap::clear()
  {
    KLocker big_locker(&global_lock, KLocker::Write);
    if (!big_locker.hasLocked())
      return;
    KLocker small_locker(&local_lock, KLocker::Write);
    if (!small_locker.hasLocked())
      return;

    if (global_tile.status != KObjectCollection::Loaded)
      return;
    if (global_tile.status == KObjectCollection::Loading)
      return;

    qDeleteAll(global_tile);
    global_tile.clear();
    for (auto& part: local_tiles)
    {
      if (part)
      {
        qDeleteAll(*part);
        delete part;
      }
    }
    local_tiles.clear();
    shapes.clear();
    qDeleteAll(shapes);
    for (int i = 0; i < max_layer_count; i++)
      render_data[i].clear();
    global_tile.status  = KObjectCollection::Null;
    render_object_count = 0;
    render_start_list.clear();
  }

  void KMap::save(Compression compression, QString new_path)
  {
    using namespace KSerialize;

    if (compression == Compression::Uncompressed)
      compression_level = 0;
    else
      compression_level = int(compression);

    auto _path = path;
    if (!new_path.isEmpty())
      _path = new_path;

    QFile f(_path);
    if (!f.open(QIODevice::WriteOnly))
    {
      qDebug() << "write error:" << path;
      return;
    }

    QReadLocker locker(&global_lock);
    write(&f, QString("kmap"));
    write(&f, compression_level);
    write(&f, frame);
    char has_borders = (borders.count() > 0);
    write(&f, has_borders);

    if (has_borders)
    {
      QByteArray ba;
      write(ba, borders.count());
      for (auto& border: borders)
        border.save(ba, border_coor_precision_coef);
      if (compression_level > 0)
        ba = qCompress(ba, compression_level);
      write(&f, ba.count());
      f.write(ba.data(), ba.count());
    }

    write(&f, local_load_mip);
    write(&f, shapes.count());
    for (auto& shape: shapes)
      shape->save(&f);

    QByteArray ba;
    write(ba, find_names);
    write(ba, find_centers);
    write(ba, find_names_en);
    write(ba, find_centers_en);
    if (compression_level > 0)
      ba = qCompress(ba, compression_level);
    write(&f, ba.count());
    f.write(ba.data(), ba.count());

    write(&f, global_tile.count());
    ba.clear();

    for (auto& obj: global_tile)
      obj->save(&shapes, ba);
    if (compression_level > 0)
      ba = qCompress(ba, compression_level);
    write(&f, ba.count());
    f.write(ba.data(), ba.count());
    write(&f, local_tiles.count());
    QList<qint64> small_part_pos_list;

    for (int part_idx = 0; auto& part: local_tiles)
    {
      small_part_pos_list.append(f.pos());
      if (part)
      {
        write(&f, part->count());
        ba.clear();
        for (auto& obj: *part)
          obj->save(&shapes, ba);
        if (compression_level)
          ba = qCompress(ba, compression_level);
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

  void KMap::loadGlobal(bool load_objects)
  {
    if (global_tile.status == KObjectCollection::Loading)
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

    if (global_tile.status != KObjectCollection::Null)
      return;

    QString format_id;
    read(&f, format_id);
    read(&f, compression_level);
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
      if (compression_level > 0)
        ba = qUncompress(ba);
      int pos = 0;
      int borders_count;
      read(ba, pos, borders_count);
      borders.resize(borders_count);
      for (auto& border: borders)
        border.load(ba, pos, border_coor_precision_coef);
      borders_deg.clear();
      for (auto border: borders)
      {
        QPolygonF border_deg;
        for (auto point: border)
          border_deg.append({point.longitude(), point.latitude()});
        if (!border_deg.isEmpty())
          borders_deg.append(border_deg);
      }
    }

    read(&f, local_load_mip);

    if (!load_objects)
      return;

    qDebug() << "loading global from" << path;
    global_tile.status = KObjectCollection::Loading;
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
    if (compression_level > 0)
      ba = qUncompress(ba);

    int pos = 0;
    read(ba, pos, find_names);
    read(ba, pos, find_centers);
    read(ba, pos, find_names_en);
    read(ba, pos, find_centers_en);

    int big_obj_count;
    read(&f, big_obj_count);
    global_tile.resize(big_obj_count);

    ba.clear();
    ba_count = 0;
    read(&f, ba_count);
    ba.resize(ba_count);
    f.read(ba.data(), ba_count);
    if (compression_level > 0)
      ba = qUncompress(ba);
    pos = 0;
    for (auto& obj: global_tile)
    {
      obj = new KObject;
      obj->load(&shapes, pos, ba);
    }
    int small_count;
    read(&f, small_count);
    local_tiles.resize(small_count);
    for (auto& part: local_tiles)
      part = nullptr;
    QWriteLocker big_locker(&global_lock);
    addCollectionToIndex(&global_tile);
    global_tile.status = KObjectCollection::Loaded;
    loaded();
  }

  void KMap::loadAll()
  {
    loadGlobal(true);
    for (int i = 0; i < local_tiles.count(); i++)
      loadLocal(i, QRect());
  }

  void KMap::loadLocal(int part_idx, QRectF tile_rect_m)
  {
    if (global_tile.status != KObjectCollection::Loaded)
      return;
    if (local_tiles[part_idx] &&
        local_tiles[part_idx]->status == KObjectCollection::Loading)
      return;

    qDebug() << "loading local" << part_idx << "from" << path;
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
    if (calc_small_part_count != local_tiles.count())
      return;

    if (part_idx > local_tiles.count() - 1)
      return;

    f.seek(small_idx_start_pos + part_idx * sizeof(qint64));
    qint64 part_pos;
    read(&f, part_pos);
    f.seek(part_pos);

    int part_obj_count;
    read(&f, part_obj_count);

    if (part_obj_count == 0)
    {
      if (!local_tiles[part_idx])
        local_tiles[part_idx] = new KObjectCollection;
      return;
    }

    if (!local_tiles[part_idx])
      local_tiles[part_idx] = new KObjectCollection;
    local_tiles[part_idx]->resize(part_obj_count);

    int ba_count = 0;
    read(&f, ba_count);
    QByteArray ba;
    ba.resize(ba_count);
    f.read(ba.data(), ba_count);
    if (compression_level > 0)
      ba = qUncompress(ba);

    local_tiles[part_idx]->status = KObjectCollection::Loading;
    int pos                       = 0;
    for (auto& obj: *local_tiles[part_idx])
    {
      obj = new KObject;
      obj->load(&shapes, pos, ba);
      obj->tile_frame_m = tile_rect_m;
    }
    QWriteLocker small_locker(&local_lock);
    addCollectionToIndex(local_tiles[part_idx]);
    local_tiles[part_idx]->status = KObjectCollection::Loaded;
    loaded();
  }

  KEditableMap::KEditableMap(const QString& path): KMap(path)
  {
  }

  void KEditableMap::addObjects(const QVector<KObject*>& obj_list,
                                int max_objects_per_tile)
  {
    int part_side_num =
        std::ceil(1.0 * obj_list.count() / max_objects_per_tile);
    int part_num = pow(part_side_num, 2);
    local_tiles.resize(part_num);
    for (auto& part: local_tiles)
      part = nullptr;
    auto map_size_m     = frame.getSizeMeters();
    auto map_top_left_m = frame.top_left.toMeters();
    for (auto& obj: obj_list)
    {
      if (obj->shape->max_mip == 0 ||
          obj->shape->max_mip > local_load_mip)
        global_tile.append(obj);
      else
      {
        auto   obj_top_left_m = obj->frame.top_left.toMeters();
        double shift_x_m = obj_top_left_m.x() - map_top_left_m.x();
        int    part_idx_x =
            1.0 * shift_x_m / map_size_m.width() * part_side_num;
        int shift_y = obj_top_left_m.y() - map_top_left_m.y();
        int part_idx_y =
            1.0 * shift_y / map_size_m.height() * part_side_num;
        int part_idx = part_idx_y * part_side_num + part_idx_x;
        if (!local_tiles[part_idx])
          local_tiles[part_idx] = new KObjectCollection;
        local_tiles[part_idx]->append(obj);
      }
    }
  }

  void KMap::addCollectionToIndex(const KObjectCollection* collection)
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

  KCategories KMapCollection::getCategories() const
  {
    KCategories ret;
    for (auto map: *this)
      for (auto shape: map->shapes)
        ret.append({shape->id, shape->image});
    return ret;
  }

  KGeoCoor KMapCollection::getCoorByName(const QString& object_name)
      const
  {
    for (auto map: *this)
    {
      auto idx = map->find_names.indexOf(object_name);
      if (idx >= 0)
      {
        auto ret = map->find_centers.at(idx);
        return ret;
      }
    }
    return KGeoCoor();
  }

  QStringList KMapCollection::find(const QString& str) const
  {
    QStringList ret;
    auto        pattern = "^" + str + ".*";
    auto        regexp  = QRegularExpression(pattern);
    regexp.setPatternOptions(
        QRegularExpression::CaseInsensitiveOption);
    for (auto map: *this)
      ret += map->find_names.filter(regexp);
    return ret;
  }

  KMapCollection::~KMapCollection()
  {
    qDeleteAll(*this);
  }