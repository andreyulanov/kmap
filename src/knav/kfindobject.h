#ifndef KFINDOBJECT_H
#define KFINDOBJECT_H

#include <QThread>

class KFindObject: public QThread
{
public:
  KFindObject();
  void start(const QString&);
};

#endif  // KFINDOBJECT_H
