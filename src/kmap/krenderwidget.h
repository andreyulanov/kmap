#ifndef KRENDERWIDGET_H
#define KRENDERWIDGET_H

#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QTimer>
#include <QGestureEvent>
#include <QWheelEvent>
#include "krender.h"
#include "kobject.h"
#include <QOpenGLWidget>

class KRenderWidget: public QWidget
{
  Q_OBJECT

  typedef QWidget base;

public:
  struct Settings
  {
    QString map_dir;
    double  pixel_size_mm;
    QSize   window_size;
    double  render_window_size_coef = 2;
    QColor  background_color        = QColor(150, 210, 240);
    int     update_interval_ms      = 100;
  };

private:
  enum ZoomMode
  {
    None,
    In,
    Out
  };

  ZoomMode      zoom_mode                            = None;
  double        zoom_speed                           = 1.1;
  double        max_zoom_speed                       = 1.1;
  const int     min_time_between_pinch_and_scroll_ms = 200;
  QElapsedTimer time_since_last_pinch;
  bool          is_pinching = false;
  QTimer        zoom_timer;
  QPoint        focus_shift;
  bool          shifted_after_zoom = false;

  QPoint mouse_pos;
  QPoint total_pan_pos;
  double intermediate_zoom_coef = 1;
  QPoint zoom_focus_shift;

  bool    zoom_pixmap_rendered = false;
  QLabel  label;
  QLabel  full_label;
  QLabel  scaled_label;
  KRender r;

  void scan(QString map_dir);
  void mousePressEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void wheelEvent(QWheelEvent* event) override;
  bool event(QEvent* e) override;
  void checkZoomFinished();
  void updateLabel(const QPixmap*, int ms_elapsed);
  void scaleLabel();
  bool checkCanScroll();

  void   stepZoom();
  void   onRendered(int ms_elapsed);
  void   startZoom(ZoomMode, QPoint focus_shift = QPoint());
  QPoint getTotalShift() const;

signals:
  void modified();
  void mousePressed(QPoint);
  void mouseMoved(QPoint);
  void mouseReleased();
  void pinchStarted();
  void zoomFinished();
  void movedCenterTo(const KGeoCoor&);
  void paintUserObjects(QPainter*);
  void tapped(KGeoCoor);
  void startedRender(QRectF);
  bool canScroll();

public:
  KRenderWidget(Settings settings);
  void         renderMap();
  void         renderUserObjects();
  void         setViewPoint(const KGeoCoor& deg, double mip);
  void         setMaxZoomSpeed(double);
  void         addMap(QString path, bool load_now);
  const KPack* getWorldMap();
  void         scroll(QPoint diff);
  void         scrollTo(const KGeoCoor& coor);
  void         zoomIn();
  void         zoomOut();
  QPoint       deg2scr(const KGeoCoor&) const;
  QPoint       deg2pix(const KGeoCoor&) const;
  KGeoCoor     scr2deg(const QPoint&) const;
  double       getMip();
};
#endif  // KRENDERWIDGET_H
