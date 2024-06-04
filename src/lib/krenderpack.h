#ifndef KRENDERPACK_H
#define KRENDERPACK_H

#include "kpack.h"

class KRender;

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

  KRender*             render = nullptr;
  QVector<KObject*>    render_data[max_layer_count];
  QReadWriteLock       main_lock;
  QReadWriteLock       tile_lock;
  QList<RenderAddress> render_start_list;
  int                  render_object_count = 0;

  void addCollectionToIndex(KTile& collection);

public:
  KRenderPack(KRender* render, const QString& path);
  ~KRenderPack();
  void clear();
  void loadMain(bool load_objects, double pixel_size_mm);
  void loadTile(int tile_idx);
  bool intersects(QPolygonF polygon) const;
};

struct KRenderPackCollection: public QVector<KRenderPack*>
{
  virtual ~KRenderPackCollection();
};

#endif  // KRENDERPACK_H
