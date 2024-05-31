#define HIDEMAXMIN

#include "math.h"
#include "mapapi.h"
#include "qdmcmp.h"
#include "kpanclassmanager.h"
#include "krenderpack.h"
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

auto joinPolys(KPackObject& obj)
{
  double join_tolerance_m = 1.0;
  auto   polygons         = obj.getPolygons();
  for (int i = -1; auto& polygon1: polygons)
  {
    i++;
    auto last1 = polygon1->last();
    for (int j = -1; auto& polygon2: polygons)
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
        obj.removePolygonAt(j);
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

  KClassManager class_man(argv[1]);
  class_man.loadClasses(QString(argv[1]) + "/" + argv[2],
                        QString(argv[1]) + "/images");
  auto class_list = class_man.getClasses();

  KPanClassManager pan_class_man(argv[1]);
  pan_class_man.loadClasses(QString(argv[1]) + "/" + argv[2],
                            QString(argv[1]) + "/images");
  auto pan_class_list = pan_class_man.getClasses();

  auto str = pan_class_man.getErrorStr();
  if (!str.isEmpty())
  {
    qDebug() << "ERROR: class manager error:" << str;
    return -1;
  }

  if (class_list.isEmpty())
  {
    qDebug() << "ERROR: empty class list!";
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

  KRenderPack* world_map = nullptr;
  if (is_analyzing_local_map)
  {
    auto world_map_path = output_dir + "/world.kpack";
    world_map           = new KRenderPack(world_map_path);
    world_map->loadAll(0);
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
    auto path      = output_dir + "/" + map_name + ".kpack";
    path.remove(".sitx");
    path.remove(".sitz");
    path.remove(".mptz");
    KPack pack(path);

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
        for (auto obj: world_map->getMainTile())
        {
          auto attr_val = QString::fromUtf8(
                              obj->getAttributes().value("iso_code"))
                              .toLower();
          if (!attr_val.isEmpty() && map_code.contains(attr_val))
          {
            found_borders = true;
            for (auto polygon: obj->getPolygons())
              pack.addBorder(*polygon);
          }
        }
        if (!found_borders)
          qDebug() << "ERROR: no borders found for" << map_name;
      }
      else
        qDebug() << "ERROR: world map not found, could not add map "
                    "borders!";
    }

    pack.setClasses(class_list);
    pack.setMainMip(class_man.getMainMip());
    pack.setTileMip(class_man.getTileMip());
    QVector<KPackObject> obj_list;
    DFRAME               df;
    mapGetTotalBorder(hMap, &df, PP_GEO);
    auto top_left =
        KGeoCoor::fromDegs(rad2deg(df.X2), rad2deg(df.Y1));
    auto bottom_right =
        KGeoCoor::fromDegs(rad2deg(df.X1), rad2deg(df.Y2));
    pack.setFrame({top_left, bottom_right});

    int  object_count = mapGetObjectCount(hMap, 1);
    HOBJ info         = mapCreateSiteObject(hMap, hMap);

    qDebug() << "total object count" << object_count;

    int curr_percentage = 0;

    for (int in_obj_idx = 0; in_obj_idx < object_count; in_obj_idx++)
    {
      QString name;

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

      QStringList attr_names;
      QStringList attr_values;
      auto        semantic_count = mapSemanticAmount(info);
      for (int sematic_idx = 1; sematic_idx <= semantic_count;
           sematic_idx++)
      {
        WCHAR str_utf16[1000];
        if (mapSemanticNameUn(info, sematic_idx, str_utf16,
                              sizeof(str_utf16)))
        {
          attr_names.append(
              QString::fromUtf16(str_utf16).simplified());
        }
        if (mapSemanticValueUnicode(info, sematic_idx, str_utf16,
                                    sizeof(str_utf16)))
          attr_values.append(
              QString::fromUtf16(str_utf16).simplified());
      }

      int class_idx = pan_class_man.getClassIdx(code, key, attr_names,
                                                attr_values);
      if (class_idx < 0)
      {
        WCHAR   str_utf16[1000];
        QString name;

        if (mapSemanticCodeValueNameUn(info, 9, str_utf16,
                                       sizeof(str_utf16), 1))
          name = QString::fromUtf16(str_utf16).simplified();
        int point_count = mapPointCount(info, 0);
        if (!name.isEmpty() && point_count == 1)
        {
          class_idx =
              class_man.getClassIdxById("default_named_point");
          if (class_idx < 0)
            continue;
          poi_count++;
        }
        else
          continue;
      }

      auto pan_class = pan_class_list.at(class_idx);

      if (pan_class->type == KClass::None)
        continue;

      WCHAR str_utf16[1000];
      if (mapSemanticCodeValueNameUn(info, 9, str_utf16,
                                     sizeof(str_utf16), 1))
        name = QString::fromUtf16(str_utf16).simplified();

      if (pan_class->name_code > 0)
      {
        name.clear();
        if (mapSemanticCodeValueNameUn(info, pan_class->name_code,
                                       str_utf16, sizeof(str_utf16),
                                       1))
        {
          auto n = QString::fromUtf16(str_utf16).simplified();
          if (n != name)
            name += "\n" + QString::fromUtf16(str_utf16).simplified();
        }
      }

      name = name.remove("\"");

      KPackObject obj;
      obj.setName(name);

      obj.setClassIdx(class_idx);
      KClass cl = class_list[class_idx];

      for (auto attr: pan_class->attributes)
      {
        WCHAR   str_utf16[1000];
        QString value;

        if (mapSemanticCodeValueNameUn(info, attr.code, str_utf16,
                                       sizeof(str_utf16), 1))
        {
          value = QString::fromUtf16(str_utf16).simplified();
          obj.addAttribute(attr.name, value.toUtf8());
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

              if (dist < pan_class->coor_precision_coef * 0.01 &&
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
        if (!obj.getAttributes().isEmpty())
        {
          auto attr = obj.getAttributes().first().toLower();
          if (attr == "ru-chu")
            need_to_wrap = true;
          if (attr.toLower() == "rus")
            need_to_wrap = true;
        }

        if (need_to_wrap)
        {
          auto polygon_frame = polygon->getFrame();
          if (polygon_frame.bottom_right.needToWrap())
            for (auto& p: *polygon)
              p = p.wrapped();
        }

        obj.addPolygon(polygon);

        if (obj.getFrame().isNull())
          obj.setFrame(polygon->getFrame());
        else
          obj.setFrame(obj.getFrame().united(polygon->getFrame()));
      }

      if (max_dist < pan_class->coor_precision_coef * 0.01 &&
          pan_class->type == KClass::Polygon)
        continue;

      if (is_analyzing_local_map)
        if (cl.id == "океан, море" || cl.id == "водоём" ||
            cl.id == "река (площадной)")
        {
          if (obj.getPolygons().count() > 20)
          {
            auto idx = class_man.getClassIdxById("complex_water");
            cl       = class_list[idx];
          }
        }

      obj_list.append(obj);
    }

    qDebug() << "poi_count" << poi_count;

    qDebug() << "joinPolys() started";
    QElapsedTimer t;
    t.start();
    for (auto obj: obj_list)
    {
      KClass cl = class_list[obj.getClassIdx()];
      if (cl.type == KClass::Line)
        while (joinPolys(obj))
          ;
    }
    qDebug() << "joinPolys() elapsed" << t.restart();

    pack.addObjects(obj_list, 500000);

    qDebug() << "  saving...";
    pack.save();

    mapCloseData(hMap);

    qDebug() << "done";
  }
}
