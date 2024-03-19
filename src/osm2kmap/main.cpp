#include "math.h"
#include "kmap.h"
#include <QApplication>
#include <QtConcurrent/QtConcurrent>
#include <QDir>
#include <QDebug>

class OsmParser
{
  typedef QMultiMap<QString, QString> Attributes;

  KShapeManager*                  shman = nullptr;
  QStringList                     type_list;
  QMultiMap<QString, KGeoCoor>    nodes;
  QMultiMap<QString, KGeoPolygon> ways;
  QVector<KObject*>               objects;
  QRectF                          bounds;
  QString                         curr_way_id;
  KGeoPolygon                     curr_way_geom;

  QString inpath;
  QString outpath;
  QString map_name;
  KMap*   world_map = nullptr;

  QHash<QString, KGeoCoor> find_names_hash;
  QHash<QString, KGeoCoor> find_names_hash_en;

  Attributes getAttributes(QString line);
  void       setBounds(OsmParser::Attributes params);
  void       addNode(OsmParser::Attributes params);
  void       processWay(OsmParser::Attributes params);
  void       processRelation();
  QString    getLineType(QString line);

public:
  OsmParser(KShapeManager*, KMap* world_map, QString inpath,
            QString outpath, QString map_name);
  void parseBounds();
  int  parseNodes();
  void unite();
};

OsmParser::Attributes OsmParser::getAttributes(QString line)
{
  auto l = line.split(' ');
  if (l.isEmpty())
    return OsmParser::Attributes();
  l.removeFirst();
  if (l.isEmpty())
    return OsmParser::Attributes();
  l.last().remove("/>");
  Attributes params;
  for (auto p: l)
  {
    auto cv = p.split('=');
    params.insert(cv.first(), cv.last().remove("\"").remove(">"));
  }
  return params;
}

void OsmParser::setBounds(OsmParser::Attributes attributes)
{
  bounds.setBottom(attributes.value("minlat").toDouble());
  bounds.setLeft(attributes.value("minlon").toDouble());
  bounds.setRight(attributes.value("maxlon").toDouble());
  bounds.setTop(attributes.value("maxlat").toDouble());
}

void OsmParser::addNode(OsmParser::Attributes attributes)
{
  auto     lat  = attributes.value("lat").toDouble();
  auto     lon  = attributes.value("lon").toDouble();
  KGeoCoor coor = KGeoCoor::fromDegs(lat, lon);
  auto     id   = attributes.value("id");
  nodes.insert(id, coor);
}

OsmParser::OsmParser(KShapeManager* v, KMap* _world_map,
                     QString _inpath, QString _outpath,
                     QString _map_name)
{
  type_list << "bounds"
            << "node"
            << "way"
            << "relation";
  shman     = v;
  world_map = _world_map;
  inpath    = _inpath;
  outpath   = _outpath;
  map_name  = _map_name;
}

QString OsmParser::getLineType(QString line)
{
  for (auto type: type_list)
  {
    if (line.startsWith("<" + type))
      return type;
  }
  return QString();
}

void OsmParser::parseBounds()
{
  QFile f(inpath);
  if (!f.open(QIODevice::ReadOnly))
    return;

  qDebug() << Q_FUNC_INFO;

  QTextStream in(&f);

  while (!in.atEnd())
  {
    QString line   = in.readLine().trimmed();
    auto    params = getAttributes(line);
    auto    type   = getLineType(line);
    if (type == "bounds")
    {
      setBounds(params);
      return;
    }
  }
}

auto joinPolys(KObject* obj)
{
  double join_tolerance_m = 1.0;
  for (int i = -1; auto& polygon1: obj->polygons)
  {
    i++;
    auto last1 = polygon1->last();
    for (int j = -1; auto& polygon2: obj->polygons)
    {
      j++;
      if (i == j)
        continue;
      auto first2   = polygon2->first();
      auto last1_m  = last1.toMeters();
      auto first2_m = first2.toMeters();
      auto dx       = fabs(last1_m.x() - first2_m.x());
      auto dy       = fabs(last1_m.y() - first2_m.y());
      if (dx < join_tolerance_m && dy < join_tolerance_m)
      {
        polygon1->append(*polygon2);
        delete polygon2;
        obj->polygons.removeAt(j);
        return true;
      }
    }
  }
  return false;
}

void OsmParser::unite()
{
  KEditableMap map(outpath);

  map.shapes         = shman->shapes;
  map.local_load_mip = shman->local_load_mip;

  auto top_left = KGeoCoor::fromDegs(bounds.top(), bounds.left());
  auto bottom_right =
      KGeoCoor::fromDegs(bounds.bottom(), bounds.right());
  map.frame = {top_left, bottom_right};

  if (world_map)
  {
    qDebug() << "adding borders...";
    auto map_code = map_name;
    map_code.remove(".osm");
    bool found_borders = false;
    for (auto obj: world_map->global_tile)
    {
      auto attr_val =
          QString::fromUtf8(obj->attributes.value("iso_code"))
              .toLower();
      if (!attr_val.isEmpty() && map_code.contains(attr_val))
      {
        found_borders = true;
        for (auto polygon: obj->polygons)
          map.borders.append(*polygon);
      }
    }
    if (!found_borders)
      qDebug() << "ERROR: no borders found for" << map.path;
  }
  else
    qDebug() << "ERROR: world map not found, could not add map "
                "borders!";

  qDebug() << "processing find names hash...";
  qDebug() << "local_count=" << find_names_hash.count();
  qDebug() << "en_count=" << find_names_hash_en.count();
  QHashIterator<QString, KGeoCoor> it(find_names_hash);
  while (it.hasNext())
  {
    it.next();
    map.find_names.append(it.key());
    map.find_centers.append(it.value());
  }
  QHashIterator<QString, KGeoCoor> it_en(find_names_hash_en);
  while (it_en.hasNext())
  {
    it_en.next();
    map.find_names_en.append(it_en.key());
    map.find_centers_en.append(it_en.value());
  }
  qDebug() << "done";

  qDebug() << "joinPolys() started";
  QElapsedTimer t;
  t.start();
  for (auto obj: objects)
    while (joinPolys(obj))
      ;
  qDebug() << "joinPolys() elapsed" << t.restart();

  map.addObjects(objects, 500000);

  qDebug() << "  saving...";
  map.save(KMap::Compression::Gzip9);

  qDeleteAll(objects);
}

int OsmParser::parseNodes()
{
  QFile f(inpath);
  if (!f.open(QIODevice::ReadOnly))
    return -1;

  QTextStream in(&f);

  qDebug() << Q_FUNC_INFO;

  bool                             inside_node = false;
  QVector<int>                     shape_matches;
  KShape*                          found_shape = nullptr;
  QString                          name;
  QString                          name_en;
  QVector<QPair<QString, QString>> elements;
  int                              line_count = 0;
  while (!in.atEnd())
  {
    QString line       = in.readLine().trimmed();
    auto    attributes = getAttributes(line);

    if (line.startsWith("<way"))
    {
      return in.pos();
    }

    if (line_count++ % 100000 == 0)
      qDebug() << "line count" << line_count;

    if (line_count > 2000000)
      return 0;

    if (line.startsWith("<node"))
    {
      inside_node = true;
      shape_matches.resize(shman->shapes.count());
      shape_matches.fill(0);
      found_shape = nullptr;
      name.clear();
      elements.clear();
      addNode(attributes);
      if (line.endsWith("/>"))
        continue;
    }
    else if (!inside_node)
      continue;
    if (inside_node)
    {
      if (line.startsWith("<node"))
        continue;
      if (line.startsWith("</node"))
      {
        if (found_shape)
        {
          KObject* obj = new KObject;
          obj->name    = name;
          obj->name_en = name_en;
          obj->shape   = found_shape;
          if (!obj->name.isEmpty() &&
              !find_names_hash.contains(obj->name))
            find_names_hash.insert(obj->name, obj->getCenter());

          if (!obj->name_en.isEmpty() &&
              !find_names_hash_en.contains(obj->name_en))
            find_names_hash_en.insert(obj->name_en, obj->getCenter());

          for (auto attr: found_shape->attributes)
            for (auto el: elements)
            {
              if (el.first == attr.id)
              {
                obj->attributes.insert(el.first, el.second.toUtf8());
              }
            }
          auto poly = new KGeoPolygon;
          auto node = nodes.last();
          poly->append(node);
          obj->polygons.append(poly);
          objects.append(obj);

          if (obj->frame.isNull())
            obj->frame = poly->getFrame();
          else
            obj->frame = obj->frame.united(poly->getFrame());

          qDebug() << "added obj" << objects.count();
        }
      }
      else
      {
        auto read_key   = attributes.value("k");
        auto read_value = attributes.value("v");

        if (read_key == "name")
          name = read_value;
        if (read_key == "name_en")
          name_en = read_value;

        for (int sh_idx = -1; auto sh: shman->shapes)
        {
          sh_idx++;
          for (auto tag: sh->keytags)
          {
            if (tag.key == read_key &&
                (tag.value == read_value || tag.value.isEmpty()))
            {
              shape_matches[sh_idx]++;
              if (shape_matches[sh_idx] == sh->keytags.count())
              {
                found_shape = shman->shapes[sh_idx];
              }
            }
          }
        }
        elements.append({read_key, read_value});
      }
    }
  }
  return 0;
}

int main(int argc, char* argv[])
{
  using namespace kmap;

  QApplication a(argc, argv);

  KShapeManager shape_man(argv[1]);
  shape_man.loadShapes(QString(argv[1]) + "/" + argv[2],
                       QString(argv[1]) + "/images");
  auto str = shape_man.error_str;
  if (!str.isEmpty())
  {
    qDebug() << "ERROR: shape manager error:" << str;
    return -1;
  }
  auto shape_list = &shape_man.shapes;

  if (shape_list->isEmpty())
  {
    qDebug() << "ERROR: empty shape list!";
    return -1;
  }

  QDir dir(argv[3]);
  dir.setFilter(QDir::Files | QDir::NoSymLinks |
                QDir::NoDotAndDotDot);
  auto        list = dir.entryInfoList();
  QStringList map_name_list;
  for (auto& fi: list)
    map_name_list.append(fi.fileName());
  std::sort(map_name_list.begin(), map_name_list.end());

  QString output_dir = argv[4];
  QDir().mkdir(argv[4]);

  bool is_analyzing_local_map = false;
  if (QString(argv[2]).contains("local"))
    is_analyzing_local_map = true;

  KMap* world_map = nullptr;
  if (is_analyzing_local_map)
  {
    auto world_map_path = output_dir + "/world.kmap";
    world_map           = new KMap(world_map_path);
    world_map->loadAll();
  }

  for (auto& map_name: map_name_list)
  {
    if (!map_name.endsWith(".osm"))
      continue;

    OsmParser parser(&shape_man, world_map,
                     QString(argv[3]) + "/" + map_name,
                     output_dir + "/" + map_name + ".kmap", map_name);

    qDebug() << "opening" << map_name;
    parser.parseBounds();
    parser.parseNodes();
    parser.unite();
  }
}
