#include "math.h"
#include "kmap.h"
#include <QApplication>
#include <QtConcurrent/QtConcurrent>
#include <QDir>
#include <QDebug>

class OsmParser
{
  typedef QMultiMap<QString, QString> Params;

  QStringList                     type_list;
  QMultiMap<QString, KGeoCoor>    nodes;
  QMultiMap<QString, KGeoPolygon> ways;
  QRectF                          bounds;
  Params                          getParams(QString line);
  void                            setBounds(OsmParser::Params params);
  void                            addNode(OsmParser::Params params);
  void                            addWay(OsmParser::Params params);
  void    processWay(OsmParser::Params params);
  void    processRelation();
  QString getLineType(QString line);

public:
  OsmParser();
  void parse(QString path, QVector<KShape> shapes);
};

OsmParser::Params OsmParser::getParams(QString line)
{
  auto l = line.split(' ');
  if (l.isEmpty())
    return OsmParser::Params();
  l.removeFirst();
  if (l.isEmpty())
    return OsmParser::Params();
  l.last().remove("/>");
  Params params;
  for (auto p: l)
  {
    auto cv = p.split('=');
    params.insert(cv.first(), cv.last().remove("\"").remove(">"));
  }
  return params;
}

void OsmParser::setBounds(OsmParser::Params params)
{
  bounds.setBottom(params.value("minlat").toDouble());
  bounds.setLeft(params.value("minlon").toDouble());
  bounds.setRight(params.value("maxlon").toDouble());
  bounds.setTop(params.value("maxlat").toDouble());
}

void OsmParser::addNode(OsmParser::Params params)
{
  auto     lat  = params.value("lat").toDouble();
  auto     lon  = params.value("lon").toDouble();
  KGeoCoor coor = KGeoCoor::fromDegs(lat, lon);
  auto     id   = params.value("id");
  nodes.insert(id, coor);
}

void OsmParser::addWay(OsmParser::Params params)
{
  auto id = params.value("id");
  ways.insert(id, KGeoPolygon());
}

OsmParser::OsmParser()
{
  type_list << "bounds"
            << "node"
            << "way"
            << "relation";
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

void OsmParser::parse(QString path, QVector<KShape> shapes)
{
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly))
    return;

  QTextStream in(&f);

  QString         curr_type;
  Params          curr_params;
  int             line_count = 0;
  QVector<Params> readtags;
  KGeoPolygon     curr_way;
  while (!in.atEnd())
  {
    QString line = in.readLine().trimmed();
    line_count++;

    auto params = getParams(line);

    auto type = getLineType(line);
    if (type == "bounds")
    {
      setBounds(params);
      continue;
    }
    if (type == "node" && line.endsWith("/>"))
    {
      addNode(params);
      continue;
    }

    if (type.isEmpty() && curr_type.isEmpty())
      continue;

    if (curr_type.isEmpty())
    {
      readtags.clear();
      if (type == "node")
        addNode(params);
      if (type == "way")
        addWay(params);
      curr_type = type;
      continue;
    }

    if (line == "</" + curr_type + ">")
    {
      KShape           curr_shape;
      QVector<KOsmTag> usetags;
      for (auto sh: shapes)
      {
        int match_count = 0;
        for (auto keytag: sh.keytags)
        {
          bool tag_found = false;
          for (auto readtag: readtags)
          {
            QMapIterator<QString, QString> it(readtag);
            QString                        key, value;
            while (it.hasNext())
            {
              it.next();
              auto k = it.key();
              if (k == "k")
                key = it.value();
              if (k == "v")
              {
                value = it.value();
                if (sh.usetags.contains(key))
                  usetags.append({key, value});
              }
            }
            if (keytag.key == key && keytag.value == value)
            {
              tag_found = true;
              break;
            }
          }
          if (tag_found)
            match_count++;
        }
        if (match_count == sh.keytags.count())
        {
          auto obj = new KObject;
          if (curr_type == "node")
          {
            auto poly = new KGeoPolygon;
            poly->append(nodes.last());
          }
        }
      }
      curr_type.clear();
      readtags.clear();
    }
    else
    {
      readtags += params;
      if (curr_type == "way" && line.startsWith("<nd"))
      {
        auto ref = params.value("ref");
        curr_way.append(nodes.value(ref));
      }
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

    OsmParser parser;

    qDebug() << "opening" << map_name;
    parser.parse((QString(argv[3]) + "/" + map_name),
                 shape_man.getShapes());
    qDebug() << "  converting...";

    auto         path = output_dir + "/" + map_name + ".kmap";
    KEditableMap map(path);

    if (is_analyzing_local_map)
    {
      if (world_map)
      {
        qDebug() << "adding borders...";
        auto map_code = map_name;
        map_code.remove(".sitx");
        map_code.remove(".sitz");
        map_code.remove(".mptz");
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
    }
  }
}
