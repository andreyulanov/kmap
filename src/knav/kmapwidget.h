#ifndef KMAPWIDGET_H
#define KMAPWIDGET_H

#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QTimer>
#include <QGestureEvent>
#include <QWheelEvent>
#include "krender.h"
#include "kportableobject.h"
#include <QOpenGLWidget>

class KMapWidget: public QWidget
{
  Q_OBJECT

  typedef QWidget base;

  enum ZoomMode
  {
    None,
    In,
    Out
  };

  ZoomMode      zoom_mode                            = None;
  double        zoom_speed                           = 1.2;
  double        max_zoom_speed                       = 1.2;
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
  double rotation = 0;

  bool    zoom_pixmap_rendered = false;
  bool    scrolling_enabled    = true;
  QLabel  label;
  QLabel  full_label;
  QLabel  scaled_label;
  KRender r;

  void mousePressEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void wheelEvent(QWheelEvent* event) override;
  bool event(QEvent* e) override;
  void checkZoomFinished();
  void updateLabel(const QPixmap*, int ms_elapsed);
  void scaleLabel();
  bool canScroll();

  void   stepZoom();
  void   onRendered(int ms_elapsed);
  void   startZoom(ZoomMode, QPoint focus_shift = QPoint());
  QPoint getTotalShift() const;

signals:
  void    modified();
  void    mousePressed(QPoint);
  void    mouseMoved(QPoint);
  void    mouseReleased();
  void    pinchStarted();
  QPointF getAutoScrollSpeed();
  void    zoomFinished();
  void    movedCenterTo(const KGeoCoor&);
  void    paintObjects(QPainter*);
  void    tapped(KGeoCoor);

public:
  KMapWidget(QSize s);
  void        render();
  void        setViewPoint(const KGeoCoor& deg, double mip);
  void        setMaxZoomSpeed(double);
  const KMap* appendMap(QString path, double min_mip, double max_mip,
                        bool load_now);
  void        scroll(QPoint diff);
  void        scrollTo(const KGeoCoor& coor);
  void        zoomIn();
  void        zoomOut();
  QPoint      deg2pix(const KGeoCoor&) const;
  QPoint      kcoor2pix(const KGeoCoor&) const;
  KGeoCoor    pix2deg(const QPoint&) const;
  void        setRotation(double);
  void        enableRotation();
  void        disableRotation();
  QStringList find(const QString&);
  void        showObject(const QString&);
  void        showCategory(const QString&);
  KCategories getCategories() const;
  void        setScrollingEnabled(bool);
};
#endif  // KMAPWIDGET_H
