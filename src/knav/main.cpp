#include <math.h>
#include <QApplication>
#include <QStandardPaths>
#include <QScreen>
#include <QDir>
#include <QDebug>
#include <QProcess>
#include "krenderwidget.h"
#include "kautoscroll.h"
#include "kcontrols.h"
#include "kfindobject.h"
#include "keditwidget.h"
#include "kmap.h"
#include "kposgenerator.h"
#include "ktrackmanager.h"
#include "knewobjectwidget.h"
#include "kmapfetcher.h"

#ifdef BUILD_WITH_SENSORS
  #include <QGeoPositionInfoSource>
  #include <QGeoCoordinate>
  #include "kpositionlabel.h"
  #include "kheading.h"
#endif

using namespace kmath;

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  QSysInfo si;
  auto     is_device = si.productType().toLower().contains("android");

  auto   screen          = QGuiApplication::screens().first();
  QSize  screen_size_pix = screen->availableSize();
  QSizeF screen_size_mm  = screen->physicalSize();

  auto   physical_diag_mm = sqrt(pow(screen_size_mm.width(), 2) +
                                 pow(screen_size_mm.height(), 2));
  auto   pixel_diag       = sqrt(pow(screen_size_pix.width(), 2) +
                                 pow(screen_size_pix.height(), 2));
  double pixel_size_mm    = physical_diag_mm / pixel_diag;

  KFindWidget findw;
  findw.setFixedSize(screen_size_pix);

  KEditWidget editw;
  editw.setFixedSize(screen_size_pix);
  editw.move((screen_size_pix.width() - editw.width()) / 2,
             (screen_size_pix.height() - editw.height()) / 2);

  KNewObjectWidget newobjw;
  newobjw.setFixedSize(screen_size_pix);

  QString     mmc_path;
  QStringList dir_list;
  if (is_device)
  {
    QStringList dir_list = QStandardPaths::standardLocations(
        QStandardPaths::AppDataLocation);
    for (auto dir: dir_list)
    {
      if (QDir(dir + "/maps").exists())
      {
        mmc_path = dir;
        break;
      }
    }
  }
  else
    mmc_path = argv[1];

  KRenderWidget::Settings mapw_settings;
  mapw_settings.map_dir                 = mmc_path + "/maps";
  mapw_settings.pixel_size_mm           = pixel_size_mm;
  mapw_settings.window_size             = screen_size_pix;
  mapw_settings.min_object_size_pix     = 40;
  mapw_settings.render_window_size_coef = 2;
  mapw_settings.update_interval_ms      = 100;

  KRenderWidget mapw(mapw_settings);
  KMapFetcher map_fetcher(mapw_settings.map_dir, mapw.getWorldMap());
  KShapeManager          kvo_shape_man(mmc_path + "/class");
  KTrackManager          track_man(mmc_path + "/tracks");
  KPortableObjectManager object_man(mmc_path + "/objects",
                                    pixel_size_mm);
  KAutoScroll            auto_scroll;

  QObject::connect(&map_fetcher, &KMapFetcher::fetched,
                   [&mapw](QString map_path)
                   {
                     mapw.addMap(map_path, true);
                     mapw.render();
                   });

  QDir        dir(mmc_path + "/class");
  QStringList filters;
  filters << "*.json";
  dir.setNameFilters(filters);

  auto path_list = dir.entryList();
  for (auto path: path_list)
    kvo_shape_man.loadShapes(mmc_path + "/class/" + path,
                             mmc_path + "/class");

  QObject::connect(&editw, &KEditWidget::saveTrack, &track_man,
                   &KTrackManager::saveTrack);

  KGeoCoor start_lat_lon = KGeoCoor::fromDegs(59.9769195, 30.3642851);

  QObject::connect(&findw, &KFindWidget::find, &mapw,
                   &KRenderWidget::find);
  QObject::connect(&findw, &KFindWidget::showObject, &mapw,
                   &KRenderWidget::showObject);
  QObject::connect(&findw, &KFindWidget::showCategory, &mapw,
                   &KRenderWidget::showCategory);
  QObject::connect(&findw, &KFindWidget::getCategories, &mapw,
                   &KRenderWidget::getCategories);

  QObject::connect(&mapw, &KRenderWidget::mousePressed, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&mapw, &KRenderWidget::mouseMoved, &auto_scroll,
                   &KAutoScroll::accumulate);
  QObject::connect(&mapw, &KRenderWidget::mouseReleased, &auto_scroll,
                   &KAutoScroll::start);
  QObject::connect(&mapw, &KRenderWidget::tapped, &object_man,
                   &KPortableObjectManager::addPoint);
  QObject::connect(&mapw, &KRenderWidget::pinchStarted, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&mapw, &KRenderWidget::startedRender, &map_fetcher,
                   &KMapFetcher::requestRect);
  QObject::connect(&auto_scroll, &KAutoScroll::scroll, &mapw,
                   &KRenderWidget::scroll);

  double edge_mm        = 15;
  double step_mm        = 30;
  double button_size_mm = 15;
  if (is_device)
  {
    edge_mm        = 7;
    step_mm        = 20;
    button_size_mm = 7;
  }

  KControls::Settings control_settings;
  control_settings.map_widget     = &mapw;
  control_settings.find_widget    = &findw;
  control_settings.edge_mm        = edge_mm;
  control_settings.step_mm        = step_mm;
  control_settings.button_size_mm = button_size_mm;
  control_settings.pixel_size_mm  = pixel_size_mm;

  KControls controls(control_settings);
  QObject::connect(&controls, &KControls::zoomIn, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&controls, &KControls::zoomIn, &mapw,
                   &KRenderWidget::zoomIn);
  QObject::connect(&controls, &KControls::zoomOut, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&controls, &KControls::zoomOut, &mapw,
                   &KRenderWidget::zoomOut);
  QObject::connect(&controls, &KControls::scrollTo, &mapw,
                   &KRenderWidget::scrollTo);
  QObject::connect(&controls, &KControls::switchRecording, &track_man,
                   &KTrackManager::onSwitchRecording);
  QObject::connect(&controls, &KControls::acceptObject, &object_man,
                   &KPortableObjectManager::acceptObject);
  QObject::connect(&track_man, &KTrackManager::switchRecording,
                   [&track_man, &editw]()
                   {
                     if (!track_man.isRecording())
                       editw.show(
                           "Enter track name",
                           QDateTime::currentDateTime().toString(
                               "yyyy-MM-dd,hh-mm-ss"));
                   });
  QObject::connect(&controls, &KControls::isRecording, &track_man,
                   &KTrackManager::isRecording);
  QObject::connect(&controls, &KControls::selectShape, &newobjw,
                   &KNewObjectWidget::show);

  QObject::connect(&newobjw, &KNewObjectWidget::getUserShapeImageList,
                   &kvo_shape_man, &KShapeManager::getShapeImageList);
  QObject::connect(&newobjw, &KNewObjectWidget::selectedShape,
                   &object_man,
                   &KPortableObjectManager::createObject);
  QObject::connect(&newobjw, &KNewObjectWidget::getShapeById,
                   &kvo_shape_man, &KShapeManager::getShapeById);

  QObject::connect(&mapw, &KRenderWidget::zoomFinished, &controls,
                   &KControls::checkZoomRepeat);
  QObject::connect(&mapw, &KRenderWidget::mouseMoved, &controls,
                   &KControls::onMouseMoved);
  QObject::connect(&mapw, &KRenderWidget::movedCenterTo, &controls,
                   &KControls::setCurrCoor);

#ifdef BUILD_WITH_SENSORS
  KPositionLabel   position_label(&mapw);
  QGeoPositionInfo info;
  info.setCoordinate({59.9769195, 30.3642851});
  position_label.updateGeoPosition(info);
  QObject::connect(&mapw, &KRenderWidget::modified, &position_label,
                   &KPositionLabel::update);
  QObject::connect(&mapw, &KRenderWidget::paintObjects, &track_man,
                   &KTrackManager::paint, Qt::DirectConnection);
  QObject::connect(&mapw, &KRenderWidget::paintObjects, &object_man,
                   &KPortableObjectManager::paint,
                   Qt::DirectConnection);
  QObject::connect(&track_man, &KTrackManager::kcoor2pix, &mapw,
                   &KRenderWidget::kcoor2pix, Qt::DirectConnection);
  QObject::connect(&object_man, &KPortableObjectManager::kcoor2pix,
                   &mapw, &KRenderWidget::kcoor2pix,
                   Qt::DirectConnection);
  QObject::connect(&position_label, &KPositionLabel::deg2pix, &mapw,
                   &KRenderWidget::deg2pix, Qt::DirectConnection);

  QObject::connect(&track_man, &KTrackManager::updated, &mapw,
                   &KRenderWidget::render);
  QObject::connect(&object_man, &KPortableObjectManager::updated,
                   &mapw, &KRenderWidget::render);
  QObject::connect(&object_man, &KPortableObjectManager::finishEdit,
                   &controls, &KControls::finishEdit);

  QGeoPositionInfoSource* geo =
      QGeoPositionInfoSource::createDefaultSource(&mapw);
  if (geo)
  {
    QObject::connect(geo, &QGeoPositionInfoSource::positionUpdated,
                     &position_label,
                     &KPositionLabel::updateGeoPosition);
    QObject::connect(geo, &QGeoPositionInfoSource::positionUpdated,
                     [&controls](const QGeoPositionInfo& geo)
                     {
                       auto c = geo.coordinate();
                       controls.updatePosition(KGeoCoor::fromDegs(
                           c.latitude(), c.longitude()));
                       controls.update();
                     });

    QObject::connect(
        geo, &QGeoPositionInfoSource::positionUpdated,
        [&track_man](const QGeoPositionInfo& geo)
        {
          auto c = geo.coordinate();
          if (track_man.isRecording())
            track_man.addPoint(KPosition{
                KGeoCoor::fromDegs(c.latitude(), c.longitude()),
                static_cast<float>(c.altitude()),
                KDateTime(QDateTime::currentDateTime())});
        });

    geo->startUpdates();
  }

  QMagnetometer    magnetometer(&mapw);
  KHeadingProvider heading_provider(&magnetometer);
  QObject::connect(&magnetometer, &QMagnetometer::readingChanged,
                   &heading_provider, &KHeadingProvider::update);
  QObject::connect(&heading_provider,
                   &KHeadingProvider::headingChanged, &position_label,
                   &KPositionLabel::updateHeading);
  magnetometer.setReturnGeoValues(true);
  magnetometer.start();

  KPosGenerator pos_gen(start_lat_lon,
                        KGeoCoor::fromDegs(0.00001, 0.00001));
  if (!is_device)
  {
    QObject::connect(&pos_gen, &KPosGenerator::generated_pos,
                     &track_man, &KTrackManager::addPoint);
    QObject::connect(&pos_gen, &KPosGenerator::generated_coor,
                     &controls, &KControls::updatePosition);
    QObject::connect(&pos_gen, &KPosGenerator::generated_coor,
                     &position_label,
                     &KPositionLabel::updatePosition);
    QObject::connect(&pos_gen, &KPosGenerator::generated_angle,
                     &heading_provider,
                     &KHeadingProvider::headingChanged);
  }

#endif

  mapw.show();
  mapw.setViewPoint(start_lat_lon, 1);

  return a.exec();
}
