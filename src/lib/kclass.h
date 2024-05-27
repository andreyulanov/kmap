#ifndef KCLASS_H
#define KCLASS_H

#include "kbase.h"

struct KClass
{
  Q_GADGET
public:
  enum Type : uchar
  {
    None,
    Point,
    Line,
    Polygon
  };
  Q_ENUM(Type)
  enum Style : uchar
  {
    Solid,
    Dash,
    Hatch,
    BDiag,
    FDiag,
    Horiz,
    Vert,
    Dots
  };
  Q_ENUM(Style)
  QString id;
  Type    type                = None;
  Style   style               = Solid;
  uchar   layer               = 0;
  float   width_mm            = 0.2;
  float   pixel_size_mm       = 0.1;
  float   min_mip             = 0;
  float   max_mip             = 100;
  int     coor_precision_coef = 1;
  QColor  pen;
  QColor  brush;
  QColor  tcolor;
  QImage  image;
  void    save(QFile* f);
  void    load(QFile* f, double pixel_size_mm);
  int     getWidthPix();
};

struct KClassImage
{
  QString id;
  QImage  image;
};

typedef QVector<KClassImage> KClassImageList;

class KClassManager: public QObject
{
  double           main_mip                    = 0;
  double           tile_mip                    = 0;
  int              default_coor_precision_coef = 1;
  QString          images_dir;
  QVector<KClass*> classes;

protected:
  QString error_str;

public:
  KClassManager(QString image_dir = QString());
  virtual ~KClassManager();
  int    getClassIdxById(QString id);
  KClass getClassById(QString id);
  void   loadClasses(QString path, QString images_dir = QString());
  KClassImageList  getClassImageList();
  QVector<KClass*> getClasses();

  void   setMainMip(double);
  double getMainMip();

  void   setTileMip(double);
  double getTileMip();

  void   setDefaultCoorPrecisionCoef(double);
  double getDefaultCoorPrecisionCoef();

  QString getErrorStr();
};

#endif  // KCLASS_H
