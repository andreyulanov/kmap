#ifndef KOBJECT_H
#define KOBJECT_H

#include <QMap>
#include "kclass.h"

struct KObject
{
  int                       class_idx = 0;
  QString                   name;
  QMap<QString, QByteArray> attributes;
  KGeoRect                  frame;
  QVector<KGeoPolygon>      polygons;

public:
  void save(const QVector<KClass>& class_list, QByteArray& ba) const;
  void load(QVector<KClass>& class_list, int& pos,
            const QByteArray& ba);
  KGeoCoor getCenter();
};

#endif  // KOBJECT_H
