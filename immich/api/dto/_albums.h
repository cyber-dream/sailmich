#ifndef _ALBUMS_H
#define _ALBUMS_H
#include "_dto.h"
#include <QObject>
#include <QVector>
#include <tl-optional.h>

namespace Immich {
namespace Api {
namespace Albums {

struct AlbumResponseDto : public DtoStruct {
  Q_GADGET
  Q_PROPERTY(QString albumName MEMBER albumName)
  Q_PROPERTY(QString description MEMBER description)
  Q_PROPERTY(QUuid id MEMBER id)
  Q_PROPERTY(QUuid ownerId MEMBER ownerId)
  //  Q_PROPERTY(
  //      tl::optional<QUuid> albumThumbnailAssetId MEMBER
  //      albumThumbnailAssetId)
  Q_PROPERTY(QUuid albumThumbnailAssetId MEMBER
                 albumThumbnailAssetId) // TODO use optional
  Q_PROPERTY(quint64 assetCount MEMBER assetCount)
  //  Q_PROPERTY(QVector<AlbumUserResponseDto> albumUsers MEMBER albumUsers)
  //  Q_PROPERTY(QVector<AssetResponseDto> assets MEMBER assets)
  //  Q_PROPERTY(QVector<ContributorCountResponseDto> contributorCounts MEMBER
  //                 contributorCounts)
  Q_PROPERTY(QDateTime createdAt MEMBER createdAt)
  Q_PROPERTY(QDateTime startDate MEMBER startDate)
  Q_PROPERTY(QDateTime updatedAt MEMBER updatedAt)
  //  Q_PROPERTY(tl::optional<QDateTime> endDate MEMBER endDate)
  //  Q_PROPERTY(tl::optional<QDateTime> lastModifiedAssetTimestamp MEMBER
  //                 lastModifiedAssetTimestamp)
  //  Q_PROPERTY(tl::optional<Models::AssetOrder> order MEMBER order)
  Q_PROPERTY(bool hasSharedLink MEMBER hasSharedLink)
  Q_PROPERTY(bool isActivityEnabled MEMBER isActivityEnabled)
  Q_PROPERTY(bool shared MEMBER shared)
  //  Q_PROPERTY(UserResponseDto owner MEMBER owner)
public:
  AlbumResponseDto() = default;
  explicit AlbumResponseDto(const QJsonObject &obj);

  QString albumName;
  QString description;

  QUuid id;
  QUuid ownerId;

  //  tl::optional<QUuid> albumThumbnailAssetId; //TODO use optional
  QUuid albumThumbnailAssetId; // TODO temp

  quint64 assetCount;

  QVector<AlbumUserResponseDto> albumUsers;

  QVector<AssetResponseDto> assets;

  QVector<ContributorCountResponseDto> contributorCounts;

  QDateTime createdAt;
  QDateTime startDate;
  QDateTime updatedAt;

  tl::optional<QDateTime> endDate;
  tl::optional<QDateTime> lastModifiedAssetTimestamp;

  tl::optional<Models::AssetOrder> order;

  bool hasSharedLink;
  bool isActivityEnabled;
  bool shared;

  UserResponseDto owner;
};
}; // namespace Albums
} // namespace Api
} // namespace Immich
//Q_DECLARE_METATYPE(Immich::Api::Albums::AlbumResponseDto)
#endif // _ALBUMS_H
