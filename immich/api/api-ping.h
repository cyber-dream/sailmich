#ifndef API_PING_H
#define API_PING_H

#include "_base-api.h"
#include "immich/result/result.h"
#include <QObject>

namespace Immich {
namespace Api {
namespace Ping {

class PingApi : public Base {
  Q_OBJECT
public:
  explicit PingApi(QObject *parent = nullptr);

  ResultErrored<bool> ping() const;
signals:
};
} // namespace Ping
} // namespace Api
} // namespace Immich
#endif // API_PING_H
