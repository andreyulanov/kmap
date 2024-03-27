#include "math.h"
#include "kmapwidget.h"
#include <QLabel>
#include <QDebug>
#include <QApplication>
#include <QDir>

using namespace kmath;

KMapWidget::KMapWidget(QString map_dir, QSize s):
    label(this), full_label(this), scaled_label(this)
{
  if (s.isEmpty())
    s = QSize(480, 800);
  setFixedSize(s);
  setAttribute(Qt::WA_AcceptTouchEvents);
  QApplication::setAttribute(
      Qt::AA_SynthesizeMouseForUnhandledTouchEvents, true);

  grabGesture(Qt::PinchGesture);

  zoom_timer.setInterval(50);
  connect(&zoom_timer, &QTimer::timeout, this, &KMapWidget::stepZoom);

  r.setPixmapSize(size());
  r.setBackgroundColor(QColor(166, 220, 238));

  connect(&r, &KRender::paintObjects, this, &KMapWidget::paintObjects,
          Qt::DirectConnection);
  connect(&r, &KRender::started, this, &KMapWidget::startedRender);
  connect(&r, &KRender::rendered, this, &KMapWidget::onRendered);
  scan(map_dir);
}

void KMapWidget::scan(QString map_dir)
{
  qDebug() << "scanning" << map_dir;

  QDir dir(map_dir);
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::Hidden |
                QDir::NoSymLinks | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();

  auto world_map_path = map_dir + "/world.kmap";
  addMap(world_map_path, 0, 0, true);
  for (auto fi: list)
  {
    auto path = fi.absoluteFilePath();
    if (path == world_map_path)
      continue;
    if (path.endsWith(".kmap"))
    {
      qDebug() << "adding" << path;
      addMap(path, 0, KMap::only_global_mip, false);
    }
  }
}

const KMap* KMapWidget::addMap(QString path, double min_mip,
                               double max_mip, bool load_now)
{
  return r.addMap(path, min_mip, max_mip, load_now);
}

const KMap* KMapWidget::getWorldMap()
{
  return r.getMaps()->first();
}

void KMapWidget::render()
{
  r.start();
}

void KMapWidget::setViewPoint(const KGeoCoor& deg, double mip)
{
  shifted_after_zoom = true;
  total_pan_pos      = QPoint();
  movedCenterTo(deg);
  r.setCenterM(deg.toMeters());
  r.setMip(mip);
  r.start();
}

void KMapWidget::setMaxZoomSpeed(double v)
{
  max_zoom_speed = v;
}

void KMapWidget::onRendered(int ms_elapsed)
{
  if (zoom_mode == None)
  {
    scaled_label.hide();
    scaled_label.move(QPoint());
    intermediate_zoom_coef = 1;
    updateLabel(r.getPixmap(), ms_elapsed);
  }
  else
    zoom_pixmap_rendered = true;
  modified();
}

void KMapWidget::mousePressEvent(QMouseEvent* e)
{
  if (!canScroll())
    return;
  mousePressed(e->pos());
  mouse_pos        = e->pos();
  zoom_focus_shift = QPoint();
}

void KMapWidget::scroll(QPoint diff)
{
  shifted_after_zoom = true;
  total_pan_pos += diff;
  label.move(label.pos() - diff);
  full_label.move(full_label.pos() - diff);
  modified();

  auto dx = abs(total_pan_pos.x());
  auto dy = abs(total_pan_pos.y());

  auto coef       = (r.getRenderWindowSizeCoef() - 1) / 4;
  auto speed_coef = std::clamp(
      getAutoScrollSpeed().manhattanLength() / 10, 1.0, 4.0);
  coef *= speed_coef;

  if (dx > width() * coef || dy > height() * coef)
  {
    r.pan(total_pan_pos);
    r.stopAndWait();
    r.start();
    total_pan_pos = QPoint();
  }
  if (scaled_label.isVisible())
    scaled_label.move(scaled_label.pos() - diff);
}

void KMapWidget::scrollTo(const KGeoCoor& coor)
{
  auto new_pos_pix = deg2pix(coor);
  auto diff        = new_pos_pix - QPoint(width() / 2, height() / 2);
  if (fabs(diff.x()) < width() && fabs(diff.y()) < height())
    scroll(diff);
  else
    setViewPoint(coor, r.getMip());
}

void KMapWidget::mouseMoveEvent(QMouseEvent* e)
{
  if (!canScroll())
    return;
  auto diff = QPoint(mouse_pos - e->pos());
  if (scrolling_enabled)
    scroll(diff);
  mouseMoved(e->pos());
  mouse_pos = e->pos();
}

void KMapWidget::mouseReleaseEvent(QMouseEvent* e)
{
  if (!canScroll())
    return;
  mouseReleased();
  if ((e->pos() - mouse_pos).manhattanLength() < 10)
    tapped(pix2deg(e->pos()));
}

void KMapWidget::wheelEvent(QWheelEvent* e)
{
  if (zoom_mode == ZoomMode::None)
  {
    auto focus_shift =
        e->position().toPoint() - QPoint(width() / 2, height() / 2);
    if (e->angleDelta().y() < 0)
      focus_shift = QPoint();

    startZoom(e->angleDelta().y() > 0 ? ZoomMode::In : ZoomMode::Out,
              focus_shift);
  }
}

bool KMapWidget::event(QEvent* e)
{
  if (e->type() == QEvent::Gesture)
  {
    auto ge = static_cast<QGestureEvent*>(e);
    if (ge)
    {
      auto pinch = ge->gesture(Qt::PinchGesture);
      auto pg    = static_cast<QPinchGesture*>(pinch);
      if (pg)
      {
        auto change_flags = pg->changeFlags();
        if (pg->state() == Qt::GestureStarted)
        {
          pinchStarted();
          focus_shift = pg->centerPoint().toPoint() -
                        QPoint{width() / 2, height() / 2};
          is_pinching = true;
        }
        if (is_pinching &&
            change_flags & QPinchGesture::ScaleFactorChanged)
        {
          auto scale_factor = pg->totalScaleFactor();
          if (zoom_mode == None)
          {
            if (scale_factor < 0.95)
            {
              focus_shift = QPoint();
              startZoom(Out, focus_shift);
            }
            if (scale_factor > 1.05)
              startZoom(In, focus_shift);
          }
          if (zoom_mode == Out)
            scale_factor = 1.0 / scale_factor;
          zoom_speed = std::clamp(1 + scale_factor * 0.02, 1.0,
                                  max_zoom_speed);
        }
        if (pg->state() == Qt::GestureFinished)
        {
          is_pinching = false;
          time_since_last_pinch.start();
        }
      }
      return true;
    }
    return QWidget::event(e);
  }
  return QWidget::event(e);
}

void KMapWidget::updateLabel(const QPixmap* pm, int ms_elapsed)
{
  if (!pm)
    return;

  auto   window_size = pm->size() / r.getRenderWindowSizeCoef();
  QPoint pos{(window_size.width() - pm->width()) / 2,
             (window_size.height() - pm->height()) / 2};

  auto pan_diff_m   = r.getRenderCenterM() - r.getCenterM();
  auto pan_diff_pix = pan_diff_m / r.getMip();

  if (!zoom_focus_shift.isNull())
    total_pan_pos = -zoom_focus_shift;

  QPoint total_shift;
  total_shift += pan_diff_pix.toPoint();
  total_shift -= total_pan_pos;

  auto updateLabelPixmap = [=, this](auto l)
  {
    l->setFixedSize(pm->size());

    if (rotation == 0)
      l->setPixmap(*pm);
    else
    {
      QPixmap rpm(pm->size());
      rpm.fill(Qt::transparent);
      QPainter p(&rpm);
      auto     center = QPoint{pm->width() / 2, pm->height() / 2};
      center += total_pan_pos;
      p.translate(center);
      p.rotate(rotation);
      p.translate(-center);
      p.drawPixmap(0, 0, *pm);
      l->setPixmap(rpm);
    }
    l->move(pos + total_shift);
    l->show();
  };
  updateLabelPixmap(&label);

  if (ms_elapsed == 0 || !shifted_after_zoom)
    updateLabelPixmap(&full_label);

  auto coef = (r.getRenderWindowSizeCoef() - 1) / 2;

  if (fabs(total_shift.x()) > window_size.width() * coef ||
      fabs(total_shift.y()) > window_size.height() * coef)
    r.start();
}

void KMapWidget::checkZoomFinished()
{
  if ((zoom_mode == In &&
       intermediate_zoom_coef < 1.0 / r.getRenderWindowSizeCoef()) ||
      (zoom_mode == Out &&
       intermediate_zoom_coef > r.getRenderWindowSizeCoef()))
  {
    if (r.isRunning())
    {
      r.stopAndWait();
      r.start();
    }
    updateLabel(r.getPixmap(), 0);
    zoom_pixmap_rendered = false;
    zoom_mode            = None;
    scaled_label.hide();
    intermediate_zoom_coef = 1;
    zoom_timer.stop();
    zoomFinished();
  }
}

void KMapWidget::stepZoom()
{
  double coef = 1;
  if (zoom_mode == In)
    coef = 1.0 / zoom_speed;
  if (zoom_mode == Out)
    coef = zoom_speed;
  intermediate_zoom_coef *= coef;
  checkZoomFinished();
  scaleLabel();
  modified();
}

void KMapWidget::startZoom(KMapWidget::ZoomMode mode,
                           QPoint               focus_shift)
{
  if (zoom_mode != ZoomMode::None)
    return;

  zoom_speed         = max_zoom_speed;
  shifted_after_zoom = false;

  r.stopAndWait();
  zoom_mode            = mode;
  zoom_pixmap_rendered = false;
  double coef          = r.getRenderWindowSizeCoef();
  if (mode == In)
    coef = 1.0 / coef;

  if (!total_pan_pos.isNull())
  {
    auto    orig_pm = label.pixmap(Qt::ReturnByValue);
    QPixmap pm{orig_pm.size()};
    pm.fill(Qt::white);
    QPainter p(&pm);
    p.drawPixmap(-total_pan_pos, orig_pm);
    p.end();
    label.setPixmap(pm);
    label.move(
        {(width() - pm.width()) / 2, (height() - pm.height()) / 2});
  }
  zoom_focus_shift = focus_shift;
  r.pan(zoom_focus_shift);
  r.pan(total_pan_pos);
  total_pan_pos = QPoint();
  r.zoom(coef);
  if (mode == Out)
    r.enableLoading(false);
  r.start();
  zoom_timer.start();
}

void KMapWidget::zoomIn()
{
  startZoom(ZoomMode::In);
}

void KMapWidget::zoomOut()
{
  startZoom(ZoomMode::Out);
}

bool KMapWidget::canScroll()
{
  if (is_pinching)
    return false;
  return !time_since_last_pinch.isValid() ||
         time_since_last_pinch.elapsed() >
             min_time_between_pinch_and_scroll_ms;
}

void KMapWidget::scaleLabel()
{
  auto   screen_size = label.size() / r.getRenderWindowSizeCoef();
  double max_shift_x = screen_size.width() / 2;
  double max_shift_y = screen_size.height() / 2;

  double rx = fabs(zoom_focus_shift.x()) / max_shift_x;
  double ry = fabs(zoom_focus_shift.y()) / max_shift_y;

  double coef = 1 + sqrt(pow(rx, 2) + pow(ry, 2));

  screen_size *= coef;

  auto   scaled_screen_size = screen_size * intermediate_zoom_coef;
  QPoint scaled_start_pos{
      (screen_size.width() - scaled_screen_size.width()) / 2,
      (screen_size.height() - scaled_screen_size.height()) / 2};
  scaled_start_pos += zoom_focus_shift;

  auto pos = (label.size() - screen_size) / 2;

  scaled_start_pos += {pos.width(), pos.height()};

  auto pm = label.pixmap(Qt::ReturnByValue);

  auto scaled_pm =
      pm.copy(scaled_start_pos.x() + total_pan_pos.x(),
              scaled_start_pos.y() + total_pan_pos.y(),
              scaled_screen_size.width(), scaled_screen_size.height())
          .scaled(screen_size, Qt::KeepAspectRatio,
                  Qt::SmoothTransformation);
  scaled_label.setFixedSize(screen_size);

  //  QPixmap rpm(scaled_pm.size());
  //  rpm.fill(Qt::transparent);
  //  QPainter p(&rpm);
  //  auto center = QPoint{scaled_pm.width() / 2, scaled_pm.height() /
  //  2}; center += total_pan_pos; p.translate(center);
  //  p.rotate(rotation);
  //  p.translate(-center);
  //  p.drawPixmap(0, 0, scaled_pm);
  //  scaled_label.setPixmap(rpm);

  scaled_label.setPixmap(scaled_pm);
  auto shift =
      label.size() / r.getRenderWindowSizeCoef() * (coef - 1) * 0.5;

  scaled_label.move(zoom_focus_shift -
                    QPoint{shift.width(), shift.height()});
  scaled_label.show();
}

void KMapWidget::setScrollingEnabled(bool v)
{
  scrolling_enabled = v;
}

KCategories KMapWidget::getCategories() const
{
  return r.getMaps()->getCategories();
}

void KMapWidget::showCategory(const QString& v)
{
  r.selectCategory(v);
  r.start();
}

void KMapWidget::showObject(const QString& object_name)
{
  auto coor = r.getMaps()->getCoorByName(object_name);
  if (coor.isValid())
    scrollTo(coor);
}

QStringList KMapWidget::find(const QString& str)
{
  return r.getMaps()->find(str);
}

void KMapWidget::setRotation(double v)
{
  if (!r.getPixmap() || r.isRunning())
    return;
  QPixmap pm(r.getPixmap()->size());
  pm.fill(Qt::transparent);
  QPainter p(&pm);
  auto     center = QPoint{pm.width() / 2, pm.height() / 2};
  center += total_pan_pos;
  p.translate(center);
  p.rotate(v);
  p.translate(-center);
  p.drawPixmap(0, 0, *r.getPixmap());
  label.setPixmap(pm);
  full_label.setPixmap(pm);
  rotation = v;
}

void KMapWidget::enableRotation()
{
  r.stopAndWait();
  r.setRenderWindowSizeCoef(2);
  r.setPixmapSize(size());
  render();
  r.wait();
}

void KMapWidget::disableRotation()
{
  r.stopAndWait();
  r.setRenderWindowSizeCoef(1.5);
  r.setPixmapSize(size());
  render();
  r.wait();
}

QPoint KMapWidget::getTotalShift() const
{
  auto   coef = r.getRenderWindowSizeCoef();
  QPoint pos{int((width() * (1 - coef)) / 2),
             int((height() * (1 - coef) / 2))};
  auto   pan_diff_m   = r.getRenderCenterM() - r.getCenterM();
  auto   pan_diff_pix = pan_diff_m / r.getMip();

  auto total_shift = pos;
  total_shift += pan_diff_pix.toPoint();
  total_shift -= total_pan_pos;
  return total_shift;
}

QPoint KMapWidget::kcoor2pix(const KGeoCoor& deg) const
{
  return r.kcoor2pix(deg);
}

QPoint KMapWidget::deg2pix(const KGeoCoor& deg) const
{
  auto coef          = r.getRenderWindowSizeCoef();
  auto total_shift   = getTotalShift();
  auto pos_on_pixmap = r.deg2pix(deg);
  auto pos_on_screen = pos_on_pixmap + total_shift;
  if (zoom_mode != None)
  {
    auto focus_pos_on_screen = QPoint{width() / 2, height() / 2};
    auto diff                = pos_on_screen - focus_pos_on_screen;
    if (zoom_mode == In)
      diff /= coef;
    else
      diff *= coef;
    auto prev_pos_on_screen = focus_pos_on_screen + diff;
    diff          = prev_pos_on_screen - focus_pos_on_screen;
    pos_on_screen = focus_pos_on_screen +
                    diff / intermediate_zoom_coef + zoom_focus_shift;
  }
  return pos_on_screen;
}

KGeoCoor KMapWidget::pix2deg(const QPoint& pix) const
{
  return r.pix2deg(pix - getTotalShift());
}
