#include "krefpack.h"

KRefPack::KRefPack(const QString& path): KPack(path)
{
}

const KTile& KRefPack::getMainTile() const
{
  return main;
}
