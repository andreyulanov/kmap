#ifndef KPACK_H
#define KPACK_H

#include <QPolygon>
#include <QColor>
#include <QPainter>
#include <QReadWriteLock>
#include <QMap>
#include <QElapsedTimer>
#include <QVariant>
#include "kclass.h"

struct KPackObject
{
  KClass*                   cl;
  QString                   name;
  QMap<QString, QByteArray> attributes;
  KGeoRect                  frame;
  QRectF                    tile_frame_m;
  QVector<KGeoPolygon*>     polygons;
  KGeoCoor                  getCenter();
  void save(const QVector<KClass*>* class_list, QByteArray& ba);
  void load(QVector<KClass*>* class_list, int& pos,
            const QByteArray& ba);
  virtual ~KPackObject();
};

struct KObjectCollection: public QVector<KPackObject*>
{
  enum Status
  {
    Null,
    Loading,
    Loaded
  };
  Status status = Null;
};

struct RenderAddress
{
  int layer_idx;
  int obj_idx;
};

class KPack
{
  static constexpr int border_coor_precision_coef = 10000;

  QString path;
  double  main_mip = 0;
  double  tile_mip = 0;

protected:
  QVector<KClass*>            classes;
  KGeoRect                    frame;
  QVector<QPolygonF>          borders_m;
  QVector<KGeoPolygon>        borders;
  KObjectCollection           main;
  QVector<KObjectCollection*> tiles;

public:
  KPack(const QString& path);
  virtual ~KPack();
  void save(QString new_path = "") const;
  void loadMain(bool load_objects, double pixel_size_mm);
  void loadTile(int tile_idx, QRectF tile_rect_m);
  void loadAll(double pixel_size_mm);
  void clear();
  void add(KPack*);

  void   setMainMip(double);
  double getMainMip() const;

  void   setTileMip(double);
  double getTileMip() const;

  void            setFrame(KGeoRect);
  const KGeoRect& getFrame() const;

  const KObjectCollection&          getMain() const;
  const QVector<KObjectCollection*> getTiles() const;
};

class KRenderPack: public QObject, public KPack
{
  Q_OBJECT

public:
  static constexpr int max_layer_count = 24;
  static constexpr int render_count    = 4;

  QVector<KPackObject*> render_data[max_layer_count];
  QReadWriteLock        main_lock;
  QReadWriteLock        tile_lock;
  QList<RenderAddress>  render_start_list;
  int                   render_object_count;

  void addCollectionToIndex(const KObjectCollection* collection);

signals:
  void loaded();

public:
  KRenderPack(const QString& path);
  ~KRenderPack();
  void clear();
  void loadMain(bool load_objects, double pixel_size_mm);
  void loadTile(int tile_idx, QRectF tile_rect_m);
  bool intersects(QPolygonF polygon) const;
};

struct KRenderPackCollection: public QVector<KRenderPack*>
{
  virtual ~KRenderPackCollection();
};

class KEditablePack: public KPack
{
public:
  KEditablePack(const QString& path);
  void addObjects(const QVector<KPackObject*>& obj_list,
                  int                          max_objects_per_tile);
  void setClasses(QVector<KClass*>);
  void addBorder(KGeoPolygon);
};

#endif  // KPACK_H
