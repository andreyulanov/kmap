#ifndef KMAP_H
#define KMAP_H

#include <QPolygon>
#include <QColor>
#include <QPainter>
#include <QReadWriteLock>
#include <QMap>
#include <QElapsedTimer>
#include <QVariant>
#include "kshape.h"
#include "kdatetime.h"

class KGeoCoor
{
  friend struct KGeoRect;
  friend struct KGeoPolygon;
  int lat = 0;
  int lon = 0;

public:
  KGeoCoor();
  KGeoCoor(int lat, int lon);
  static KGeoCoor fromDegs(double lat, double lon);
  static KGeoCoor fromMeters(QPointF m);
  QPointF         toMeters() const;
  double          longitude() const;
  double          latitude() const;
  bool            isValid();
  KGeoCoor        inc(KGeoCoor step) const;
  KGeoCoor        wrapped() const;
};

struct KPosition
{
  KGeoCoor  coor;
  float     altitude;
  KDateTime dt;
};

namespace kmath
{
constexpr double earth_r = 6378137;
double           deg2rad(double deg);
double           rad2deg(double rad);
double           getLength(QPoint p1, QPoint p2);
double           getAngle(QPoint p1, QPoint p2);
}

struct KGeoRect
{
  KGeoCoor top_left;
  KGeoCoor bottom_right;
  KGeoRect united(const KGeoRect&) const;
  bool     isNull() const;
  QRectF   toMeters() const;
  QSizeF   getSizeMeters() const;
  QRectF   toRectM();
};

struct KGeoPolygon: public QVector<KGeoCoor>
{
  KGeoRect getFrame();
  void     save(QByteArray& ba, int coor_precision_coef);
  void load(const QByteArray& ba, int& pos, int coor_precision_coef);
  QPolygonF toPolygonM();
};

struct KObject
{
  QString                   name;
  QString                   name_en;
  QMap<QString, QByteArray> attributes;
  KGeoRect                  frame;
  QVector<KGeoPolygon*>     polygons;
  KShape*                   shape;
  QRectF                    tile_frame_m;
  QVector<int>              inner_polygon_idx_list;
  KGeoCoor                  getCenter();
  void save(const QVector<KShape*>* shape_list, QByteArray& ba);
  void load(QVector<KShape*>* shape_list, int& pos,
            const QByteArray& ba);
  virtual ~KObject();
};

struct KObjectCollection: public QVector<KObject*>
{
  enum Status
  {
    Null,
    Loading,
    Loaded
  };
  Status status = Null;
};

struct KCategory
{
  QString name;
  QImage  image;
};
typedef QVector<KCategory> KCategories;

struct PartBorder
{
  int layer_idx;
  int obj_idx;
};

class KMap: public QObject
{
  Q_OBJECT

  void addCollectionToIndex(const KObjectCollection* collection);

public:
  static constexpr int    max_layer_count            = 24;
  static constexpr int    render_count               = 6;
  static constexpr int    border_coor_precision_coef = 10000;
  static constexpr double wrap_longitude             = -168.5;

  enum class Compression
  {
    Uncompressed,
    Gzip1,
    Gzip2,
    Gzip3,
    Gzip4,
    Gzip5,
    Gzip6,
    Gzip7,
    Gzip8,
    Gzip9
  };

  QString                     path;
  int                         compression_level;
  double                      main_mip = 0;
  double                      tile_mip = 0;
  QVector<KShape*>            shapes;
  KGeoRect                    frame;
  QVector<KGeoPolygon>        borders;
  QVector<QPolygonF>          borders_m;
  QStringList                 find_names;
  QVector<KGeoCoor>           find_centers;
  QStringList                 find_names_en;
  QVector<KGeoCoor>           find_centers_en;
  KObjectCollection           main;
  QReadWriteLock              main_lock;
  QVector<KObjectCollection*> tiles;
  QReadWriteLock              tile_lock;
  QVector<KObject*>           render_data[max_layer_count];
  QList<PartBorder>           render_start_list;
  int                         render_object_count;

  KMap(const QString& path);
  virtual ~KMap();
  void save(Compression compression, QString new_path = "");
  void loadMain(bool load_objects);
  void loadTile(int tile_idx, QRectF tile_rect_m);
  void loadAll();
  void clear();
  void add(KMap*);
  bool intersects(QPolygonF polygon) const;
signals:
  void loaded();
};

class KEditableMap: public KMap
{
public:
  KEditableMap(const QString& path);
  void addObjects(const QVector<KObject*>& obj_list,
                  int                      max_objects_per_tile);
};

struct KMapCollection: public QVector<KMap*>
{
  virtual ~KMapCollection();
  QStringList find(const QString&) const;
  KGeoCoor    getCoorByName(const QString&) const;
  KCategories getCategories() const;
};

#endif  // KMAP_H
