#include "api-timeline.h"
#include "dto/json-parser.h"
#include <src/utils/bool-str.h>
#include <src/utils/uuid-str.h>

using namespace Immich::Api::Timeline;

ApiTimeline::ApiTimeline(QObject *parent) : Base(parent) {}

Result::Result<QVector<TimeBucketAsset>>
ApiTimeline::getBucket(const QDate &pDate,
                       const GetBucketQueryParam &params) const {
  if (!pDate.isValid())
    return {Result::Error("date parameter is invalid")};
  QUrlQuery query;
  query.addQueryItem("timeBucket", pDate.toString("yyyy-MM-dd"));
  if (params.albumId.has_value())
    query.addQueryItem("albumId", uuidStr(params.albumId.value()));

  if (params.isFavorite.has_value())
    query.addQueryItem("isFavorite", boolStr(params.isFavorite.value()));

  if (params.isTrashed.has_value())
    query.addQueryItem("isTrashed", boolStr(params.isTrashed.value()));

  if (params.key.has_value())
    query.addQueryItem("key", params.key.value());

  if (params.order.has_value())
    query.addQueryItem("order",
                       Models::AssetOrderToString(params.order.value()));

  if (params.personId.has_value())
    query.addQueryItem("personId", params.personId.value().toString());

  if (params.slug.has_value())
    query.addQueryItem("slug", params.slug.value());

  if (params.tagId.has_value())
    query.addQueryItem("tagId", params.tagId.value().toString());

  if (params.userId.has_value())
    query.addQueryItem("userId", params.userId.value().toString());

  if (params.visibility.has_value())
    query.addQueryItem("visibility", Models::AssetVisibilityToString(
                                         params.visibility.value()));

  if (params.withCoordinates.has_value())
    query.addQueryItem("withCoordinates",
                       boolStr(params.withCoordinates.value()));

  if (params.withPartners.has_value())
    query.addQueryItem("withPartners", boolStr(params.withPartners.value()));

  if (params.withStacked.has_value())
    query.addQueryItem("withStacked", boolStr(params.withStacked.value()));

  auto url = apiUrl.resolved(QUrl("timeline/bucket"));
  url.setQuery(query.query());
  Request req(url);

  const auto rep = nm->get(req);

  QEventLoop loop;
  QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  if (rep->error())
    return {Result::Error(rep->errorString())};

  QByteArray ReplyText = rep->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(ReplyText);
  if (!doc.isObject())
    return {Result::Error("json is not an object")};

  const auto obj = doc.object();

  if (!obj["id"].isArray())
    return {Result::Error("malformed json object")};

  const int bucketSize = obj["id"].toArray().size();

  QVector<TimeBucketAsset> assets(bucketSize);

  for (int idx = 0; idx < bucketSize; ++idx) {
    assets[idx] = TimeBucketAsset(obj, idx);
    if (!assets[idx].isValid())
      return {Result::Error("can't parse timebucket [" +
                          QVariant(idx).toString() +
                          "]: " + assets[idx].errMessage())};
  }

  return Result::Result<QVector<TimeBucketAsset>>(assets);
}

Result::Result<QVector<TimeBucketsResponse>>
ApiTimeline::getBuckets(const GetBucketsQueryParam &params) const {
  QUrlQuery query;
  if (params.albumId.has_value()) {
    query.addQueryItem("albumId", uuidStr(params.albumId.value()));
  };
  if (params.isFavorite.has_value()) {
    query.addQueryItem("isFavorite", boolStr(params.isFavorite.value()));
  };
  if (params.isTrashed.has_value()) {
    query.addQueryItem("isTrashed", boolStr(params.isTrashed.value()));
  };
  if (params.key.has_value()) {
    query.addQueryItem("key", params.key.value());
  };
  if (params.order.has_value()) {
    query.addQueryItem("order",
                       Models::AssetOrderToString(params.order.value()));
  };
  if (params.personId.has_value()) {
    query.addQueryItem("personId", params.personId.value().toString());
  };
  if (params.slug.has_value()) {
    query.addQueryItem("slug", params.slug.value());
  };
  if (params.tagId.has_value()) {
    query.addQueryItem("tagId", params.tagId.value().toString());
  };
  if (params.userId.has_value()) {
    query.addQueryItem("userId", params.userId.value().toString());
  };
  if (params.visibility.has_value()) {
    query.addQueryItem("visibility", Models::AssetVisibilityToString(
                                         params.visibility.value()));
  };
  if (params.withCoordinates.has_value()) {
    query.addQueryItem("withCoordinates",
                       boolStr(params.withCoordinates.value()));
  };
  if (params.withPartners.has_value()) {
    query.addQueryItem("withPartners", boolStr(params.withPartners.value()));
  };
  if (params.withStacked.has_value()) {
    query.addQueryItem("withStacked", boolStr(params.withStacked.value()));
  };

  auto url = apiUrl.resolved(QUrl("timeline/buckets"));
  url.setQuery(query.query());

  Request req(url);

  const auto rep = nm->get(req);

  QEventLoop loop;
  QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  if (rep->error())
    return {Result::Error(rep->errorString())};

  QByteArray ReplyText = rep->readAll();

  QJsonDocument doc = QJsonDocument::fromJson(ReplyText);
  if (!doc.isArray())
    return {Result::Error("json is not an array")};

  QJsonArray array = doc.array();
  QVector<TimeBucketsResponse> resp(array.size());

  for (int idx = 0; idx < array.size(); ++idx) {
    if (!array[idx].isObject())
      return {Result::Error("can't parse timelineBuckets, [" +
                          QVariant(idx).toString() + "] is not an jsonObject")};

    resp[idx] = TimeBucketsResponse(array[idx].toObject());

    if (!resp[idx].isValid())
      return {Result::Error("can't parse timelineBuckets, [" +
                          QVariant(idx).toString() +
                          "] is not a valid: " + resp[idx].errMessage())};
  }

  return {resp};
}
