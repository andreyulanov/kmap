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
#include "keditwidget.h"
#include "kmap.h"
#include "kposgenerator.h"
#include "ktrackmanager.h"
#include "knewobjectwidget.h"
#include "kmapfetcher.h"
#include "kscalelabel.h"
#include "kxmppclient.h"
#include "kloginwidget.h"
#include "krosterwidget.h"
#include <kportableobjectsender.h>
#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppLogger.h>
#include <qxmpp/QXmppRosterManager.h>

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

  if (!is_device)
    pixel_size_mm /= 2;

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
  mapw_settings.render_window_size_coef = 2;
  mapw_settings.update_interval_ms      = 100;

  KRenderWidget  renderw(mapw_settings);
  KMapFetcher    map_fetcher(mapw_settings.map_dir,
                             renderw.getWorldMap());
  KShapeManager  kvo_shape_man(mmc_path + "/class");
  KTrackManager  track_man(mmc_path + "/tracks");
  KObjectManager object_man(mmc_path + "/objects", pixel_size_mm);
  KAutoScroll    auto_scroll;

  QObject::connect(&map_fetcher, &KMapFetcher::fetched,
                   [&renderw](QString map_path)
                   {
                     renderw.addMap(map_path, true);
                     renderw.renderMap();
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

  QObject::connect(&renderw, &KRenderWidget::mousePressed,
                   &auto_scroll, &KAutoScroll::stop);
  QObject::connect(&renderw, &KRenderWidget::mouseMoved, &auto_scroll,
                   &KAutoScroll::accumulate);
  QObject::connect(&renderw, &KRenderWidget::mouseReleased,
                   &auto_scroll, &KAutoScroll::start);
  QObject::connect(&renderw, &KRenderWidget::tapped, &object_man,
                   &KObjectManager::addPoint);
  QObject::connect(&renderw, &KRenderWidget::pinchStarted,
                   &auto_scroll, &KAutoScroll::stop);
  QObject::connect(&renderw, &KRenderWidget::startedRender,
                   &map_fetcher, &KMapFetcher::requestRect);
  QObject::connect(&auto_scroll, &KAutoScroll::scroll, &renderw,
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
  control_settings.map_widget     = &renderw;
  control_settings.edge_mm        = edge_mm;
  control_settings.step_mm        = step_mm;
  control_settings.button_size_mm = button_size_mm;
  control_settings.pixel_size_mm  = pixel_size_mm;

  KControls controls(control_settings);
  QObject::connect(&controls, &KControls::zoomIn, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&controls, &KControls::zoomIn, &renderw,
                   &KRenderWidget::zoomIn);
  QObject::connect(&controls, &KControls::zoomOut, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&controls, &KControls::zoomOut, &renderw,
                   &KRenderWidget::zoomOut);
  QObject::connect(&controls, &KControls::scrollTo, &renderw,
                   &KRenderWidget::scrollTo);
  QObject::connect(&controls, &KControls::switchRecording, &track_man,
                   &KTrackManager::onSwitchRecording);
  QObject::connect(&controls, &KControls::acceptObject, &object_man,
                   &KObjectManager::acceptObject);
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
                   &object_man, &KObjectManager::createObject);
  QObject::connect(&newobjw, &KNewObjectWidget::getShapeById,
                   &kvo_shape_man, &KShapeManager::getShapeById);

  QObject::connect(&renderw, &KRenderWidget::zoomFinished, &controls,
                   &KControls::checkZoomRepeat);
  QObject::connect(&renderw, &KRenderWidget::mouseMoved, &controls,
                   &KControls::onMouseMoved);
  QObject::connect(&renderw, &KRenderWidget::movedCenterTo, &controls,
                   &KControls::setCurrCoor);

  KScaleLabel scale_label(&renderw);
  QObject::connect(&scale_label, &KScaleLabel::getMip, &renderw,
                   &KRenderWidget::getMip, Qt::DirectConnection);

#ifdef BUILD_WITH_SENSORS
  KPositionLabel   position_label(&renderw);
  QGeoPositionInfo info;
  info.setCoordinate({59.9769195, 30.3642851});
  position_label.updateGeoPosition(info);
  QObject::connect(&renderw, &KRenderWidget::modified,
                   &position_label, &KPositionLabel::update);
  QObject::connect(&renderw, &KRenderWidget::paintUserObjects,
                   &track_man, &KTrackManager::paint,
                   Qt::DirectConnection);
  QObject::connect(&renderw, &KRenderWidget::paintUserObjects,
                   &object_man, &KObjectManager::paint,
                   Qt::DirectConnection);
  QObject::connect(&track_man, &KTrackManager::kcoor2pix, &renderw,
                   &KRenderWidget::kcoor2pix, Qt::DirectConnection);
  QObject::connect(&object_man, &KObjectManager::kcoor2pix, &renderw,
                   &KRenderWidget::kcoor2pix, Qt::DirectConnection);
  QObject::connect(&position_label, &KPositionLabel::deg2pix,
                   &renderw, &KRenderWidget::deg2pix,
                   Qt::DirectConnection);

  QObject::connect(&track_man, &KTrackManager::updated, &renderw,
                   &KRenderWidget::renderUserObjects);
  QObject::connect(&object_man, &KObjectManager::updated, &renderw,
                   &KRenderWidget::renderUserObjects);
  QObject::connect(&object_man, &KObjectManager::finishEdit,
                   &controls, &KControls::finishEdit);

  QGeoPositionInfoSource* geo =
      QGeoPositionInfoSource::createDefaultSource(&renderw);
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

  QMagnetometer    magnetometer(&renderw);
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

  renderw.show();
  renderw.setViewPoint(start_lat_lon, 1);

  /// TODO: Improve setting up
  QString aliceJid      = "knav.alice@macaw.me";
  QString alicePassword = "very-secure-password-for-knav-alice";
  QString bobJid        = "knav.bob@macaw.me";
  QString bobPassword   = "very-secure-password-for-knav-bob";
  QString log_path      = "client.log";
  // QString jidResource	 	= String("QXmpp");
  QString jidResource = "flowerpot";
  QString objects_dir = "./objects";
  QString proxy       = "proxy.macaw.me";

  KXmppClient client(objects_dir, proxy);
  client.logger()->setLogFilePath(log_path);
  client.logger()->setLoggingType(QXmppLogger::FileLogging);

  KLoginWidget* loginw =
      new KLoginWidget(screen_size_pix, bobJid, bobPassword);
  QObject::connect(loginw, &KLoginWidget::connectToServer, &client,
                   qOverload<const QString&, const QString&>(
                       &KXmppClient::reconnectToServer));
  QObject::connect(&controls, &KControls::login, loginw,
                   &QWidget::show);
  loginw->show();

  KRosterWidget roster_widget(
      client.findExtension<QXmppRosterManager>());
  roster_widget.setFixedSize(screen_size_pix);
  KPortableObjectSender sender;

  QObject::connect(&client, &QXmppClient::disconnected,
                   &roster_widget, &KRosterWidget::clear);
  QObject::connect(&newobjw, &KNewObjectWidget::sendObject,
                   &roster_widget, &KRosterWidget::show);
  QObject::connect(&newobjw, &KNewObjectWidget::sendObject, &sender,
                   &KPortableObjectSender::turnOnSendOnReady);
  QObject::connect(&newobjw, &KNewObjectWidget::doNotSendObject,
                   &sender,
                   &KPortableObjectSender::turnOffSendOnReady);
  QObject::connect(&client, &KXmppClient::fileDownloaded, &object_man,
                   qOverload<QString>(&KObjectManager::loadFile));
  QObject::connect(&sender, &KPortableObjectSender::send, &client,
                   &KXmppClient::sendFile);
  QObject::connect(&roster_widget, &KRosterWidget::jidSelected,
                   &sender, &KPortableObjectSender::setJid);
  QObject::connect(&object_man, &KObjectManager::saved, &sender,
                   &KPortableObjectSender::setFilename);

  return a.exec();
}
