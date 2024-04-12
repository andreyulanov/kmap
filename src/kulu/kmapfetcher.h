#ifndef KMAPFETCHER_H
#define KMAPFETCHER_H

#include <QThread>
#include <QPolygonF>
#include <QMap>
#include "kmap.h"

class KMapFetcher: public QThread
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
  KMapFetcher(QString map_dir, const KMap* world_map);
  void requestRect(QRectF);
};

#endif  // KMAPFETCHER_H
