#ifndef KOBJECT_H
#define KOBJECT_H

#include <QFileInfo>
#include "kpack.h"

struct KObjectClass
{
  static constexpr int default_width_mm = 1.0;

  QString       name;
  KShape::Type  type  = KShape::None;
  KShape::Style style = KShape::Solid;
  QColor        pen;
  float         pen_width_mm = 0;
  QColor        brush;
  QImage        image;
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
  int              selected_object_idx = -1;
  QPair<int, int>  moving_point_idx    = {-1, -1};
  QString          generateObjectFileName();
  QPair<int, int>  getSelectedObjectPointIdxAt(QPoint p0);

signals:
  QPoint   deg2pix(KGeoCoor);
  QPoint   deg2scr(KGeoCoor);
  KGeoCoor scr2deg(QPoint);
  void     updated();
  void     finishEdit();
  void     saved(QString);

public:
  KObjectManager(QString objects_dir, double pixel_size_mm);
  void createObject(KShape);
  void paintObject(QPainter* p, KObject obj,
                   bool highlighted = false);
  void onTapped(KGeoCoor coor);
  void paint(QPainter*);
  void acceptObject();
  void loadFile(QString path);

  int  getObjectIdxAt(QPoint);
  int  getObjectIdxInsidePolygon(QPolygon);
  void startMovingPoint(QPoint);
  void stopMovingPoint();
  void movePoint(QPoint);
  bool canScroll();

private slots:
  // Catches signals without arguments and emmits its overloaded
  // variants with arguments
  void loadFileWithoutUpdate(QFileInfo file_info);
  void loadFileWithoutUpdate(QString path);
};

#endif  // KOBJECT_H
