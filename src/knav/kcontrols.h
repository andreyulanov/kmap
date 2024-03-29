#ifndef KCONTROLS_H
#define KCONTROLS_H

#include <QPushButton>
#include <QLineEdit>
#include <QElapsedTimer>
#include "kmap.h"
#include "kfindwidget.h"

class KControls: public QObject
{
  Q_OBJECT

public:
  struct Settings
  {
    QWidget* map_widget     = nullptr;
    QWidget* find_widget    = nullptr;
    double   edge_mm        = 0;
    double   step_mm        = 0;
    double   button_size_mm = 0;
    double   pixel_size_mm  = 0;
  };

private:
  enum ZoomMode
  {
    None,
    In,
    Out
  };

  enum OrientMode
  {
    North,
    Heading
  };

  bool        centering_enabled = false;
  ZoomMode    zoom_mode         = None;
  QPushButton find;
  QPushButton zoom_in;
  QPushButton zoom_out;
  QPushButton center_position;
  QPushButton record;
  QPushButton add;
  QPushButton ok;
  KGeoCoor    curr_coor;
  Settings    settings;

  void initButton(QPushButton* b, QPixmap image_path, QPoint pos,
                  double size_mm);
  void setIcon(QPushButton* b, QPixmap pm);
  void onFind();
  void onZoomIn();
  void onZoomOut();
  void onZoomReleased();

signals:
  void zoomIn();
  void zoomOut();
  void zoomReleased();
  void scrollTo(const KGeoCoor&);
  void setRotation(double);
  void changedFindString(const QString&);
  void switchRecording();
  bool isRecording();
  void selectShape();
  void acceptObject();

public:
  KControls(Settings settings);
  void setCurrCoor(const KGeoCoor&);
  void update();
  void updatePosition(const KGeoCoor&);
  void checkZoomRepeat();
  void enableCentering();
  void onSwitchRecording();
  void onMouseMoved();
  void finishEdit();
};

#endif  // KCONTROLS_H
