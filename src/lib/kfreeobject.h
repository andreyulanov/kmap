#ifndef KFREEOBJECT_H
#define KFREEOBJECT_H

#include <QFileInfo>
#include <QUuid>
#include <QMap>
#include <QVariant>
#include "kclass.h"

struct KFreeObject
{
  struct Attribute
  {
    enum Type : uchar
    {
      Int,
      Double,
      String,
      File
    };

    Type       type;
    QString    name;
    float      min_mip;
    float      max_mip;
    QByteArray data;
  };

  QUuid guid;

  KClass               shape;
  QVector<KGeoPolygon> polygons;
  QVector<Attribute>   attributes;

  void save(QString path);
  void load(QString path, double pixel_size_mm);
  bool isEmpty();
  int  getWidthPix(double pixel_size_mm);
};

class KFreeObjectManager: public QObject
{
  Q_OBJECT

  static constexpr double proximity_mm = 5.0;

  enum class PaintMode
  {
    Normal,
    Selected,
    Edited
  };

  double               pixel_size_mm = 0;
  QString              objects_dir;
  QVector<KFreeObject> objects;
  int                  edited_object_idx = -1;
  QVector<QUuid>       selected_guids;
  QPair<int, int>      moving_point_idx       = {-1, -1};
  bool                 is_creating_new_object = false;
  QString              getObjectPath(QUuid object_name);
  QPair<int, int>      getSelectedObjectPointIdxAt(QPoint p0);

signals:
  QPoint   deg2pix(KGeoCoor);
  QPoint   deg2scr(KGeoCoor);
  KGeoCoor scr2deg(QPoint);
  void     updated();
  void     startEdit();
  void     finishEdit();
  void     saved(QString);

public:
  KFreeObjectManager(QString objects_dir, double pixel_size_mm);
  void createObject(KClass);
  void removeObject();
  void paintObject(QPainter* p, KFreeObject obj,
                   PaintMode paint_mode);
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

#endif  // KFREEOBJECT_H
