#define HIDEMAXMIN

#include "math.h"
#include "mapapi.h"
#include "qdmcmp.h"
#include "kmap.h"
#include <QApplication>
#include <QtConcurrent/QtConcurrent>
#include <QDir>
#include <QDebug>

bool isVectorMap(QString path)
{
  int dot_pos = path.lastIndexOf('.');
  if (dot_pos < 0)
    return false;
  QString ext = path.right(path.count() - dot_pos - 1);
  return QStringList{"sit", "sitx", "sitz", "map", "mapz", "mptz"}
      .contains(ext);
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
  using namespace kmath;

  QApplication a(argc, argv);
  QDMapView    qd;

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

  mapMessageEnable(1);
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
    if (!isVectorMap(map_name))
      continue;

    qDebug() << "opening" << map_name;
    HMAP hMap = mapOpenData(
        (QString(argv[3]) + "/" + map_name).toUtf8(), GENERIC_READ);
    if (!hMap)
    {
      qDebug() << "    error";
      continue;
    }
    qDebug() << "  converting...";

    int  poi_count = 0;
    auto path      = output_dir + "/" + map_name + ".kmap";
    path.remove(".sitx");
    path.remove(".sitz");
    path.remove(".mptz");
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
        for (auto obj: world_map->main)
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

    map.shapes         = *shape_list;
    map.local_load_mip = shape_man.local_load_mip;
    QVector<KObject*> obj_list;
    DFRAME            df;
    mapGetTotalBorder(hMap, &df, PP_GEO);
    auto top_left =
        KGeoCoor::fromDegs(rad2deg(df.X2), rad2deg(df.Y1));
    auto bottom_right =
        KGeoCoor::fromDegs(rad2deg(df.X1), rad2deg(df.Y2));
    map.frame = {top_left, bottom_right};

    int  object_count = mapGetObjectCount(hMap, 1);
    HOBJ info         = mapCreateSiteObject(hMap, hMap);

    qDebug() << "total object count" << object_count;

    int curr_percentage = 0;

    QHash<QString, KGeoCoor> find_names_hash;
    QHash<QString, KGeoCoor> find_names_hash_en;

    for (int in_obj_idx = 0; in_obj_idx < object_count; in_obj_idx++)
    {
      QString name, name_en;

      int percentage = 100 * in_obj_idx / object_count;
      if (percentage != curr_percentage)
      {
        fprintf(stderr, "%d%% done\r", percentage);
        curr_percentage = percentage;
      }
      mapClearObject(info);
      mapReadObjectByNumber(hMap, hMap, info, 1, in_obj_idx + 1);

      int code = mapObjectExcode(info);

      WCHAR key_wchar[100];
      mapObjectRscKeyUn(info, key_wchar, sizeof(key_wchar));
      auto key = QString::fromUtf16(key_wchar);

      int shape_idx = shape_man.getShapeIdx(code, key);

      int local_shape_idx = 0;
      int attr_shape_idx  = -1;
      for (auto& sh: shape_man.shapes)
      {
        if (!sh->attrval.isEmpty())
        {
          WCHAR str_utf16[1000];
          auto  semantic_count = mapSemanticAmount(info);
          for (int sematic_idx = 1; sematic_idx <= semantic_count;
               sematic_idx++)
          {
            if (mapSemanticValueUnicode(info, sematic_idx, str_utf16,
                                        sizeof(str_utf16)))
            {
              auto value = QString::fromUtf16(str_utf16).simplified();
              if (value == sh->attrval)
              {
                attr_shape_idx = local_shape_idx;
                if (mapSemanticCodeValueNameUn(info, 9, str_utf16,
                                               sizeof(str_utf16), 1))
                  name = QString::fromUtf16(str_utf16).simplified();
                break;
              }
            }
          }
        }
        if (attr_shape_idx >= 0)
        {
          shape_idx = attr_shape_idx;
          break;
        }
        local_shape_idx++;
      }

      if (shape_idx < 0)
      {
        WCHAR   str_utf16[1000];
        QString name;

        if (mapSemanticCodeValueNameUn(info, 9, str_utf16,
                                       sizeof(str_utf16), 1))
          name = QString::fromUtf16(str_utf16).simplified();
        int point_count = mapPointCount(info, 0);
        if (!name.isEmpty() && point_count == 1)
        {
          shape_idx =
              shape_man.getShapeIdxById("default_named_point");
          if (shape_idx < 0)
            continue;
          poi_count++;
        }
        else
          continue;
      }

      auto shape = shape_list->at(shape_idx);
      if (shape->type == KShape::None)
        continue;

      WCHAR str_utf16[1000];
      if (mapSemanticCodeValueNameUn(info, 9, str_utf16,
                                     sizeof(str_utf16), 1))
        name = QString::fromUtf16(str_utf16).simplified();

      if (shape->name_code > 0)
        if (mapSemanticCodeValueNameUn(info, shape->name_code,
                                       str_utf16, sizeof(str_utf16),
                                       1))
        {
          auto n = QString::fromUtf16(str_utf16).simplified();
          if (n != name)
            name += "\n" + QString::fromUtf16(str_utf16).simplified();
        }

      if (mapSemanticCodeValueNameUn(info, 3238, str_utf16,
                                     sizeof(str_utf16), 1))
      {
        auto n = QString::fromUtf16(str_utf16).simplified();
        if (n != name)
          name_en += QString::fromUtf16(str_utf16).simplified();
      }

      name    = name.remove("\"");
      name_en = name_en.remove("\"");

      KObject* obj = new KObject;
      obj->name    = name;
      obj->name_en = name_en;

      obj->shape = (*shape_list)[shape_idx];

      if (!obj->name.isEmpty() &&
          !find_names_hash.contains(obj->name))
        find_names_hash.insert(obj->name, obj->getCenter());

      if (!obj->name_en.isEmpty() &&
          !find_names_hash_en.contains(obj->name_en))
        find_names_hash_en.insert(obj->name_en, obj->getCenter());

      for (auto attr: shape->attributes)
      {
        WCHAR   str_utf16[1000];
        QString value;

        if (mapSemanticCodeValueNameUn(info, attr.code, str_utf16,
                                       sizeof(str_utf16), 1))
        {
          value = QString::fromUtf16(str_utf16).simplified();
          obj->attributes.insert(attr.name, value.toUtf8());
          if (attr.name == "street")
          {
            auto address_str = value + " " + obj->name;
            address_str      = address_str.simplified();
            address_str      = address_str.remove("\"");
            if (!find_names_hash.contains(address_str))
              find_names_hash.insert(obj->name, obj->getCenter());
          }
        }
      }

      int    poly_count = mapPolyCount(info);
      double max_dist   = 0;
      for (int poly_idx = 0; poly_idx < poly_count; poly_idx++)
      {
        DOUBLEPOINT point;
        int         point_count = mapPointCount(info, poly_idx);
        auto        polygon     = new KGeoPolygon;
        DOUBLEPOINT prev_point_m;
        for (int point_idx = 0; point_idx < point_count; point_idx++)
        {
          DOUBLEPOINT point_m;
          if (mapGetPlanePoint(info, &point_m, point_idx + 1,
                               poly_idx))
          {
            if (point_idx == 0)
              prev_point_m = point_m;
            else
            {
              auto dist = sqrt(pow(prev_point_m.x - point_m.x, 2) +
                               pow(prev_point_m.y - point_m.y, 2));
              max_dist  = std::max(dist, max_dist);

              if (dist < shape_man.reduction_precision_m &&
                  point_idx < point_count - 1)
                continue;
            }
          }
          if (mapGetGeoPoint(info, &point, point_idx + 1, poly_idx))
          {
            auto vp = KGeoCoor::fromDegs(rad2deg(point.x),
                                         rad2deg(point.y));
            polygon->append(vp);
            prev_point_m = point_m;
          }
        }

        bool need_to_wrap = false;
        if (map_name.contains("ru-chu"))
          need_to_wrap = true;
        if (!obj->attributes.isEmpty())
        {
          auto attr = obj->attributes.first().toLower();
          if (attr == "ru-chu")
            need_to_wrap = true;
          if (attr.toLower() == "rus")
            need_to_wrap = true;
        }

        if (need_to_wrap)
        {
          auto polygon_frame = polygon->getFrame();
          if (polygon_frame.bottom_right.longitude() <
              KMap::wrap_longitude)
            for (auto& p: *polygon)
              p = p.wrapped();
        }

        obj->polygons.append(polygon);

        if (obj->frame.isNull())
          obj->frame = polygon->getFrame();
        else
          obj->frame = obj->frame.united(polygon->getFrame());
      }

      if (max_dist < shape_man.reduction_precision_m &&
          shape->type == KShape::Polygon)
      {
        delete obj;
        continue;
      }

      obj_list.append(obj);
    }

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

    qDebug() << "poi_count" << poi_count;

    qDebug() << "joinPolys() started";
    QElapsedTimer t;
    t.start();
    for (auto obj: obj_list)
      while (joinPolys(obj))
        ;
    qDebug() << "joinPolys() elapsed" << t.restart();

    map.addObjects(obj_list, 500000);

    qDebug() << "  saving...";
    map.save(KMap::Compression::Gzip9);

    qDeleteAll(obj_list);

    mapCloseData(hMap);

    qDebug() << "done";
  }
}
