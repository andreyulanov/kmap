#ifndef KREFPACK_H
#define KREFPACK_H

#include "kpack.h"

class KRefPack: public KPack
{
public:
  KRefPack(const QString& path);
  const KTile& getMainTile() const;
};

#endif  // KREFPACK_H
