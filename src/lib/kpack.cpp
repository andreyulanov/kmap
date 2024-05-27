#include "math.h"
#include "kpack.h"
#include "kserialize.h"
#include "klocker.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QDateTime>
#include <QRegularExpression>

void KPackObject::load(QVector<KClass*>* class_list, int& pos,
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
  cl = (*class_list)[shape_idx];

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
      polygon->load(ba, pos, cl->coor_precision_coef);
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
    polygon->load(ba, pos, cl->coor_precision_coef);
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

void KPackObject::save(const QVector<KClass*>* class_list,
                       QByteArray&             ba)
{
  using namespace KSerialize;
  write(ba, (uchar)(name.count() > 0));
  if (!name.isEmpty())
    write(ba, name);

  write(ba, attributes);

  auto shape_idx = class_list->indexOf(cl);
  write(ba, shape_idx);
  write(ba, (uchar)(polygons.count() > 1));
  write(ba, (uchar)(frame.isNull()));

  if (frame.isNull())
  {
    write(ba, polygons.first()->first());
    return;
  }

  if (polygons.count() == 1)
    polygons[0]->save(ba, cl->coor_precision_coef);
  else
  {
    write(ba, polygons.count());
    for (auto& polygon: polygons)
      polygon->save(ba, cl->coor_precision_coef);
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

bool KRenderPack::intersects(QPolygonF polygon_m) const
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
    auto new_sh = new_obj->cl;
    bool found  = false;
    for (auto sh: classes)
      if (new_sh->id == sh->id)
      {
        new_obj->cl = sh;
        found       = true;
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
  classes.clear();
  qDeleteAll(classes);
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
  write(&f, classes.count());
  for (auto& shape: classes)
    shape->save(&f);

  QByteArray ba;
  ba = qCompress(ba, 9);
  write(&f, ba.count());
  f.write(ba.data(), ba.count());

  write(&f, main.count());
  ba.clear();

  for (auto& obj: main)
    obj->save(&classes, ba);
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
        obj->save(&classes, ba);
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

void KPack::loadMain(bool load_objects, double pixel_size_mm)
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
  classes.resize(shape_count);
  for (auto& shape: classes)
  {
    shape = new KClass;
    shape->load(&f, pixel_size_mm);
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
    obj->load(&classes, pos, ba);
  }
  int small_count;
  read(&f, small_count);
  tiles.resize(small_count);
  for (auto& part: tiles)
    part = nullptr;
}

void KPack::loadAll(double pixel_size_mm)
{
  loadMain(true, pixel_size_mm);
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
    obj->load(&classes, pos, ba);
    obj->tile_frame_m = tile_rect_m;
  }
}

KRenderPack::KRenderPack(const QString& path): KPack(path)
{
}

KRenderPack::~KRenderPack()
{
  clear();
}

void KRenderPack::clear()
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

void KRenderPack::loadMain(bool load_objects, double pixel_size_mm)
{
  KPack::loadMain(load_objects, pixel_size_mm);
  if (load_objects)
  {
    QWriteLocker big_locker(&main_lock);
    addCollectionToIndex(&main);
    main.status = KObjectCollection::Loaded;
    loaded();
  }
}

void KRenderPack::loadTile(int tile_idx, QRectF tile_rect_m)
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

void KEditablePack::setClasses(QVector<KClass*> v)
{
  classes = v;
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
    if (obj->cl->max_mip == 0 || obj->cl->max_mip > getTileMip())
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

void KRenderPack::addCollectionToIndex(
    const KObjectCollection* collection)
{
  for (auto& obj: *collection)
    render_data[obj->cl->layer].append(obj);

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

KRenderPackCollection::~KRenderPackCollection()
{
  qDeleteAll(*this);
}
