#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include "kpack.h"

int main(int argc, char* argv[])
{
  QCoreApplication a(argc, argv);

  QDir dir(argv[1]);
  dir.setFilter(QDir::Files | QDir::NoSymLinks |
                QDir::NoDotAndDotDot);
  auto fi_list = dir.entryInfoList();

  auto result_path = argv[3];

  QFile().remove(result_path);

  QString first_map_path = QString(argv[1]) + "/" + argv[2];
  KPack*  united_map     = new KPack(first_map_path);
  united_map->loadAll();

  for (auto& fi: fi_list)
  {
    if (fi.suffix() != "kpack")
      continue;

    if (fi.absoluteFilePath() == first_map_path)
      continue;

    qDebug() << "loading" << fi.absoluteFilePath();
    KPack* map = new KPack(fi.absoluteFilePath());
    map->loadAll();
    united_map->add(map);
  }
  qDebug() << "saving united map...";
  if (united_map)
    united_map->save(result_path);
}
