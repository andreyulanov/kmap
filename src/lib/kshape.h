#ifndef KSHAPE_H
#define KSHAPE_H

#include "kbase.h"

struct KShape
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

struct KShapeImage
{
  QString id;
  QImage  image;
};

typedef QVector<KShapeImage> KShapeImageList;

class KShapeManager: public QObject
{
  double           main_mip                    = 0;
  double           tile_mip                    = 0;
  int              default_coor_precision_coef = 1;
  QString          images_dir;
  QVector<KShape*> shapes;

protected:
  QString error_str;

public:
  KShapeManager(QString image_dir = QString());
  virtual ~KShapeManager();
  int    getShapeIdxById(QString id);
  KShape getShapeById(QString id);
  void   loadShapes(QString path, QString images_dir = QString());
  KShapeImageList  getShapeImageList();
  QVector<KShape*> getShapes();

  void   setMainMip(double);
  double getMainMip();

  void   setTileMip(double);
  double getTileMip();

  void   setDefaultCoorPrecisionCoef(double);
  double getDefaultCoorPrecisionCoef();

  QString getErrorStr();
};

#endif  // KSHAPE_H
