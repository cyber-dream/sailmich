#include "ping.h"
#include <immich/api/api-ping.h>

using namespace Immich::Module::Ping;
Ping::Ping(QObject *parent) : BaseModule(parent) {}

Immich::ResultPromise<bool> *Ping::ping() const {
  return new ResultPromise<bool>([]() { return api.ping->ping(); });
}
