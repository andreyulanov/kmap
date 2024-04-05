#ifndef KOBJECT_H
#define KOBJECT_H

#include "kmap.h"

struct KObject
{
  QString                   name;
  KShape::Type              type         = KShape::None;
  KShape::Style             style        = KShape::Solid;
  QColor                    pen          = Qt::black;
  float                     pen_width_mm = 0.2;
  QColor                    brush        = Qt::white;
  QColor                    text_color   = Qt::black;
  float                     text_size_mm = 2;
  QImage                    image;
  QVector<KGeoPolygon>      polygons;
  QVector<int>              inner_polygon_idx_list;
  QMap<QString, QString>    text_attr;
  QMap<QString, QByteArray> file_attr;
  void                      save(QString path);
  void                      load(QString path, double pixel_size_mm);
  bool                      isEmpty();
  int                       getWidthPix(double pixel_size_mm);
};

class KObjectManager: public QObject
{
  Q_OBJECT

  double                   pixel_size_mm = 0;
  QString                  objects_dir;
  QVector<KObject> objects;
  KObject          active_object;
  QString                  generateObjectFileName();

signals:
  QPoint kcoor2pix(KGeoCoor);
  void   updated();
  void   finishEdit();

public:
  KObjectManager(QString objects_dir, double pixel_size_mm);
  void createObject(KShape);
  void paintObject(QPainter* p, KObject obj);
  void addPoint(KGeoCoor coor);
  void paint(QPainter*);
  void acceptObject();
};

#endif  // KOBJECT_H
