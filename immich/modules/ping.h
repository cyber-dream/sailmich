#ifndef PING_H
#define PING_H

#include "_base-module.h"
#include "immich/result/result.h"
#include <QObject>

namespace Immich {
namespace Module {
namespace Ping {
class Ping : public BaseModule {
  Q_OBJECT
public:
  explicit Ping(QObject *parent = nullptr);

  ResultPromise<bool> *ping() const;
signals:
};
} // namespace Ping
} // namespace Module
} // namespace Immich
#endif // PING_H
