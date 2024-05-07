#ifndef KSHAPE_H
#define KSHAPE_H

#include <QObject>
#include <QColor>
#include <QPixmap>
#include <QFile>

struct KAttribute
{
  QString name;
  int     code;
  bool    visible;
  double  max_mip;
};

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
  QString             id;
  int                 pan_code = 0;
  QString             pan_key;
  Type                type                = None;
  Style               style               = Solid;
  uchar               layer               = 0;
  float               width_mm            = 0.2;
  float               pixel_size_mm       = 0.1;
  float               min_mip             = 0;
  float               max_mip             = 100;
  int                 coor_precision_coef = 1;
  QColor              pen;
  QColor              brush;
  QColor              tcolor;
  QImage              image;
  int                 name_code = 0;
  QString             attrname;
  QString             attrval;
  QVector<KAttribute> attributes;
  void                save(QFile* f);
  void                load(QFile* f, double pixel_size_mm);
  bool                operator==(KShape) const;
  int                 getWidthPix();
};

struct KShapeImage
{
  QString id;
  QImage  image;
};

typedef QVector<KShapeImage> KShapeImageList;

struct KShapeManager: public QObject
{
  Q_OBJECT

public:
  double           main_mip                    = 0;
  double           tile_mip                    = 0;
  int              default_coor_precision_coef = 1;
  QVector<KShape*> shapes;
  QString          error_str;
  QString          images_dir;
  KShapeManager(QString image_dir = QString());
  int    getShapeIdx(int code, QString key, QStringList attr_names,
                     QStringList attr_values);
  int    getShapeIdxById(QString id);
  KShape getShapeById(QString id);
  void   loadShapes(QString path, QString images_dir = QString());
  KShapeImageList getShapeImageList();
  QVector<KShape> getShapes();
};

#endif  // KSHAPE_H
