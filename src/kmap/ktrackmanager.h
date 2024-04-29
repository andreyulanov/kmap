#ifndef KTRACKMANAGER_H
#define KTRACKMANAGER_H

#include "kpack.h"

typedef QVector<KPosition> KTrack;

class KTrackManager: public QObject
{
  Q_OBJECT

  bool                   is_recording = false;
  QString                objects_dir;
  QString                active_track_name = "active";
  KTrack                 active_track;
  QHash<QString, KTrack> tracks;
  QString                getActiveTrackPath();
  void                   paintTrack(QPainter*, const QString& name,
                                    const KTrack& track);

signals:
  QPoint deg2pix(KGeoCoor);
  void   updated();
  void   switchRecording();

public:
  KTrackManager(QString objects_dir);
  void addPoint(KPosition);
  void onSwitchRecording();
  bool isRecording();
  void paint(QPainter*);
  void saveTrack(QString);
};

#endif  // KTRACKMANAGER_H
