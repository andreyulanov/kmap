#include <math.h>
#include <QApplication>
#include <QStandardPaths>
#include <QScreen>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include "kmapwidget.h"
#include "kautoscroll.h"
#include "kcontrols.h"
#include "kfindobject.h"
#include "keditwidget.h"
#include "kmap.h"
#include "kposgenerator.h"
#include "ktrackmanager.h"
#include "knewobjectwidget.h"
#include "kxmppclient.h"
#include "krosterwidget.h"
#include <qxmpp/QXmppClient.h>
#include <qxmpp/QXmppLogger.h>
#include <qxmpp/QXmppRosterManager.h>
#include <kportableobjectsender.h>

#ifdef BUILD_WITH_SENSORS
  #include <QGeoPositionInfoSource>
  #include <QGeoCoordinate>
  #include "kpositionlabel.h"
  #include "kheading.h"
#endif

using namespace kmap;

struct LocalMapEntry
{
  KGeoRect     rect;
  KGeoPolygon* border;
};

class LocalMapDownloader
{
  QMap<QString, LocalMapEntry> iso_metrics_map;
  QStringList                  name_list;

public:
  LocalMapDownloader(QString file_list_path);
  void setMetricsForIsoCode(QString iso_code, LocalMapEntry entry);
  QStringList getIsoCodesForArea(KGeoRect);
};

LocalMapDownloader::LocalMapDownloader(QString file_list_path)
{
  QFile f(file_list_path);
  if (f.open(QIODevice::ReadOnly))
  {
    QTextStream in(&f);
    while (!in.atEnd())
      name_list.append(in.readLine());
  }
}

void LocalMapDownloader::setMetricsForIsoCode(QString       iso_code,
                                              LocalMapEntry entry)
{
  if (name_list.contains(iso_code))
    iso_metrics_map[iso_code] = entry;
}

void scan(KMapWidget* w, LocalMapDownloader* loader,
          const QString dir_path)
{
  qDebug() << "scanning" << dir_path;

  QDir dir(dir_path);
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::Hidden |
                QDir::NoSymLinks | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();

  auto world_map_path = dir_path + "/world.kmap";
  auto m              = w->appendMap(world_map_path, 0, 0, true);
  for (auto obj: m->global_tile)
  {
    auto iso_code =
        QString::fromUtf8(obj->attributes.value("iso_code"))
            .toLower();
    auto entry = LocalMapEntry{obj->frame, obj->polygons.first()};
    loader->setMetricsForIsoCode(iso_code, entry);
  }

  for (auto fi: list)
  {
    auto path = fi.absoluteFilePath();
    if (path == world_map_path)
      continue;
    if (path.endsWith(".kmap"))
    {
      qDebug() << "adding" << path;
      w->appendMap(path, 0, KMap::only_global_mip, false);
    }
  }
}

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  QSysInfo si;
  auto     is_device = si.productType().toLower().contains("android");

  auto   screen          = QGuiApplication::screens().first();
  QSize  screen_size_pix = screen->availableSize();
  QSizeF screen_size_mm  = screen->physicalSize();

  auto physical_diag_mm = sqrt(pow(screen_size_mm.width(), 2) +
                               pow(screen_size_mm.height(), 2));
  auto pixel_diag       = sqrt(pow(screen_size_pix.width(), 2) +
                               pow(screen_size_pix.height(), 2));
  KShape::pixel_size_mm = physical_diag_mm / pixel_diag;

  if (!is_device)
  {
    screen_size_pix = {720, 1000};
    screen_size_mm  = {
         screen_size_mm.width() * screen_size_pix.width() /
             screen_size_pix.width(),
         screen_size_mm.height() * screen_size_pix.height() /
             screen_size_pix.height()};
  }

  KMapWidget       mapw(screen_size_pix);
  KFindWidget      findw(screen_size_pix);
  KEditWidget      editw(screen_size_pix);
  KNewObjectWidget newobjw(screen_size_pix);

  editw.move((screen_size_pix.width() - editw.width()) / 2,
             (screen_size_pix.height() - editw.height()) / 2);

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
    mmc_path = ".";

  LocalMapDownloader loader(mmc_path + "/maplist.txt");
  scan(&mapw, &loader, mmc_path + "/maps");

  KShapeManager kvo_shape_man;

  QDir        dir(mmc_path + "/class");
  QStringList filters;
  filters << "*.json";
  dir.setNameFilters(filters);

  auto path_list = dir.entryList();
  for (auto path: path_list)
    kvo_shape_man.loadShapes(mmc_path + "/class/" + path,
                             mmc_path + "/class");

  KTrackManager          track_man(mmc_path + "/tracks");
  KPortableObjectManager object_man(mmc_path + "/objects");
  QObject::connect(&editw, &KEditWidget::saveTrack, &track_man,
                   &KTrackManager::saveTrack);

  KGeoCoor start_lat_lon = KGeoCoor::fromDegs(59.9769195, 30.3642851);

  KAutoScroll auto_scroll;

  QObject::connect(&findw, &KFindWidget::find, &mapw,
                   &KMapWidget::find);
  QObject::connect(&findw, &KFindWidget::showObject, &mapw,
                   &KMapWidget::showObject);
  QObject::connect(&findw, &KFindWidget::showCategory, &mapw,
                   &KMapWidget::showCategory);
  QObject::connect(&findw, &KFindWidget::getCategories, &mapw,
                   &KMapWidget::getCategories);

  QObject::connect(&mapw, &KMapWidget::mousePressed, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&mapw, &KMapWidget::mouseMoved, &auto_scroll,
                   &KAutoScroll::accumulate);
  QObject::connect(&mapw, &KMapWidget::mouseReleased, &auto_scroll,
                   &KAutoScroll::start);
  QObject::connect(&mapw, &KMapWidget::tapped, &object_man,
                   &KPortableObjectManager::addPoint);
  QObject::connect(&mapw, &KMapWidget::pinchStarted, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&mapw, &KMapWidget::getAutoScrollSpeed,
                   &auto_scroll, &KAutoScroll::getSpeed);
  QObject::connect(&auto_scroll, &KAutoScroll::scroll, &mapw,
                   &KMapWidget::scroll);

  double edge_mm        = 30;
  double step_mm        = 60;
  double button_size_mm = 30;
  if (is_device)
  {
    edge_mm        = 7;
    step_mm        = 20;
    button_size_mm = 7;
  }
  KControls controls(&mapw, &findw, edge_mm, step_mm, button_size_mm);
  QObject::connect(&controls, &KControls::zoomIn, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&controls, &KControls::zoomIn, &mapw,
                   &KMapWidget::zoomIn);
  QObject::connect(&controls, &KControls::zoomOut, &auto_scroll,
                   &KAutoScroll::stop);
  QObject::connect(&controls, &KControls::zoomOut, &mapw,
                   &KMapWidget::zoomOut);
  QObject::connect(&controls, &KControls::scrollTo, &mapw,
                   &KMapWidget::scrollTo);
  QObject::connect(&controls, &KControls::setRotation, &mapw,
                   &KMapWidget::setRotation);
  QObject::connect(&controls, &KControls::enableRotation, &mapw,
                   &KMapWidget::enableRotation);
  QObject::connect(&controls, &KControls::disableRotation, &mapw,
                   &KMapWidget::disableRotation);
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

  QObject::connect(&newobjw, &KNewObjectWidget::getUserShapeList,
                   &kvo_shape_man, &KShapeManager::getShapeList);
  QObject::connect(&newobjw, &KNewObjectWidget::selectedShape,
                   &object_man,
                   &KPortableObjectManager::createObject);
  QObject::connect(&newobjw, &KNewObjectWidget::getShapeById,
                   &kvo_shape_man, &KShapeManager::getShapeById);

  QObject::connect(&mapw, &KMapWidget::zoomFinished, &controls,
                   &KControls::checkZoomRepeat);
  QObject::connect(&mapw, &KMapWidget::mouseMoved, &controls,
                   &KControls::onMouseMoved);
  QObject::connect(&mapw, &KMapWidget::movedCenterTo, &controls,
                   &KControls::setCurrCoor);

#ifdef BUILD_WITH_SENSORS
  KPositionLabel   position_label(&mapw);
  QGeoPositionInfo info;
  info.setCoordinate({59.9769195, 30.3642851});
  position_label.updateGeoPosition(info);
  QObject::connect(&mapw, &KMapWidget::modified, &position_label,
                   &KPositionLabel::update);
  QObject::connect(&mapw, &KMapWidget::paintObjects, &track_man,
                   &KTrackManager::paint, Qt::DirectConnection);
  QObject::connect(&mapw, &KMapWidget::paintObjects, &object_man,
                   &KPortableObjectManager::paint,
                   Qt::DirectConnection);
  QObject::connect(&track_man, &KTrackManager::kcoor2pix, &mapw,
                   &KMapWidget::kcoor2pix, Qt::DirectConnection);
  QObject::connect(&object_man, &KPortableObjectManager::kcoor2pix,
                   &mapw, &KMapWidget::kcoor2pix,
                   Qt::DirectConnection);
  QObject::connect(&position_label, &KPositionLabel::deg2pix, &mapw,
                   &KMapWidget::deg2pix, Qt::DirectConnection);

  QObject::connect(&position_label,
                   &KPositionLabel::hasNorthOrientation, &controls,
                   &KControls::hasNorthOrientation,
                   Qt::DirectConnection);

  QObject::connect(&track_man, &KTrackManager::updated, &mapw,
                   &KMapWidget::render);
  QObject::connect(&object_man, qOverload<>(&KPortableObjectManager::updated),
                   &mapw, &KMapWidget::render);
  QObject::connect(&object_man, qOverload<>(&KPortableObjectManager::finishEdit),
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
  QObject::connect(&heading_provider,
                   &KHeadingProvider::headingChanged, &controls,
                   &KControls::updateHeading);
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

  ///TODO: Improve setting up
  QString aliceJid 			= QString("knav.alice@macaw.me");
  QString alicePassword 	= "very-secure-password-for-knav-alice";
  QString aliceLog 			= "alice.log";
  QString bobJid 			= QString("knav.bob@macaw.me");
  QString bobPassword	 	= "very-secure-password-for-knav-bob";
  QString bobLog 			= "bob.log";
  //QString jidResource	 	= QString("QXmpp");
  QString jidResource	 	= QString("flowerpot");
  QString objects_dir	 	= QString("./objects");
  QString proxy 			= QString("proxy.macaw.me");
  QString fileToSendPath	= objects_dir + QString("/file-to-send.kpo");

  //KXmppClient alice(objects_dir, proxy);
  //alice.logger()->setLogFilePath(aliceLog);
  //alice.logger()->setLoggingType(QXmppLogger::FileLogging);
  //alice.connectToServer(aliceJid, alicePassword);

  KXmppClient bob(objects_dir, proxy);
  bob.logger()->setLogFilePath(bobLog);
  bob.logger()->setLoggingType(QXmppLogger::FileLogging);
  bob.connectToServer(bobJid, bobPassword);
  //bob.sendFile(aliceJid + "/" + jidResource, fileToSendPath,
  //             "Alice, let's test if the file transfer works");


  KRosterWidget roster_widget(bob.findExtension<QXmppRosterManager>());
  roster_widget.setFixedSize(screen_size_pix);
  QObject::connect(&roster_widget, &KRosterWidget::jidSelected, [](QList<QString> jids)
  {
      qDebug() << "Selected jids:" << jids;
  });
  KPortableObjectSender sender;

  QObject::connect(&controls, &KControls::sendOnSave,
                   &roster_widget, &KRosterWidget::show);
  QObject::connect(&controls, &KControls::sendOnSave,
                   &sender, &KPortableObjectSender::turnOnSendOnReady);
  QObject::connect(&sender, &KPortableObjectSender::send,
                   &bob, &KXmppClient::sendFile);
  QObject::connect(&roster_widget, &KRosterWidget::jidSelected,
                   &sender, &KPortableObjectSender::setJid);
  QObject::connect(&object_man, &KPortableObjectManager::saved,
                   &sender, &KPortableObjectSender::setFilename);

  return a.exec();
}
