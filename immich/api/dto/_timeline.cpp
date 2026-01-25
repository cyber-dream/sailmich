#include "_timeline.h"

using namespace Immich::Api::Timeline;

TimeBucketsResponse::TimeBucketsResponse(const QJsonObject &obj) : DtoStruct() {
  *this = fromJson<TimeBucketsResponse>(obj);
}

TimeBucketAsset::TimeBucketAsset(const QJsonObject &obj, const int idx)
    : DtoStruct() {
  *this = fromFlattenedArrObject<TimeBucketAsset>(obj, idx);
}
