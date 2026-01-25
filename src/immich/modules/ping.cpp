#include "ping.h"
#include <src/immich/api/api-ping.h>

using namespace Immich::Module::Ping;
Ping::Ping(QObject *parent) : BaseModule(parent) {}

 Result::Promise<bool> *Ping::ping() const {
  return new  Result::Promise<bool>([]() { return api.ping->ping(); });
}
