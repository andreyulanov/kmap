#ifndef KOBJECT_H
#define KOBJECT_H

#include <QFileInfo>
#include "kpack.h"

struct KObjectClass
{
  QString              name;
  KShape::Type         type;
  KShape::Style        style;
  QColor               pen;
  float                pen_width_mm = 0;
  QColor               brush;
  QImage               image;
  static constexpr int default_image_size_mm = 3.0;
};

struct KObject
{
  KObjectClass              cl;
  QString                   name;
  QVector<KGeoPolygon>      polygons;
  QMap<QString, QString>    text_attr;
  QMap<QString, QByteArray> data_attr;
  void                      save(QString path);
  void                      load(QString path, double pixel_size_mm);
  bool                      isEmpty();
  int                       getWidthPix(double pixel_size_mm);
};

class KObjectManager: public QObject
{
  Q_OBJECT

  static constexpr double proximity_mm = 5.0;

  double           pixel_size_mm = 0;
  QString          objects_dir;
  QVector<KObject> objects;
  KObject          active_object;
  QString          generateObjectFileName();

signals:
  QPoint kcoor2pix(KGeoCoor);
  void   updated();
  void   finishEdit();
  void   saved(QString);

public:
  KObjectManager(QString objects_dir, double pixel_size_mm);
  void    createObject(KShape);
  void    paintObject(QPainter* p, KObject obj);
  void    addPoint(KGeoCoor coor);
  void    paint(QPainter*);
  void    acceptObject();
  void    loadFile(QString path);
  KObject getObjectNearPoint(QPoint);
  KObject getObjectInsidePolygon(QPolygon);

private slots:
  // Catches signals without arguments and emmits its overloaded
  // variants with arguments
  void loadFileWithoutUpdate(QFileInfo file_info);
  void loadFileWithoutUpdate(QString path);
};

#endif  // KOBJECT_H
