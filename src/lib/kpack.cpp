#include "math.h"
#include "kpack.h"
#include "kserialize.h"
#include "klocker.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QDateTime>
#include <QRegularExpression>

void KPackObject::load(QVector<KClass>& class_list, int& pos,
                       const QByteArray& ba)
{
  using namespace KSerialize;

  uchar has_name;
  read(ba, pos, has_name);
  if (has_name)
    read(ba, pos, name);

  read(ba, pos, attributes);

  read(ba, pos, class_idx);
  auto cl = &class_list[class_idx];

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

int KPackObject::getClassIdx() const
{
  return class_idx;
}

void KPackObject::setClassIdx(int v)
{
  class_idx = v;
}

QString KPackObject::getName() const
{
  return name;
}

void KPackObject::setName(QString v)
{
  name = v;
}

QMap<QString, QByteArray> KPackObject::getAttributes() const
{
  return attributes;
}

void KPackObject::addAttribute(QString k, QByteArray v)
{
  attributes.insert(k, v);
}

KGeoRect KPackObject::getFrame() const
{
  return frame;
}

void KPackObject::setFrame(KGeoRect v)
{
  frame = v;
}

const QVector<KGeoPolygon*>& KPackObject::getPolygons() const
{
  return polygons;
}

void KPackObject::removePolygonAt(int idx)
{
  delete polygons.at(idx);
  polygons.removeAt(idx);
}

void KPackObject::addPolygon(KGeoPolygon* v)
{
  polygons.append(v);
}

void KPackObject::save(const QVector<KClass>& class_list,
                       QByteArray&            ba)
{
  using namespace KSerialize;
  write(ba, (uchar)(name.count() > 0));
  if (!name.isEmpty())
    write(ba, name);

  write(ba, attributes);

  auto cl = &class_list[class_idx];
  write(ba, class_idx);
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

KPackObject& KPackObject::operator=(const KPackObject& src_obj)
{
  qDeleteAll(polygons);
  polygons.clear();
  attributes.clear();
  class_idx  = src_obj.class_idx;
  name       = src_obj.name;
  attributes = src_obj.attributes;
  frame      = src_obj.frame;
  for (auto src_polygon: src_obj.polygons)
  {
    auto polygon = new KGeoPolygon;
    *polygon     = *src_polygon;
    polygons.append(polygon);
  }
  return *this;
}

KPackObject::KPackObject(const KPackObject& src_obj)
{
  *this = src_obj;
}

KPackObject::~KPackObject()
{
  qDeleteAll(polygons);
}

KTile::Status KTile::getStatus() const
{
  return status;
}

void KTile::setStatus(Status v)
{
  status = v;
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

void KPack::clear()
{
  if (main.getStatus() != KTile::Loaded)
    return;
  if (main.getStatus() == KTile::Loading)
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
  main.setStatus(KTile::Null);
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
  for (auto cl: classes)
    cl.save(&f);

  QByteArray ba;
  ba = qCompress(ba, 9);
  write(&f, ba.count());
  f.write(ba.data(), ba.count());

  write(&f, main.count());
  ba.clear();

  for (auto& obj: main)
    obj->save(classes, ba);
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
        obj->save(classes, ba);
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
  if (main.getStatus() == KTile::Loading)
    return;
  qDebug() << "111" << path;
  QElapsedTimer t;
  t.start();

  using namespace KSerialize;
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly))
  {
    qDebug() << "read error:" << path;
    return;
  }
  qDebug() << "222" << path;

  if (main.getStatus() != KTile::Null)
    return;

  QString format_id;
  read(&f, format_id);
  read(&f, frame);

  char has_borders = false;
  read(&f, has_borders);
  qDebug() << "333" << path;
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
  qDebug() << "444" << path;

  read(&f, main_mip);
  read(&f, tile_mip);

  qDebug() << "main_mip" << main_mip;
  qDebug() << "tile_mip" << tile_mip;

  if (!load_objects)
  {
    qDebug() << Q_FUNC_INFO << "exit";
    return;
  }

  qDebug() << "loading main from" << path;
  main.setStatus(KTile::Loading);
  int class_count;
  read(&f, class_count);
  qDebug() << "class_count" << class_count;
  for (int i = 0; i < class_count; i++)
  {
    KClass cl;
    cl.load(&f, pixel_size_mm);
    classes.append(cl);
  }

  qDebug() << "555" << path;
  int ba_count = 0;
  read(&f, ba_count);
  QByteArray ba;
  ba.resize(ba_count);
  f.read(ba.data(), ba_count);
  ba = qUncompress(ba);

  int pos = 0;
  qDebug() << "777" << path;

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
    obj->load(classes, pos, ba);
  }
  qDebug() << "888" << path;

  int small_count;
  read(&f, small_count);
  tiles.resize(small_count);
  for (auto& part: tiles)
    part = nullptr;
  qDebug() << Q_FUNC_INFO << "finished" << path;
}

void KPack::loadAll(double pixel_size_mm)
{
  loadMain(true, pixel_size_mm);
  for (int i = 0; i < tiles.count(); i++)
    loadTile(i);
}

void KPack::loadTile(int tile_idx)
{
  if (main.getStatus() != KTile::Loaded)
    return;
  if (tiles[tile_idx] &&
      tiles[tile_idx]->getStatus() == KTile::Loading)
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
      tiles[tile_idx] = new KTile;
    return;
  }

  if (!tiles[tile_idx])
    tiles[tile_idx] = new KTile;
  tiles[tile_idx]->resize(part_obj_count);

  int ba_count = 0;
  read(&f, ba_count);
  QByteArray ba;
  ba.resize(ba_count);
  f.read(ba.data(), ba_count);
  ba = qUncompress(ba);

  tiles[tile_idx]->setStatus(KTile::Loading);
  int pos = 0;
  for (auto& obj: *tiles[tile_idx])
  {
    obj = new KPackObject;
    obj->load(classes, pos, ba);
  }
}

void KPack::setClasses(QVector<KClass> v)
{
  classes = v;
}

void KPack::addBorder(KGeoPolygon v)
{
  borders.append(v);
}

const QVector<KClass>& KPack::getClasses() const
{
  return classes;
}

void KPack::setObjects(QVector<KPackObject> src_obj_list,
                       int                  max_objects_per_tile)
{
  int tile_side_num =
      std::ceil(1.0 * src_obj_list.count() / max_objects_per_tile);
  int tile_num = pow(tile_side_num, 2);
  tiles.resize(tile_num);
  for (auto& tile: tiles)
    tile = nullptr;
  auto map_size_m     = getFrame().getSizeMeters();
  auto map_top_left_m = getFrame().top_left.toMeters();
  for (auto& src_obj: src_obj_list)
  {
    auto obj = new KPackObject(src_obj);
    auto cl  = classes[obj->getClassIdx()];
    if (cl.max_mip == 0 || cl.max_mip > getTileMip())
      main.append(obj);
    else
    {
      auto   obj_top_left_m = obj->getFrame().top_left.toMeters();
      double shift_x_m      = obj_top_left_m.x() - map_top_left_m.x();
      int    part_idx_x =
          1.0 * shift_x_m / map_size_m.width() * tile_side_num;
      int shift_y = obj_top_left_m.y() - map_top_left_m.y();
      int part_idx_y =
          1.0 * shift_y / map_size_m.height() * tile_side_num;
      int part_idx = part_idx_y * tile_side_num + part_idx_x;
      if (!tiles[part_idx])
        tiles[part_idx] = new KTile;
      tiles[part_idx]->append(obj);
    }
  }
}
