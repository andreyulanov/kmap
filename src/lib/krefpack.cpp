#include "krefpack.h"

KRefPack::KRefPack(const QString& path): KPack(path)
{
}

const KTile& KRefPack::getMainTile() const
{
  return main;
}

const QVector<KTile*>& KRefPack::getTiles() const
{
  return tiles;
}
