#include "timebucket-model.h"
#include "dependencypointer.h"
#include <algorithm>
#include <src/utils/parse-iso-time.h>

using namespace Immich::Model;

TimebucketModel::TimebucketModel(QDate pDate, int pAssetCount, QObject *parent)
    : QAbstractListModel(parent), m_date(pDate), m_assetCount(pAssetCount) {}

TimebucketModel::TimebucketModel(const QDate &pDate,
                                 const QVector<TimebucketModelAsset *> &pAssets,
                                 QObject *parent)
    : QAbstractListModel(parent), m_assets(pAssets), m_date(pDate),
      m_assetCount(pAssets.size()), m_status(RevealStatusRevealed),
      m_thumbnailer(QInjection::Pointer<Immich::Thumbnailer>()) {}

int TimebucketModel::rowCount(const QModelIndex &) const {
  if (m_status != RevealStatusRevealed)
    return m_assetCount;
  return m_assets.size(); // TODO dynamic upate on asset vector changes
}

QVariant TimebucketModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  int rowIdx = index.row();
  if (rowIdx < 0 || rowIdx >= rowCount())
    return QVariant();

  const auto asset = m_assets[rowIdx];

  if (asset == nullptr) {
    return QVariant();
  }

  switch (role) {
  case AssetId:
    return asset->m_id;
  case ThumbnailSource: {
    if (asset->m_isThumbnailExists)
      return asset->m_thumbnailPath;

    const auto watcher = m_thumbnailer->getThumbnailSize(asset->m_id);

    if (!watcher->path().isEmpty()) {
      QMetaObject::invokeMethod(const_cast<TimebucketModel *>(this),
                                "updateThumbnail", Qt::QueuedConnection,
                                Q_ARG(QString, asset->m_id),
                                Q_ARG(QString, watcher->path()));

      watcher->deleteLater();
    } else {
      connect(watcher, &ThumbnailWatcher::finished, [this, asset, watcher]() {
        QMetaObject::invokeMethod(const_cast<TimebucketModel *>(this),
                                  "updateThumbnail", Qt::QueuedConnection,
                                  Q_ARG(QString, asset->m_id),
                                  Q_ARG(QString, watcher->path()));
        watcher->deleteLater();
      });
    }

    return "";
  }

  default:
    return QVariant();
  }
}

QDate TimebucketModel::date() const { return m_date; }

QHash<int, QByteArray> TimebucketModel::roleNames() const {
  return {{AssetId, "assetId"}, {ThumbnailSource, "thumbnailSource"}};
}

TimebucketModel::RevealStatus TimebucketModel::status() const {
  return m_status;
}

void TimebucketModel::setStatus(RevealStatus pStatus) {
  if (m_status == pStatus)
    return;
  m_status = pStatus;
  emit statusChanged();
}

void TimebucketModel::updateThumbnail(const QString &pAssetId,
                                      const QString &pPath) {
  for (int idx = 0; idx < m_assets.size(); ++idx) {
    const auto asset = m_assets[idx];
    if (asset == nullptr)
      continue;

    if (asset->m_id != pAssetId)
      continue;

    asset->m_isThumbnailExists = true;
    asset->m_thumbnailPath = pPath;
    QModelIndex qIdx = index(idx);
    emit dataChanged(qIdx, qIdx, {ThumbnailSource});
    return;
  }
}

TimebucketModelAsset::TimebucketModelAsset(
    const QString &pCity, const QString &pCountry, const QString &pDuration,
    const QString &pFile_created_at, const QString &pId,
    const bool &pIsFavorite, const bool &pIsImage, const bool &pIsTrashed,
    const double &pLatitude, const QString &pLivePhotoVideoId,
    const double &pLocalOffsetHours, const double &pLongitude,
    const QString &pOwnerId, const QString &pProjectionType,
    const double &pRatio, const QVector<QString> &pStack,
    const QString &pThumbhash,
    const OpenAPI::OAIAssetVisibility::eOAIAssetVisibility &pAssetVisibility,
    QObject *pParent)
    : QObject(pParent), m_city(pCity), m_country(pCountry),
      m_duration(pDuration), m_file_created_at(pFile_created_at), m_id(pId),
      m_thumbhash(pThumbhash), m_owner_id(pOwnerId),
      m_projection_type(pProjectionType),
      m_live_photo_video_id(pLivePhotoVideoId), m_is_favorite(pIsFavorite),
      m_is_image(pIsImage), m_is_trashed(pIsTrashed), m_latitude(pLatitude),
      m_local_offset_hours(pLocalOffsetHours), m_longitude(pLongitude),
      m_ratio(pRatio), m_visibility(pAssetVisibility), m_stack(pStack) {}

const QString &TimebucketModelAsset::city() const { return m_city; }

void TimebucketModelAsset::setCity(const QString &newCity) {
  if (m_city == newCity)
    return;
  m_city = newCity;
  emit cityChanged();
}

const QString &TimebucketModelAsset::country() const { return m_country; }

void TimebucketModelAsset::setCountry(const QString &newCountry) {
  if (m_country == newCountry)
    return;
  m_country = newCountry;
  emit countryChanged();
}

const QString &TimebucketModelAsset::duration() const { return m_duration; }

void TimebucketModelAsset::setDuration(const QString &newDuration) {
  if (m_duration == newDuration)
    return;
  m_duration = newDuration;
  emit durationChanged();
}

const QString &TimebucketModelAsset::file_created_at() const {
  return m_file_created_at;
}

void TimebucketModelAsset::setFile_created_at(
    const QString &newFile_created_at) {
  if (m_file_created_at == newFile_created_at)
    return;
  m_file_created_at = newFile_created_at;
  emit file_created_atChanged();
}

const QString &TimebucketModelAsset::id() const { return m_id; }

void TimebucketModelAsset::setId(const QString &newId) {
  if (m_id == newId)
    return;
  m_id = newId;
  emit idChanged();
}

const QString &TimebucketModelAsset::thumbhash() const { return m_thumbhash; }

void TimebucketModelAsset::setThumbhash(const QString &newThumbhash) {
  if (m_thumbhash == newThumbhash)
    return;
  m_thumbhash = newThumbhash;
  emit thumbhashChanged();
}

const QString &TimebucketModelAsset::owner_id() const { return m_owner_id; }

void TimebucketModelAsset::setOwner_id(const QString &newOwner_id) {
  if (m_owner_id == newOwner_id)
    return;
  m_owner_id = newOwner_id;
  emit owner_idChanged();
}

const QString &TimebucketModelAsset::projection_type() const {
  return m_projection_type;
}

void TimebucketModelAsset::setProjection_type(
    const QString &newProjection_type) {
  if (m_projection_type == newProjection_type)
    return;
  m_projection_type = newProjection_type;
  emit projection_typeChanged();
}

const QString &TimebucketModelAsset::live_photo_video_id() const {
  return m_live_photo_video_id;
}

void TimebucketModelAsset::setLive_photo_video_id(
    const QString &newLive_photo_video_id) {
  if (m_live_photo_video_id == newLive_photo_video_id)
    return;
  m_live_photo_video_id = newLive_photo_video_id;
  emit live_photo_video_idChanged();
}

const QString &TimebucketModelAsset::thumbnailPath() const {
  return m_thumbnailPath;
}

void TimebucketModelAsset::setThumbnailPath(const QString &newThumbnailPath) {
  if (m_thumbnailPath == newThumbnailPath)
    return;
  m_thumbnailPath = newThumbnailPath;
  emit thumbnailPathChanged();
}

const QString &TimebucketModelAsset::previewPath() const {
  return m_previewPath;
}

void TimebucketModelAsset::setPreviewPath(const QString &newPreviewPath) {
  if (m_previewPath == newPreviewPath)
    return;
  m_previewPath = newPreviewPath;
  emit previewPathChanged();
}

bool TimebucketModelAsset::is_favorite() const { return m_is_favorite; }

void TimebucketModelAsset::setIs_favorite(bool newIs_favorite) {
  if (m_is_favorite == newIs_favorite)
    return;
  m_is_favorite = newIs_favorite;
  emit is_favoriteChanged();
}

bool TimebucketModelAsset::is_image() const { return m_is_image; }

void TimebucketModelAsset::setIs_image(bool newIs_image) {
  if (m_is_image == newIs_image)
    return;
  m_is_image = newIs_image;
  emit is_imageChanged();
}

bool TimebucketModelAsset::is_trashed() const { return m_is_trashed; }

void TimebucketModelAsset::setIs_trashed(bool newIs_trashed) {
  if (m_is_trashed == newIs_trashed)
    return;
  m_is_trashed = newIs_trashed;
  emit is_trashedChanged();
}

bool TimebucketModelAsset::isThumbnailExists() const {
  return m_isThumbnailExists;
}

void TimebucketModelAsset::setIsThumbnailExists(bool newIsThumbnailExists) {
  if (m_isThumbnailExists == newIsThumbnailExists)
    return;
  m_isThumbnailExists = newIsThumbnailExists;
  emit isThumbnailExistsChanged();
}

bool TimebucketModelAsset::isPreviewExists() const { return m_isPreviewExists; }

void TimebucketModelAsset::setIsPreviewExists(bool newIsPreviewExists) {
  if (m_isPreviewExists == newIsPreviewExists)
    return;
  m_isPreviewExists = newIsPreviewExists;
  emit isPreviewExistsChanged();
}

double TimebucketModelAsset::latitude() const { return m_latitude; }

void TimebucketModelAsset::setLatitude(double newLatitude) {
  if (qFuzzyCompare(m_latitude, newLatitude))
    return;
  m_latitude = newLatitude;
  emit latitudeChanged();
}

double TimebucketModelAsset::local_offset_hours() const {
  return m_local_offset_hours;
}

void TimebucketModelAsset::setLocal_offset_hours(double newLocal_offset_hours) {
  if (qFuzzyCompare(m_local_offset_hours, newLocal_offset_hours))
    return;
  m_local_offset_hours = newLocal_offset_hours;
  emit local_offset_hoursChanged();
}

double TimebucketModelAsset::longitude() const { return m_longitude; }

void TimebucketModelAsset::setLongitude(double newLongitude) {
  if (qFuzzyCompare(m_longitude, newLongitude))
    return;
  m_longitude = newLongitude;
  emit longitudeChanged();
}

double TimebucketModelAsset::ratio() const { return m_ratio; }

void TimebucketModelAsset::setRatio(double newRatio) {
  if (qFuzzyCompare(m_ratio, newRatio))
    return;
  m_ratio = newRatio;
  emit ratioChanged();
}

OpenAPI::OAIAssetVisibility::eOAIAssetVisibility
TimebucketModelAsset::visibility() const {
  return m_visibility;
}

void TimebucketModelAsset::setVisibility(
    OpenAPI::OAIAssetVisibility::eOAIAssetVisibility newVisibility) {
  if (m_visibility == newVisibility)
    return;
  m_visibility = newVisibility;
  emit visibilityChanged();
}

const QVector<QString> &TimebucketModelAsset::stack() const { return m_stack; }

void TimebucketModelAsset::setStack(const QVector<QString> &newStack) {
  if (m_stack == newStack)
    return;
  m_stack = newStack;
  emit stackChanged();
}

// QJsonObject TimebucketModelAsset::asJsonObject()
//{
//     QJsonObject json;

//    json["city"] = m_city;
//    json["country"] = m_country;
//    json["duration"] = m_duration;
//    json["file_created_at"] = m_file_created_at;
//    json["id"] = m_id;
//    json["thumbhash"] = m_thumbhash;
//    json["owner_id"] = m_owner_id;
//    json["projection_type"] = m_projection_type;
//    json["live_photo_video_id"] = m_live_photo_video_id;
//    json["thumbnailPath"] = m_thumbnailPath;
//    json["previewPath"] = m_previewPath;

//    json["is_favorite"] = m_is_favorite;
//    json["is_image"] = m_is_image;
//    json["is_trashed"] = m_is_trashed;
//    json["isThumbnailExists"] = m_isThumbnailExists;
//    json["isPreviewExists"] = m_isPreviewExists;

//    json["latitude"] = m_latitude;
//    json["local_offset_hours"] = m_local_offset_hours;
//    json["longitude"] = m_longitude;
//    json["ratio"] = m_ratio;

//    QJsonArray stackArray;
//    for (const QString &item : m_stack) {
//        stackArray.append(item);
//    }
//    json["stack"] = stackArray;

//    json["visibility"] = static_cast<int>(m_visibility);

//    return json;
//}
