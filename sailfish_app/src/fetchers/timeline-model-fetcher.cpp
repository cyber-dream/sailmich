#include "timeline-model-fetcher.h"
#include <OAITimelineApi.h>
#include <QLocale>
#include <QtConcurrent>
#include <dependencypointer.h>
#include <src/api-manager.h>
#include <src/utils/parse-iso-time.h>

using namespace Immich::Fetcher;

TimelineModelFetcherParams::TimelineModelFetcherParams(QObject *parent)
    : QObject(parent), album_id(), is_favorite(), is_trashed(), key(), order(),
      person_id(), slug(), tag_id(), user_id(), visibility(),
      with_coordinates(), with_partners(), with_stacked() {}

//TODO bug, not works if model created in constructor, but works if model setted in qml manually

TimelineModelFetcher::TimelineModelFetcher(QObject *parent)
    : QObject(parent), m_model(new Model::TimelineModel(this)),
      m_apiManager(QInjection::Pointer<ApiManager>()),
      m_params(new TimelineModelFetcherParams(this)) {}

TimelineModelFetcher::~TimelineModelFetcher() {
  if (m_model == nullptr)
    return;

  disconnect(m_model, nullptr, this, nullptr);
}

Immich::Model::TimelineModel *TimelineModelFetcher::model() const {
  return m_model;
}

void TimelineModelFetcher::setModel(Model::TimelineModel *newModel) {
  if (m_model == newModel)
    return;

  if (m_model != nullptr) {
    m_model->disconnect(this);
  }

  m_model = newModel;

  emit modelChanged();

  if (m_model == nullptr)
    return;

  connect(m_model, &Model::TimelineModel::needReveal, this,
          &TimelineModelFetcher::slotOnTimelineNeedReveal);
}

void TimelineModelFetcher::fetch() {
  const auto timelineApi = m_apiManager->timeline();
  QPointer<TimelineModelFetcher> self = this;

  connect(
      timelineApi, &OpenAPI::OAITimelineApi::getTimeBucketsSignal, this,
      [self,
       timelineApi](QList<OpenAPI::OAITimeBucketsResponseDto> timeBucketsDto) {
        QVector<Model::TimebucketModel *> pBuckets;
        for (const auto &b : timeBucketsDto) {
          const auto date = QDate::fromString(b.getTimeBucket(), "yyyy-MM-dd");

          if (!date.isValid()) {
            qWarning() << "timeBucket date is invalid";
            continue;
          }

          const auto model = new Model::TimebucketModel(date, b.getCount());
          auto modelPtr = self->m_model;

          if (!modelPtr) {
            model->deleteLater();
            qDeleteAll(pBuckets);
            QMetaObject::invokeMethod(timelineApi, "deleteLater");
            return;
          }

          model->moveToThread(modelPtr->thread());
          pBuckets.append(model);
        }

        if (pBuckets.isEmpty()) {
          QMetaObject::invokeMethod(self->m_model, "slotEmptyInitialize",
                                    Qt::QueuedConnection);
        } else {
          if (!QMetaObject::invokeMethod(
                  self->m_model, "slotAppendDataBatch", Qt::QueuedConnection,
                  Q_ARG(QVector<Immich::Model::TimebucketModel *>, pBuckets))) {
            qDeleteAll(pBuckets);
          }
        }

        QMetaObject::invokeMethod(timelineApi, "deleteLater");
      });

  connect(timelineApi, &OpenAPI::OAITimelineApi::getTimeBucketsSignalError,
          this,
          [timelineApi](QList<OpenAPI::OAITimeBucketsResponseDto>,
                        QNetworkReply::NetworkError, const QString &error_str) {
            qWarning() << "can't retrive timeBuckets" << error_str;
            QMetaObject::invokeMethod(timelineApi, "deleteLater");
          });

  auto visibility = OpenAPI::OAIAssetVisibility();
  visibility.setValue(
      static_cast<OpenAPI::OAIAssetVisibility::eOAIAssetVisibility>(
          m_params->visibility));

  timelineApi->getTimeBuckets(
      m_params->album_id, m_params->is_favorite, m_params->is_trashed,
      m_params->key, m_params->order, m_params->person_id, m_params->slug,
      m_params->tag_id, m_params->user_id, visibility,
      m_params->with_coordinates, m_params->with_partners,
      m_params->with_stacked);
}

void TimelineModelFetcher::slotOnTimelineNeedReveal(const QDate &pDate) {
  const auto timelineApi = m_apiManager->timeline();

  QPointer<TimelineModelFetcher> self = this;

  connect(timelineApi, &OpenAPI::OAITimelineApi::getTimeBucketSignal, this,
          [self, timelineApi,
           pDate](OpenAPI::OAITimeBucketAssetResponseDto assetsDto) {

           const auto cityList             = assetsDto.getCity();
           const auto countryList          = assetsDto.getCountry();
           const auto durationList         = assetsDto.getDuration();
           const auto fileCreatedAtList    = assetsDto.getFileCreatedAt();
           const auto idList               = assetsDto.getId();
           const auto isFavoriteList       = assetsDto.getIsFavorite();
           const auto isImageList          = assetsDto.getIsImage();
           const auto isTrashedList        = assetsDto.getIsTrashed();
           const auto latitudeList         = assetsDto.getLatitude();
           const auto livePhotoVideoIdList = assetsDto.getLivePhotoVideoId();
           const auto localOffsetHoursList = assetsDto.getLocalOffsetHours();
           const auto longitudeList        = assetsDto.getLongitude();
           const auto ownerIdList          = assetsDto.getOwnerId();
           const auto projectionTypeList   = assetsDto.getProjectionType();
           const auto ratioList            = assetsDto.getRatio();
           const auto stackList            = assetsDto.getStack();
           const auto thumbhashList        = assetsDto.getThumbhash();
           const auto visList              = assetsDto.getVisibility();

            QVector<Model::TimebucketModel *> models;


            if (idList.isEmpty()) {
              qWarning() << "timeBucket" << pDate.toString()
                         << "is empty on revealing";
              QMetaObject::invokeMethod(timelineApi, "deleteLater");
              return;
            }

            auto curDateTime = parseIsoTime(fileCreatedAtList[0]);
            if (!curDateTime.isValid()) {
              qWarning() << "first asset in timBucket:" << pDate.toString()
                         << "has invalid date:" << curDateTime.toString();
              QMetaObject::invokeMethod(timelineApi, "deleteLater");
              return;
            }
            auto curBucketAssets = QVector<Model::TimebucketModelAsset*>();

            if (!self) {
              QMetaObject::invokeMethod(timelineApi, "deleteLater");
              return;
            }

            auto modelPtr = self->m_model;

            for (int idx = 0; idx < idList.size(); ++idx) {
              const auto assetDateTime =
                  parseIsoTime(fileCreatedAtList[idx]);

              if (idx > 0 && assetDateTime.date() != curDateTime.date()) {
                auto newModel = new Model::TimebucketModel(assetDateTime.date(), curBucketAssets);

                if (!modelPtr) {
                  QMetaObject::invokeMethod(newModel, "deleteLater");
                  qDeleteAll(models);
                  QMetaObject::invokeMethod(timelineApi, "deleteLater");
                  return;
                }

                newModel->moveToThread(modelPtr->thread());
                models.append(newModel);

                curBucketAssets.clear();
                curDateTime = assetDateTime;
              }

              auto vis =
                  (idx < visList.size())
                      ? visList[idx].getValue()
                      : OpenAPI::OAIAssetVisibility::eOAIAssetVisibility::
                            INVALID_VALUE_OPENAPI_GENERATED;

              curBucketAssets.append(new Model::TimebucketModelAsset(
                  cityList.value(idx),
                  countryList.value(idx),
                  durationList.value(idx),
                  fileCreatedAtList.value(idx),
                  idList.value(idx),
                  isFavoriteList.value(idx),
                  isImageList.value(idx),
                  isTrashedList.value(idx),
                  latitudeList.value(idx),
                  livePhotoVideoIdList.value(idx),
                  localOffsetHoursList.value(idx),
                  longitudeList.value(idx),
                  ownerIdList.value(idx),
                  projectionTypeList.value(idx),
                  ratioList.value(idx),
                  stackList.value(idx).toVector(),
                  thumbhashList.value(idx), vis));
            }

            if (!modelPtr) {
              qDeleteAll(models);
              QMetaObject::invokeMethod(timelineApi, "deleteLater");
              return;
            }

            if (!curBucketAssets.isEmpty()) {
              auto b = new Model::TimebucketModel(curDateTime.date(), curBucketAssets);
              b->moveToThread(modelPtr->thread());
              models.append(b);
            }

            if (!QMetaObject::invokeMethod(
                    modelPtr, "slotRevealTimebucket", Qt::QueuedConnection,
                    Q_ARG(QDate, pDate),
                    Q_ARG(QVector<Immich::Model::TimebucketModel *>, models))) {
              qDeleteAll(models);
            }
            timelineApi->deleteLater();
          });

  connect(timelineApi, &OpenAPI::OAITimelineApi::getTimeBucketSignalError, this,
          [pDate, timelineApi](OpenAPI::OAITimeBucketAssetResponseDto, QNetworkReply::NetworkError error_type, const QString &error_str) {
            qWarning() << "error in fetch timeBucket" << pDate.toString() << "(ErrType:" << error_type << ")" << error_str;
            QMetaObject::invokeMethod(timelineApi, "deleteLater");
          });

  auto visibility = OpenAPI::OAIAssetVisibility();
  visibility.setValue(
      static_cast<OpenAPI::OAIAssetVisibility::eOAIAssetVisibility>(
          m_params->visibility));

  ;

  QString dateString = QLocale(QLocale::English, QLocale::UnitedStates).toString(pDate, "yyyy-MM-dd");

  timelineApi->getTimeBucket(
      pDate.toString("yyyy-MM-dd"), m_params->album_id, m_params->is_favorite,
      m_params->is_trashed, m_params->key, m_params->order, m_params->person_id,
      m_params->slug, m_params->tag_id, m_params->user_id, visibility,
      m_params->with_coordinates, m_params->with_partners,
      m_params->with_stacked);
}

QString TimelineModelFetcherParams::getAlbum_id() const {
  return valueOrDefault(album_id);
}

void TimelineModelFetcherParams::setAlbum_id(const QString &newAlbum_id) {
  if (isEqual(album_id, newAlbum_id))
    return;

  album_id = newAlbum_id;
  emit album_idChanged();
}

bool TimelineModelFetcherParams::getIs_favorite() const {
  return valueOrDefault(is_favorite);
}

void TimelineModelFetcherParams::setIs_favorite(const bool newIs_favorite) {
  if (isEqual(is_favorite, newIs_favorite))
    return;
  is_favorite = newIs_favorite;
  emit is_favoriteChanged();
}

bool TimelineModelFetcherParams::getIs_trashed() const {
  return valueOrDefault(is_trashed);
}

void TimelineModelFetcherParams::setIs_trashed(const bool newIs_trashed) {
  if (isEqual(is_trashed, newIs_trashed))
    return;
  is_trashed = newIs_trashed;
  emit is_trashedChanged();
}

QString TimelineModelFetcherParams::getKey() const {
  return valueOrDefault(key);
}

void TimelineModelFetcherParams::setKey(const QString &newKey) {
  if (isEqual(key, newKey))
    return;
  key = newKey;
  emit keyChanged();
}

OpenAPI::OAIAssetOrder::eOAIAssetOrder
TimelineModelFetcherParams::getOrder() const {
  return valueOrEnumDefault(order).getValue();
}

void TimelineModelFetcherParams::setOrder(
    const OpenAPI::OAIAssetOrder::eOAIAssetOrder newOrder) {
  if (isEnumEqual(order, newOrder))
    return;

  updateOptionalEnum(order, newOrder);

  emit orderChanged();
}

QString TimelineModelFetcherParams::getPerson_id() const {
  return valueOrDefault(person_id);
}

void TimelineModelFetcherParams::setPerson_id(const QString &newPerson_id) {
  if (isEqual(person_id, newPerson_id))
    return;
  person_id = newPerson_id;
  emit person_idChanged();
}

QString TimelineModelFetcherParams::getSlug() const {
  return valueOrDefault(slug);
}

void TimelineModelFetcherParams::setSlug(const QString &newSlug) {
  if (isEqual(slug, newSlug))
    return;
  slug = newSlug;
  emit slugChanged();
}

QString TimelineModelFetcherParams::getTag_id() const {
  return valueOrDefault(tag_id);
}

void TimelineModelFetcherParams::setTag_id(const QString &newTag_id) {
  if (isEqual(tag_id, newTag_id))
    return;
  tag_id = newTag_id;
  emit tag_idChanged();
}

QString TimelineModelFetcherParams::getUser_id() const {
  return valueOrDefault(user_id);
}

void TimelineModelFetcherParams::setUser_id(const QString &newUser_id) {
  if (isEqual(user_id, newUser_id))
    return;
  user_id = newUser_id;
  emit user_idChanged();
}

TimelineModelFetcherParams::AssetVisibility
TimelineModelFetcherParams::getVisibility() const {
  return visibility;
}

void TimelineModelFetcherParams::setVisibility(
    const AssetVisibility newVisibility) {
  if (visibility == newVisibility)
    return;

  visibility = newVisibility;

  emit visibilityChanged();
}

bool TimelineModelFetcherParams::getWith_coordinates() const {
  return valueOrDefault(with_coordinates);
}

void TimelineModelFetcherParams::setWith_coordinates(
    const bool newWith_coordinates) {
  if (isEqual(with_coordinates, newWith_coordinates))
    return;
  with_coordinates = newWith_coordinates;
  emit with_coordinatesChanged();
}

bool TimelineModelFetcherParams::getWith_partners() const {
  return valueOrDefault(with_partners);
}

void TimelineModelFetcherParams::setWith_partners(const bool newWith_partners) {
  if (isEqual(with_partners, newWith_partners))
    return;
  with_partners = newWith_partners;
  emit with_partnersChanged();
}

bool TimelineModelFetcherParams::getWith_stacked() const {
  return valueOrDefault(with_stacked);
}

void TimelineModelFetcherParams::setWith_stacked(const bool newWith_stacked) {
  if (isEqual(with_stacked, newWith_stacked))
    return;
  with_stacked = newWith_stacked;
  emit with_stackedChanged();
}

TimelineModelFetcherParams *TimelineModelFetcher::params() const {
  return m_params;
}

void TimelineModelFetcher::setParams(TimelineModelFetcherParams *newParams) {
  if (m_params == newParams)
    return;
  m_params = newParams;
  emit paramsChanged();
}
