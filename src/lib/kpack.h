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

class KPackObject
{
  int class_idx;

public:
  QString name;

  QMap<QString, QByteArray> attributes;
  KGeoRect                  frame;
  QRectF                    tile_frame_m;
  QVector<KGeoPolygon*>     polygons;

  KPackObject() = default;
  KPackObject(const KPackObject&);
  KPackObject& operator=(const KPackObject&);
  virtual ~KPackObject();

  int  getClassIdx() const;
  void setClassIdx(int);

  void     save(const QVector<KClass>& class_list, QByteArray& ba);
  void     load(QVector<KClass>& class_list, int& pos,
                const QByteArray& ba);
  KGeoCoor getCenter();
};

struct KTile: public QVector<KPackObject*>
{
  enum Status
  {
    Null,
    Loading,
    Loaded
  };
  Status status = Null;
};

class KPack
{
  static constexpr int border_coor_precision_coef = 10000;

  QString path;
  double  main_mip = 0;
  double  tile_mip = 0;

protected:
  QVector<KClass>      classes;
  KGeoRect             frame;
  QVector<QPolygonF>   borders_m;
  QVector<KGeoPolygon> borders;
  KTile                main;
  QVector<KTile*>      tiles;

public:
  KPack(const QString& path);
  virtual ~KPack();
  void save(QString new_path = "") const;
  void loadMain(bool load_objects, double pixel_size_mm);
  void loadTile(int tile_idx, QRectF tile_rect_m);
  void loadAll(double pixel_size_mm);
  void clear();
  void add(const KPack&);

  void   setMainMip(double);
  double getMainMip() const;

  void   setTileMip(double);
  double getTileMip() const;

  void            setFrame(KGeoRect);
  const KGeoRect& getFrame() const;

  const KTile&           getMain() const;
  const QVector<KTile*>& getTiles() const;

  void                   addObjects(QVector<KPackObject> obj_list,
                                    int                  max_objects_per_tile);
  void                   setClasses(QVector<KClass> src_classes);
  void                   addBorder(KGeoPolygon);
  const QVector<KClass>& getClasses() const;
};

#endif  // KPACK_H
