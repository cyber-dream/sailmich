#include "api-albums.h"

#include <src/immich/api/dto/_albums.h>
#include <src/utils/uuid-str.h>

using namespace Immich::Api::Albums;

ApiAlbums::ApiAlbums(QObject *parent) : Base(parent) {}

Result::Result<QVector<AlbumResponseDto>>
ApiAlbums::getAlbums(bool isShared, const QUuid &assetId) const {
  QUrlQuery query;

  if (!assetId.isNull())
    query.addQueryItem("assetId", uuidStr(assetId));

//  query.addQueryItem("shared", boolStr(isShared));

  auto url = apiUrl.resolved(QUrl("albums"));
  url.setQuery(query.query());
  Request req(url);

  const auto rep = nm->get(req);

  QEventLoop loop;
  QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  if (rep->error()) {
    return {Result::Error(rep->errorString())};
  }

  QByteArray ReplyText = rep->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(ReplyText);
  if (!doc.isArray())
    return {Result::Error("json is not an array")};

  const auto arr = doc.array();
  QVector<AlbumResponseDto> albums;
  albums.reserve(arr.size());

  for (const auto &val : arr) {
    albums.append(AlbumResponseDto(val.toObject()));
  }

  return {albums};
}
