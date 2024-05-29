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

  QString first_pack_path = QString(argv[1]) + "/" + argv[2];
  KPack*  united_pack     = new KPack(first_pack_path);
  united_pack->loadAll(0);

  for (auto& fi: fi_list)
  {
    if (fi.suffix() != "kpack")
      continue;

    if (fi.absoluteFilePath() == first_pack_path)
      continue;

    qDebug() << "loading" << fi.absoluteFilePath();
    KPack pack(fi.absoluteFilePath());
    pack.loadAll(0);
    united_pack->add(pack);
  }
  qDebug() << "saving united pack...";
  if (united_pack)
    united_pack->save(result_path);
}
