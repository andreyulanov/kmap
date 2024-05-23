#ifndef KSHAPE_H
#define KSHAPE_H

#include <QObject>
#include <QColor>
#include <QPixmap>
#include <QFile>
#include <QPainter>
#include "kdatetime.h"

namespace kmath
{
constexpr double earth_r = 6378137;
double           deg2rad(double deg);
double           rad2deg(double rad);
double           getDistance(QPoint p1, QPoint p2);
double           getAngle(QPoint p1, QPoint p2);
double           sqr(double x);
int              getPolylinePointIdxAt(QPoint p0, QPolygon polyline,
                                       int tolerance_pix);
bool isNearPolyline(const QPoint& p0, const QPolygon& polyline,
                    int tolerance_pix);
}

class KGeoCoor
{
  friend struct KGeoRect;
  friend struct KGeoPolygon;
  int                     lat            = 0;
  int                     lon            = 0;
  static constexpr double wrap_longitude = -168.5;

public:
  KGeoCoor();
  KGeoCoor(int lat, int lon);
  static KGeoCoor fromDegs(double lat, double lon);
  static KGeoCoor fromMeters(QPointF m);
  QPointF         toMeters() const;
  double          longitude() const;
  double          latitude() const;
  bool            isValid();
  KGeoCoor        inc(KGeoCoor step) const;
  KGeoCoor        wrapped() const;
  bool            needToWrap();
};

struct KPosition
{
  KGeoCoor  coor;
  float     altitude;
  KDateTime dt;
};

struct KGeoRect
{
  KGeoCoor top_left;
  KGeoCoor bottom_right;
  KGeoRect united(const KGeoRect&) const;
  bool     isNull() const;
  QRectF   toMeters() const;
  QSizeF   getSizeMeters() const;
  QRectF   toRectM() const;
};

struct KGeoPolygon: public QVector<KGeoCoor>
{
  KGeoRect getFrame();
  void     save(QByteArray& ba, int coor_precision_coef);
  void load(const QByteArray& ba, int& pos, int coor_precision_coef);
  QPolygonF toPolygonM();
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

struct KShapeManager: public QObject
{
  double           main_mip                    = 0;
  double           tile_mip                    = 0;
  int              default_coor_precision_coef = 1;
  QVector<KShape*> shapes;
  QString          error_str;
  QString          images_dir;
  KShapeManager(QString image_dir = QString());
  virtual ~KShapeManager();
  int    getShapeIdxById(QString id);
  KShape getShapeById(QString id);
  void   loadShapes(QString path, QString images_dir = QString());
  KShapeImageList getShapeImageList();
  QVector<KShape> getShapes();
};

#endif  // KSHAPE_H
