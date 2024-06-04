#ifndef KPACKFETCHER_H
#define KPACKFETCHER_H

#include <QThread>
#include <QPolygonF>
#include <QMap>
#include "kpack.h"

class KPackFetcher: public QThread
{
  Q_OBJECT

  struct LocalMapEntry
  {
    QRectF             rect;
    QVector<QPolygonF> polygons;
  };
  QMap<QString, LocalMapEntry> iso_metrics_map;
  QStringList                  name_list;
  QRectF                       requested_rect;
  QString                      map_dir;

  void run();

signals:
  void fetched(QString map_name);

public:
  KPackFetcher(QString map_dir, const KPack* world_map);
  void requestRect(QRectF);
};

#endif  // KPACKFETCHER_H
