#include "ktrackmanager.h"
#include "krender.h"
#include <QDir>
#include <QDebug>

KTrackManager::KTrackManager(QString _objects_dir)
{
  QDir dir(_objects_dir);
  auto fi_list = dir.entryInfoList(QDir::Files, QDir::Name);
  for (auto fi: fi_list)
  {
    QFile f(fi.absoluteFilePath());
    if (f.open(QIODevice::ReadOnly))
    {
      auto   ba          = f.readAll();
      int    point_count = ba.size() / sizeof(KPosition);
      KTrack track;
      for (int i = 0; i < point_count; i++)
      {
        KPosition point;
        memcpy(&point, (char*)&ba.data()[i * sizeof(KPosition)],
               sizeof(KPosition));
        track.append(point);
      }
      tracks.insert(fi.fileName().remove(".ktrack"), track);
    }
  }
  objects_dir = _objects_dir;
}

void KTrackManager::onSwitchRecording()
{
  if (is_recording)
    is_recording = false;
  else
  {
    active_track.clear();
    QFile(getActiveTrackPath()).remove();
    is_recording = true;
  }
  switchRecording();
}

void KTrackManager::addPoint(KPosition p)
{
  if (!is_recording)
    return;

  active_track.append(p);
  QFile f(getActiveTrackPath());
  if (f.open(QIODevice::Append))
    f.write((char*)&p, sizeof(p));
  updated();
}

bool KTrackManager::isRecording()
{
  return is_recording;
}

void KTrackManager::saveTrack(QString new_track_name)
{
  QFile(getActiveTrackPath())
      .rename(objects_dir + "/" + new_track_name + ".ktrack");
  tracks.insert(new_track_name, active_track);
  active_track.clear();
}

void KTrackManager::paintTrack(QPainter* p, const QString& name,
                               const KTrack& track)
{
  QPoint prev_pix;
  QPoint pix;
  auto   f = p->font();
  f.setPixelSize(20);
  p->save();
  p->setFont(f);
  for (int i = -1; auto point: track)
  {
    i++;
    QPen pen = QPen(QColor(255 * i / track.count(), 0, 0), 5);
    p->setPen(pen);
    pix = kcoor2pix(point.coor);
    if (i > 0)
      p->drawLine(prev_pix, pix);
    prev_pix = pix;
    if (i == 0)
    {
      auto pos = pix + QPoint(10, 10);
      p->translate(pos);
      KRender::paintOutlinedText(p, name + " (start)", Qt::black);
      p->translate(-pos);
    }
  }
  auto pos = pix + QPoint(10, 10);
  p->translate(pos);
  KRender::paintOutlinedText(p, name + " (finish)", Qt::red);
  p->translate(-pos);
  p->restore();
}

void KTrackManager::paint(QPainter* p)
{
  QHashIterator<QString, KTrack> it(tracks);
  while (it.hasNext())
  {
    it.next();
    paintTrack(p, it.key(), it.value());
  }
  if (is_recording && active_track.count() >= 2)
    paintTrack(p, "active", active_track);
}

QString KTrackManager::getActiveTrackPath()
{
  return objects_dir + "/" + active_track_name + ".ktrack";
}
