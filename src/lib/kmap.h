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
  int                     lat            = 0;
  int                     lon            = 0;
  static constexpr double wrap_longitude = -168.5;

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
  bool            needToWrap();
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

struct KMapObject
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
  virtual ~KMapObject();
};

struct KObjectCollection: public QVector<KMapObject*>
{
  enum Status
  {
    Null,
    Loading,
    Loaded
  };
  Status status = Null;
};

struct PartBorder
{
  int layer_idx;
  int obj_idx;
};

class KMap
{
  static constexpr int border_coor_precision_coef = 10000;

  QString path;
  double  main_mip = 0;
  double  tile_mip = 0;

public:
  QVector<KShape*>            shapes;
  KGeoRect                    frame;
  QVector<KGeoPolygon>        borders;
  QVector<QPolygonF>          borders_m;
  KObjectCollection           main;
  QVector<KObjectCollection*> tiles;

  KMap(const QString& path);
  virtual ~KMap();
  void save(QString new_path = "");
  void loadMain(bool load_objects);
  void loadTile(int tile_idx, QRectF tile_rect_m);
  void loadAll();
  void clear();
  void add(KMap*);
  bool intersects(QPolygonF polygon) const;

  void   setMainMip(double);
  double getMainMip() const;

  void   setTileMip(double);
  double getTileMip() const;
};

class KRenderMap: public QObject, public KMap
{
  Q_OBJECT

public:
  static constexpr int max_layer_count = 24;
  static constexpr int render_count    = 6;

  QVector<KMapObject*> render_data[max_layer_count];
  QReadWriteLock       main_lock;
  QReadWriteLock       tile_lock;
  QList<PartBorder>    render_start_list;
  int                  render_object_count;

  void addCollectionToIndex(const KObjectCollection* collection);

signals:
  void loaded();

public:
  KRenderMap(const QString& path);
  ~KRenderMap();
  void clear();
  void loadMain(bool load_objects);
  void loadTile(int tile_idx, QRectF tile_rect_m);
};

struct KRenderMapCollection: public QVector<KRenderMap*>
{
  virtual ~KRenderMapCollection();
};

class KEditableMap: public KMap
{
public:
  KEditableMap(const QString& path);
  void addObjects(const QVector<KMapObject*>& obj_list,
                  int                         max_objects_per_tile);
};

#endif  // KMAP_H
