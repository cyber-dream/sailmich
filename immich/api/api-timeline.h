#ifndef APITIMELINE_H
#define APITIMELINE_H

#include "_base-api.h"
#include "dto/_timeline.h"
#include <QObject>
#include <QUuid>
#include <immich/result/result.h>
#include <tl-optional.h>

namespace Immich {
namespace Api {
namespace Timeline {

class ApiTimeline : public Base {
  Q_OBJECT
public:
  explicit ApiTimeline(QObject *parent = nullptr);

  struct GetBucketQueryParam {
    // Filter assets belonging to a specific album
    tl::optional<QUuid> albumId;

    // Filter by favorite status (true for favorites only, false for
    // non-favorites only)
    tl::optional<bool> isFavorite;

    // Filter by trash status (true for trashed assets only, false for
    // non-trashed only)
    tl::optional<bool> isTrashed;

    tl::optional<QString> key;

    // Sort order for assets within time buckets (ASC for oldest first, DESC for
    // newest first)
    tl::optional<Models::AssetOrder> order;

    // Filter assets containing a specific person (face recognition)
    tl::optional<QUuid> personId;

    tl::optional<QString> slug;

    // Filter assets with a specific tag
    tl::optional<QUuid> tagId;

    // Filter assets by specific user ID
    tl::optional<QUuid> userId;

    // Filter by asset visibility status (ARCHIVE, TIMELINE, HIDDEN, LOCKED)
    tl::optional<Models::AssetVisibility> visibility;

    // Include location data in the response
    tl::optional<bool> withCoordinates;

    // Include assets shared by partners
    tl::optional<bool> withPartners;

    // Include stacked assets in the response. When true, only primary assets
    // from stacks are returned.
    tl::optional<bool> withStacked;
  };

  Immich::ResultErrored<QVector<TimeBucketAsset>> getBucket(const QDate& pTimeBucket, const GetBucketQueryParam& params = {}) const;

  struct GetBucketsQueryParam {
    // Filter assets belonging to a specific album
    tl::optional<QUuid> albumId;

    // Filter by favorite status (true for favorites only, false for
    // non-favorites only)
    tl::optional<bool> isFavorite;

    // Filter by trash status (true for trashed assets only, false for
    // non-trashed only)
    tl::optional<bool> isTrashed;

    tl::optional<QString> key;

    // Sort order for assets within time buckets (ASC for oldest first, DESC for
    // newest first)
    tl::optional<Models::AssetOrder> order;

    // Filter assets containing a specific person (face recognition)
    tl::optional<QUuid> personId;

    tl::optional<QString> slug;
    // Filter assets with a specific tag
    tl::optional<QUuid> tagId;

    // Filter assets by specific user ID
    tl::optional<QUuid> userId;

    // Filter by asset visibility status (ARCHIVE, TIMELINE,
    // HIDDEN, LOCKED)
    tl::optional<Models::AssetVisibility> visibility;

    // Include location data in the response
    tl::optional<bool> withCoordinates;

    // Include assets shared by partners
    tl::optional<bool> withPartners;

    // Include stacked assets in the
    // response. When true, only primary
    // assets from stacks are returned.
    tl::optional<bool> withStacked;
  };

  Immich::ResultErrored<QVector<TimeBucketsResponse>>
  getBuckets(const GetBucketsQueryParam &params = {}) const;

signals:
};
} // namespace Timeline
} // namespace Api
} // namespace Immich

#endif // APITIMELINE_H
