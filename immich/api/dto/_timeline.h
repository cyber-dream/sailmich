#ifndef _TIMELINE_H
#define _TIMELINE_H

#include "_dto.h"
#include <QDate>
#include <QJsonArray>
#include <QObject>
#include <QUuid>
#include <QVariant>
#include <QVector>
#include <tl-optional.h>

namespace Immich {
namespace Api {
namespace Timeline {

struct TimeBucketAsset : public DtoStruct {
  Q_GADGET
  Q_PROPERTY(QUuid id MEMBER id)
  Q_PROPERTY(QUuid ownerId MEMBER ownerId)

  Q_PROPERTY(QDateTime fileCreatedAt MEMBER fileCreatedAt)

  //  Q_PROPERTY(tl::optional<QString> city MEMBER city)
  //  Q_PROPERTY(tl::optional<QString> country MEMBER country)
  //  Q_PROPERTY(QVariant livePhotoVideoId READ getLivePhotoVideoId)
  //  Q_PROPERTY(QVariant projectionType READ getProjectionType)
  //  Q_PROPERTY(QVariant stack READ getStack)
  //  Q_PROPERTY(QVariant thumbhash READ getThumbhash)

//  Q_PROPERTY(quint64 duration MEMBER duration)

  Q_PROPERTY(bool isFavorite MEMBER isFavorite)
  Q_PROPERTY(bool isImage MEMBER isImage)
  Q_PROPERTY(bool isTrashed MEMBER isTrashed)

//  Q_PROPERTY(float ratio MEMBER ratio)
//  Q_PROPERTY(float localOffsetHours MEMBER localOffsetHours)

  //  Q_PROPERTY(QVariant longitude READ getLongitude)
  //  Q_PROPERTY(QVariant latitude READ getLatitude)

//  Q_PROPERTY(Models::AssetVisibility visibility MEMBER visibility)
//  Q_CLASSINFO("duration:nullIsZeroValue", "true")
public:
  TimeBucketAsset() = default;
  TimeBucketAsset(const QJsonObject &obj, int idx);

  Q_INVOKABLE QVariant getCity() const {
    return city.has_value() ? QVariant::fromValue(city.value()) : QVariant();
  };
  Q_INVOKABLE QVariant getCountry() const {
    return country.has_value() ? QVariant::fromValue(country.value())
                               : QVariant();
  };
  Q_INVOKABLE QVariant getLivePhotoVideoId() const {
    return livePhotoVideoId.has_value()
               ? QVariant::fromValue(livePhotoVideoId.value())
               : QVariant();
  };
  Q_INVOKABLE QVariant getProjectionType() const {
    return projectionType.has_value()
               ? QVariant::fromValue(projectionType.value())
               : QVariant();
  };
  Q_INVOKABLE QVariant getStack() const {
    return stack.has_value() ? QVariant::fromValue(stack.value()) : QVariant();
  };
  Q_INVOKABLE QVariant getThumbhash() const {
    return thumbhash.has_value() ? QVariant::fromValue(thumbhash.value())
                                 : QVariant();
  };
  Q_INVOKABLE QVariant getLongitude() const {
    return longitude.has_value() ? QVariant::fromValue(longitude.value())
                                 : QVariant();
  };
  Q_INVOKABLE QVariant getLatitude() const {
    return latitude.has_value() ? QVariant::fromValue(latitude.value())
                                : QVariant();
  };

  QUuid id;
  QUuid ownerId;

  QDateTime fileCreatedAt;

  tl::optional<QString> city;
  tl::optional<QString> country;
  tl::optional<QString> livePhotoVideoId;
  tl::optional<QString> projectionType;
  tl::optional<QString> thumbhash;
  tl::optional<QString> stack;

  quint64 duration;

  bool isFavorite;
  bool isImage;
  bool isTrashed;

  float ratio;
  float localOffsetHours;

  tl::optional<float> latitude;
  tl::optional<float> longitude;

  Models::AssetVisibility visibility;
};

struct TimeBucketsResponse : public DtoStruct {
  Q_GADGET

  Q_PROPERTY(QDate timeBucketDate MEMBER timeBucketDate)
  Q_PROPERTY(int count MEMBER count)
  Q_CLASSINFO("timeBucketDate:json", "timeBucket")
public:
  TimeBucketsResponse() = default;
  explicit TimeBucketsResponse(const QJsonObject &obj);
  QDate timeBucketDate;
  int count;
};

} // namespace Timeline
} // namespace Api
} // namespace Immich
#endif // _TIMELINE_H
