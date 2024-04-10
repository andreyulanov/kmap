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
  };

  struct NameHolder
  {
    int               length_pix  = 0;
    int               start_idx   = 0;
    int               end_idx     = 0;
    int               point_count = 0;
    double            angle_deg   = 0;
    QPoint            mid_point;
    const KMapObject* obj = nullptr;
    void              fix(const KMapObject* obj, const QPoint& start,
                          const QPoint& end);
  };

  Q_OBJECT

  double render_window_size_coef      = 0;
  QColor background_color             = Qt::white;
  int    update_interval_ms           = 0;
  double max_object_size_with_name_mm = 20.0;

  QPointF render_center_m;
  double  mip        = 1;
  double  render_mip = 1;
  QPixmap main_pixmap;
  QPixmap render_pixmap;
  bool    rendering_enabled      = false;
  bool    loading_enabled        = true;
  bool    getting_pixmap_enabled = false;
  int     load_thread_count      = 0;

  QPointF              center_m;
  QSize                pixmap_size   = {100, 100};
  double               pixel_size_mm = 0.1;
  KRenderMapCollection maps;
  QFont                font;

  QVector<DrawTextEntry> draw_text_array[KRenderMap::render_count];
  QVector<NameHolder>    name_holder_array[KRenderMap::render_count];
  QVector<QRect>         text_rect_array;
  QSizeF                 size_m;
  QPointF                render_top_left_m;
  QRectF                 render_frame_m;
  QElapsedTimer          yield_timer;
  QVector<QRect>         point_object_text_rects;

  void run();
  void start() = delete;
  void insertMap(int idx, QString path, bool load_now);
  void renderMap(QPainter* p, KRenderMap* map, int render_idx);
  void render(QPainter* p, QVector<KRenderMap*> render_maps,
              int render_idx);

  bool checkMipRange(const KMapObject* obj);
  bool canContinue();
  void checkYieldResult();

  bool paintObject(QPainter* p, const KMapObject* obj,
                   int render_idx);
  bool paintLineNames(QPainter* p);
  bool paintPolygonNames(QPainter* p);

  bool isCluttering(const QRect&);
  void paintOutlinedText(QPainter* p, const DrawTextEntry& dte);
  void addDrawTextEntry(QVector<DrawTextEntry>& draw_text_array,
                        DrawTextEntry           new_dte);

  QPolygon poly2pix(const KGeoPolygon& polygon);
  void     paintPointObject(QPainter* p, const KMapObject* obj);
  void     paintPolygonObject(QPainter* p, const KMapObject* obj,
                              int render_idx);
  void     paintLineObject(QPainter* painter, const KMapObject* obj,
                           int render_idx);
  QRectF   getDrawRectM() const;
  bool     needToLoadMap(const KRenderMap*, const QRectF& draw_rect);
  void     checkLoad();
  void     checkUnload();
  void     onLoaded();

signals:
  void started(QRectF);
  void paintUserObjects(QPainter* p);
  void rendered(int ms_elapsed);

public:
  virtual ~KRender();
  void           addMap(QString path, bool load_now);
  void           setMip(double);
  double         getMip() const;
  void           setCenterM(QPointF);
  QPointF        getCenterM() const;
  QPointF        getRenderCenterM() const;
  void           setPixmapSize(QSize);
  void           setPixelSizeMM(double);
  void           setUpdateIntervalMs(int ms);
  void           setBackgroundColor(QColor);
  double         getRenderWindowSizeCoef() const;
  void           setRenderWindowSizeCoef(double);
  static void    paintOutlinedText(QPainter* p, const QString& text,
                                   const QColor& tcolor);
  const QPixmap* getPixmap() const;
  const KRenderMapCollection* getMaps() const;
  void                        renderMap();
  void                        renderUserObjects();
  void                        stopAndWait();
  void                        enableLoading(bool);

  QPoint kcoor2pix(KGeoCoor) const;

  QPoint  meters2pix(QPointF m) const;
  QPointF pix2meters(QPointF pix) const;

  QPoint   deg2pix(const KGeoCoor& deg) const;
  KGeoCoor pix2deg(QPoint pix) const;

  void pan(QPoint);
  void zoom(double);
};

#endif  // KRENDER_H
