#include <QDebug>
#include <QMessageBox>
#include "kcontrols.h"

void KControls::initButton(QPushButton* b, QPixmap pm, QPoint pos,
                           double size_mm)
{
  int  size_pix = size_mm / KShape::pixel_size_mm;
  auto scaled_pm =
      pm.scaledToWidth(size_pix, Qt::SmoothTransformation);
  b->setFixedSize(size_pix, size_pix);
  b->setIconSize({size_pix, size_pix});
  b->setIcon(scaled_pm);
  b->move({pos.x() - b->width() / 2, pos.y() - b->height() / 2});
  QRect rect(10, 10, scaled_pm.width() - 20, scaled_pm.height() - 20);
  QRegion region(rect, QRegion::Ellipse);
  b->setMask(region);
  b->show();
}

void KControls::setIcon(QPushButton* b, QPixmap pm)
{
  b->setIcon(pm);
}

void KControls::onFind()
{
  findw->show();
}

void KControls::onZoomIn()
{
  zoom_mode = In;
  zoomIn();
}

void KControls::onZoomOut()
{
  zoom_mode = Out;
  zoomOut();
}

void KControls::onZoomReleased()
{
  zoom_mode = None;
}

void KControls::enableCentering()
{
  centering_enabled = true;
  scrollTo(curr_coor);
}

void KControls::onSwitchRecording()
{
  switchRecording();
  if (isRecording())
    setIcon(&record,
            QPixmap(":/labels/recordon.jpg").scaledToWidth(50));
  else
    setIcon(&record,
            QPixmap(":/labels/recordoff.png").scaledToWidth(50));
}

void KControls::onMouseMoved()
{
  centering_enabled = false;
}

void KControls::setCurrCoor(const KGeoCoor& v)
{
  curr_coor = v;
}

void KControls::finishEdit()
{
  record.show();
  ok.hide();
}

void KControls::updatePosition(const KGeoCoor& v)
{
  setCurrCoor(v);
  update();
}

void KControls::update()
{
  if (centering_enabled)
    scrollTo(curr_coor);
}

KControls::KControls(QWidget* mapw, QWidget* _findw, double edge_mm,
                     double step_mm, double button_size_mm):
    find(mapw),
    zoom_in(mapw), zoom_out(mapw), center_position(mapw),
    record(mapw), add(mapw), ok(mapw)
{
  findw = _findw;

  int edge = edge_mm / KShape::pixel_size_mm;
  int step = step_mm / KShape::pixel_size_mm;

  initButton(&find, QPixmap(":/labels/find.png"),
             {mapw->width() - edge, mapw->height() / 2 - step * 2},
             button_size_mm);
  connect(&find, &QPushButton::pressed, findw, &QWidget::show);

  auto       pm = QPixmap(":/labels/compass.png");
  QTransform tr;
  tr.translate(pm.width() / 2, pm.height() / 2);
  tr.rotate(-90);
  tr.translate(-pm.width() / 2, -pm.height() / 2);

  initButton(&zoom_in, QPixmap(":/labels/plus.png"),
             {mapw->width() - edge, mapw->height() / 2},
             button_size_mm);
  connect(&zoom_in, &QPushButton::pressed, this,
          &KControls::onZoomIn);
  connect(&zoom_in, &QPushButton::released, this,
          &KControls::onZoomReleased);
  initButton(&zoom_out, QPixmap(":/labels/minus.png"),
             {mapw->width() - edge, mapw->height() / 2 + step},
             button_size_mm);
  connect(&zoom_out, &QPushButton::pressed, this,
          &KControls::onZoomOut);
  connect(&zoom_out, &QPushButton::released, this,
          &KControls::onZoomReleased);
  initButton(&center_position, QPixmap(":/labels/center.png"),
             {mapw->width() - edge, mapw->height() / 2 + step * 2},
             button_size_mm);
  connect(&center_position, &QPushButton::pressed, this,
          &KControls::enableCentering);

  initButton(&record, QPixmap(":/labels/recordoff.png"),
             {edge, mapw->height() / 2 + step * 2}, button_size_mm);
  connect(&record, &QPushButton::pressed, this,
          &KControls::onSwitchRecording);

  initButton(&add, QPixmap(":/labels/plus.png"),
             {mapw->width() / 2, mapw->height() / 2 + step * 2},
             button_size_mm);
  connect(&add, &QPushButton::pressed, this, &KControls::selectShape);
  connect(&add, &QPushButton::pressed, &record, &QWidget::hide);
  connect(&add, &QPushButton::pressed, &ok, &QWidget::show);
  initButton(&ok, QPixmap(":/labels/ok.png"),
             {mapw->width() / 2, mapw->height() / 2 + step * 2},
             button_size_mm);
  connect(&ok, &QPushButton::pressed, &record, &QWidget::show);
  connect(&ok, &QPushButton::pressed, &ok, &QWidget::hide);
  connect(&ok, &QPushButton::pressed, this, &KControls::acceptObject);
  ok.hide();
}

void KControls::checkZoomRepeat()
{
  if (zoom_mode == In)
    zoomIn();
  if (zoom_mode == Out)
    zoomOut();
}
