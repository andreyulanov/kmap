#ifndef KPORTABLEOBJECT_H
#define KPORTABLEOBJECT_H

#include "kmap.h"

struct KPortableObject
{
  KShape::Type              type         = KShape::None;
  KShape::Style             style        = KShape::Solid;
  QColor                    pen          = Qt::black;
  float                     pen_width_mm = 0.2;
  QColor                    brush        = Qt::white;
  QColor                    text_color   = Qt::black;
  float                     text_size_mm = 2;
  QImage                    image;
  QString                   name;
  QVector<KGeoPolygon>      polygons;
  QVector<int>              inner_polygon_idx_list;
  QMap<QString, QString>    text_attr;
  QMap<QString, QByteArray> file_attr;
  void                      save(QString path);
  void                      load(QString path);
  bool                      isEmpty();
  int                       getWidthPix();
};

class KPortableObjectManager: public QObject
{
  Q_OBJECT

  QString                  objects_dir;
  QVector<KPortableObject> objects;
  KPortableObject          active_object;
  QString                  generateObjectFileName();

signals:
  QPoint kcoor2pix(KGeoCoor);
  void   updated();
  void   finishEdit();

public:
  KPortableObjectManager(QString objects_dir);
  void createObject(KShape);
  void paintObject(QPainter* p, KPortableObject obj);
  void addPoint(KGeoCoor coor);
  void paint(QPainter*);
  void acceptObject();
};

#endif  // KPORTABLEOBJECT_H
