#include <QDebug>
#include <QMessageBox>
#include "kcontrols.h"

void KControls::initButton(QPushButton* b, QPixmap pm, QPoint pos,
                           double size_mm)
{
  int  size_pix = size_mm / settings.pixel_size_mm;
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

void KControls::startEdit()
{
  record.hide();
  ok.show();
  add.hide();
  remove.show();
}

void KControls::finishEdit()
{
  record.show();
  ok.hide();
  add.show();
  remove.hide();
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

KControls::KControls(Settings v):
    zoom_in(v.map_widget), zoom_out(v.map_widget),
    center_position(v.map_widget), record(v.map_widget),
    add(v.map_widget), remove(v.map_widget), ok(v.map_widget),
    login_button(v.map_widget)
{
  settings           = v;
  auto mapw          = settings.map_widget;
  auto pixel_size_mm = settings.pixel_size_mm;

  int edge = settings.edge_mm / pixel_size_mm;
  int step = settings.step_mm / pixel_size_mm;

  auto       pm = QPixmap(":/labels/compass.png");
  QTransform tr;
  tr.translate(pm.width() / 2, pm.height() / 2);
  tr.rotate(-90);
  tr.translate(-pm.width() / 2, -pm.height() / 2);

  initButton(&zoom_in, QPixmap(":/labels/plus.png"),
             {mapw->width() - edge, mapw->height() / 2},
             settings.button_size_mm);
  connect(&zoom_in, &QPushButton::pressed, this,
          &KControls::onZoomIn);
  connect(&zoom_in, &QPushButton::released, this,
          &KControls::onZoomReleased);
  initButton(&zoom_out, QPixmap(":/labels/minus.png"),
             {mapw->width() - edge, mapw->height() / 2 + step},
             settings.button_size_mm);
  connect(&zoom_out, &QPushButton::pressed, this,
          &KControls::onZoomOut);
  connect(&zoom_out, &QPushButton::released, this,
          &KControls::onZoomReleased);
  initButton(&center_position, QPixmap(":/labels/center.png"),
             {mapw->width() - edge, mapw->height() / 2 + step * 2},
             settings.button_size_mm);
  connect(&center_position, &QPushButton::pressed, this,
          &KControls::enableCentering);

  initButton(&record, QPixmap(":/labels/recordoff.png"),
             {edge, mapw->height() / 2 + step * 2},
             settings.button_size_mm);
  connect(&record, &QPushButton::pressed, this,
          &KControls::onSwitchRecording);

  initButton(&add, QPixmap(":/labels/plus.png"),
             {mapw->width() / 2, mapw->height() / 2 + step * 2},
             settings.button_size_mm);
  connect(&add, &QPushButton::pressed, this, &KControls::selectClass);
  connect(&add, &QPushButton::pressed, &record, &QWidget::hide);
  connect(&add, &QPushButton::pressed, &ok, &QWidget::show);

  initButton(&remove, QPixmap(":/labels/minus.png"),
             {static_cast<int>(mapw->width() * 0.75),
              mapw->height() / 2 + step * 2},
             settings.button_size_mm);
  connect(&remove, &QPushButton::pressed, this,
          &KControls::removeObject);
  remove.hide();

  initButton(&ok, QPixmap(":/labels/ok.png"),
             {mapw->width() / 2, mapw->height() / 2 + step * 2},
             settings.button_size_mm);
  connect(&ok, &QPushButton::pressed, &record, &QWidget::show);
  connect(&ok, &QPushButton::pressed, &ok, &QWidget::hide);
  connect(&ok, &QPushButton::pressed, this, &KControls::acceptObject);
  ok.hide();

  initButton(&login_button, QPixmap(":/labels/in.png"),
             {edge, mapw->height() / 2 - step * 2},
             settings.button_size_mm);
  connect(&login_button, &QPushButton::pressed, this,
          &KControls::login);
}

void KControls::checkZoomRepeat()
{
  if (zoom_mode == In)
    zoomIn();
  if (zoom_mode == Out)
    zoomOut();
}
