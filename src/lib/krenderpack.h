#ifndef KRENDERPACK_H
#define KRENDERPACK_H

#include "kpack.h"

class KRenderPack: public QObject, public KPack
{
  Q_OBJECT

  struct RenderAddress
  {
    int layer_idx;
    int obj_idx;
  };

public:
  static constexpr int max_layer_count = 24;
  static constexpr int render_count    = 4;

  QVector<KPackObject*> render_data[max_layer_count];
  QReadWriteLock        main_lock;
  QReadWriteLock        tile_lock;
  QList<RenderAddress>  render_start_list;
  int                   render_object_count;

  void addCollectionToIndex(const KPackObjectCollection* collection);

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

#endif  // KRENDERPACK_H