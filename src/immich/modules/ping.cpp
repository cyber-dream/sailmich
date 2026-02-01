#include "ping.h"
#include <src/immich/api/api-ping.h>

using namespace Immich::Module::Ping;
Ping::Ping(QObject *parent) : BaseModule(parent) {}

Result::Promise<bool> *Ping::ping() const {
  return new Result::Promise<bool>([]() -> Result::Result<bool> {
    const auto res = api.ping->ping();

    if (!res.has_value()) {
      return tl::make_unexpected(res.error());
    }

    return {res.value()};
  });
}
