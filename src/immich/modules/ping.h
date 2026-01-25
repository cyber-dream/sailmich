#ifndef PING_H
#define PING_H

#include "_base.h"
#include <src/result/result.h>
#include <QObject>

namespace Immich {
namespace Module {
namespace Ping {
class Ping : public BaseModule {
  Q_OBJECT
public:
  explicit Ping(QObject *parent = nullptr);

   Result::Promise<bool> *ping() const;
signals:
};
} // namespace Ping
} // namespace Module
} // namespace Immich
#endif // PING_H
