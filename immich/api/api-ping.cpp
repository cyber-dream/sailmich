#include "api-ping.h"
using namespace Immich::Api::Ping;

PingApi::PingApi(QObject *parent) : Base(parent) {}

Immich::ResultErrored<bool> PingApi::ping() const {
  Request req(apiUrl);

  const auto rep = nm->head(req);

  QEventLoop loop;
  QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  if (rep->error()) {
    const int statusCode =
        rep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 404)
      return Immich::ResultErrored<bool>(true);
    return Immich::ResultErrored<bool>(ResultError(rep->errorString()));
  }

  return Immich::ResultErrored<bool>(true);
}
