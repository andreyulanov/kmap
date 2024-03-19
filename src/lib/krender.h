#ifndef KRENDER_H
#define KRENDER_H

#include "kmap.h"
#include <QReadWriteLock>
#include <QThread>
#include <QSet>
#include <QMutex>

class KRender: public QThread
{
  friend class KRenderThread;

  struct DrawTextEntry
  {
    QString       text;
    KShape*       shape;
    QRect         rect;
    QRect         actual_rect;
    Qt::Alignment alignment;
    bool          is_selected = false;
  };

  struct NameHolder
  {
    int            length_pix  = 0;
    int            start_idx   = 0;
    int            end_idx     = 0;
    int            point_count = 0;
    double         angle_deg   = 0;
    QPoint         mid_point;
    const KObject* obj = nullptr;
    void           fix(const KObject* obj, const QPoint& start,
                       const QPoint& end);
  };

  Q_OBJECT

  int    min_object_size_pix     = 40;
  double mip                     = 1;
  double render_window_size_coef = 2;
  QColor background_color        = Qt::white;
  int    update_interval_ms      = 100;

  QPointF render_center_m;
  double  render_mip = 1;
  QPixmap render_pixmap;
  bool    rendering_enabled      = false;
  bool    loading_enabled        = true;
  bool    getting_pixmap_enabled = false;
  QString selected_category_name;

  QPointF        center_m;
  QSize          pixmap_size = {100, 100};
  KMapCollection maps;
  QFont          font;

  QVector<DrawTextEntry> draw_text_array[KMap::render_count];
  QVector<NameHolder>    name_holder_array[KMap::render_count];
  QVector<QRect>         text_rect_array;
  QSizeF                 size_m;
  QPointF                render_top_left_m;
  QRectF                 render_frame_m;
  QElapsedTimer          yield_timer;
  QVector<QRect>         point_object_text_rects;

  void run();
  void render(QPainter* p, KMap* map, int render_idx);

  bool checkMipRange(const KObject* obj);
  bool canContinue();
  void checkYieldResult();

  bool drawObject(QPainter* p, const KObject* obj, int render_idx);
  bool drawLineNames(QPainter* p);
  bool drawPolygonNames(QPainter* p);

  bool isCluttering(const QRect&);
  void drawOutlinedText(QPainter* p, const DrawTextEntry& dte);
  void addDrawTextEntry(QVector<DrawTextEntry>& draw_text_array,
                        DrawTextEntry           new_dte);

  QPolygon poly2pix(const KGeoPolygon& polygon);
  void     drawPointObject(QPainter* p, const KObject* obj);
  void     drawPolygonObject(QPainter* p, const KObject* obj,
                             int render_idx);
  void     drawLineObject(QPainter* painter, const KObject* obj,
                          int render_idx);
  QRectF   getDrawRectM() const;
  bool     needToLoadMap(const KMap*, const QRectF& draw_rect);
  void     checkLoad();
  void     checkUnload();
  bool     needToLoadSmall(const KMap* map);
  void     onLoaded();

signals:
  void paintObjects(QPainter* p);
  void rendered(int ms_elapsed);

public:
  virtual ~KRender();
  const KMap* appendMap(QString path, double min_mip, double max_mip,
                        bool load_now);
  const KMap* insertMap(int idx, QString path, double min_mip,
                        double max_mip, bool load_now);
  void        setMip(double);
  double      getMip() const;
  void        setCenterM(QPointF);
  QPointF     getCenterM() const;
  QPointF     getRenderCenterM() const;
  void        setPixmapSize(QSize);
  void        setMinObjectSizePix(int);
  void        setBackgroundColor(QColor);
  double      getRenderWindowSizeCoef() const;
  void        setRenderWindowSizeCoef(double);
  static void drawOutlinedText(QPainter* p, const QString& text,
                               const QColor& tcolor);
  const QPixmap*        getPixmap() const;
  const KMapCollection* getMaps() const;
  void                  selectCategory(const QString&);
  void                  start();
  void                  stopAndWait();
  void                  enableLoading(bool);

  QPoint kcoor2pix(KGeoCoor) const;

  QPoint  meters2pix(QPointF m) const;
  QPointF pix2meters(QPointF pix) const;

  QPoint   deg2pix(const KGeoCoor& deg) const;
  KGeoCoor pix2deg(QPoint pix) const;

  void pan(QPoint);
  void zoom(double);
};

#endif  // KRENDER_H
