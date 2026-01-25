#include "api-assets.h"
#include <src/utils/uuid-str.h>

using namespace Immich::Api::Assets;

ApiAssets::ApiAssets(QObject *parent) : Base(parent) {}

Result::Result<QByteArray>
ApiAssets::thumbnail(const QUuid &id,
                     const ThumbnailQueryParams &params) const {
  QUrlQuery query;

  if (params.key.has_value())
    query.addQueryItem("key", params.key.value());
  if (params.slug.has_value())
    query.addQueryItem("slug", params.slug.value());

  query.addQueryItem("size",
                       Models::AssetMediaSizeToString(params.size));

  auto url = apiUrl.resolved(QUrl("assets/" + uuidStr(id) + "/thumbnail"));
  url.setQuery(query.query());
  Request req(url);

  const auto rep = nm->get(req);

  QEventLoop loop;
  QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  if (rep->error()) {
    return Result::Result<QByteArray>(Result::Error(rep->errorString()));
  }

  return Result::Result<QByteArray>(rep->readAll());
}
