#include "api-ping.h"
using namespace Immich::Api::Ping;

PingApi::PingApi(QObject *parent) : Base(parent) {}

Result::Result<bool> PingApi::ping() const {
  Request req(apiUrl);

  const auto rep = nm->head(req);

  QEventLoop loop;
  QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  if (rep->error()) {
    const int statusCode =
        rep->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 404)
      return Result::Result<bool>(true);
    return Result::Result<bool>(Result::Error(rep->errorString()));
  }

  return Result::Result<bool>(true);
}
