#ifndef TIMEBUCKETMODEL_H
#define TIMEBUCKETMODEL_H

#include <OAITimelineApi.h>
#include <QAbstractListModel>
#include <QDate>
#include <src/thumbnailer.h>

namespace Immich {
namespace Model {
class MediaViewModel;

struct TimebucketModelAsset : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
  Q_PROPERTY(
      QString country READ country WRITE setCountry NOTIFY countryChanged)
  Q_PROPERTY(
      QString duration READ duration WRITE setDuration NOTIFY durationChanged)
  Q_PROPERTY(QString file_created_at READ file_created_at WRITE
                 setFile_created_at NOTIFY file_created_atChanged)
  Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
  Q_PROPERTY(QString thumbhash READ thumbhash WRITE setThumbhash NOTIFY
                 thumbhashChanged)
  Q_PROPERTY(
      QString owner_id READ owner_id WRITE setOwner_id NOTIFY owner_idChanged)
  Q_PROPERTY(QString projection_type READ projection_type WRITE
                 setProjection_type NOTIFY projection_typeChanged)
  Q_PROPERTY(QString live_photo_video_id READ live_photo_video_id WRITE
                 setLive_photo_video_id NOTIFY live_photo_video_idChanged)
  Q_PROPERTY(QString thumbnailPath READ thumbnailPath WRITE setThumbnailPath
                 NOTIFY thumbnailPathChanged)
  Q_PROPERTY(QString previewPath READ previewPath WRITE setPreviewPath NOTIFY
                 previewPathChanged)
  Q_PROPERTY(bool is_favorite READ is_favorite WRITE setIs_favorite NOTIFY
                 is_favoriteChanged)
  Q_PROPERTY(
      bool is_image READ is_image WRITE setIs_image NOTIFY is_imageChanged)
  Q_PROPERTY(bool is_trashed READ is_trashed WRITE setIs_trashed NOTIFY
                 is_trashedChanged)
  Q_PROPERTY(bool isThumbnailExists READ isThumbnailExists WRITE
                 setIsThumbnailExists NOTIFY isThumbnailExistsChanged)
  Q_PROPERTY(bool isPreviewExists READ isPreviewExists WRITE setIsPreviewExists
                 NOTIFY isPreviewExistsChanged)
  Q_PROPERTY(
      double latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
  Q_PROPERTY(double local_offset_hours READ local_offset_hours WRITE
                 setLocal_offset_hours NOTIFY local_offset_hoursChanged)
  Q_PROPERTY(double longitude READ longitude WRITE setLongitude NOTIFY
                 longitudeChanged)
  Q_PROPERTY(double ratio READ ratio WRITE setRatio NOTIFY ratioChanged)
  //  Q_PROPERTY(OpenAPI::OAIAssetVisibility::eOAIAssetVisibility visibility
  //  READ
  //                 visibility WRITE setVisibility NOTIFY visibilityChanged)
  //  Q_PROPERTY(
  //      QVector<QString> stack READ stack WRITE setStack NOTIFY stackChanged)
  friend class TimebucketModel;
  friend class MediaViewModel;

public:
  TimebucketModelAsset(QObject *pParent = nullptr) : QObject(pParent) {};
  TimebucketModelAsset(
      const QString &pCity, const QString &pCountry, const QString &pDuration,
      const QString &pFile_created_at, const QString &pId,
      const bool &pIsFavorite, const bool &pIsImage, const bool &pIsTrashed,
      const double &pLatitude, const QString &pLivePhotoVideoId,
      const double &pLocalOffsetHours, const double &pLongitude,
      const QString &pOwnerId, const QString &pProjectionType,
      const double &pRatio, const QVector<QString> &pStack,
      const QString &pThumbhash,
      const OpenAPI::OAIAssetVisibility::eOAIAssetVisibility &pAssetVisibility, QObject *pParent = nullptr);

  const QString &city() const;
  void setCity(const QString &newCity);

  const QString &country() const;
  void setCountry(const QString &newCountry);

  const QString &duration() const;
  void setDuration(const QString &newDuration);

  const QString &file_created_at() const;
  void setFile_created_at(const QString &newFile_created_at);

  const QString &id() const;
  void setId(const QString &newId);

  const QString &thumbhash() const;
  void setThumbhash(const QString &newThumbhash);

  const QString &owner_id() const;
  void setOwner_id(const QString &newOwner_id);

  const QString &projection_type() const;
  void setProjection_type(const QString &newProjection_type);

  const QString &live_photo_video_id() const;
  void setLive_photo_video_id(const QString &newLive_photo_video_id);

  const QString &thumbnailPath() const;
  void setThumbnailPath(const QString &newThumbnailPath);

  const QString &previewPath() const;
  void setPreviewPath(const QString &newPreviewPath);

  bool is_favorite() const;
  void setIs_favorite(bool newIs_favorite);

  bool is_image() const;
  void setIs_image(bool newIs_image);

  bool is_trashed() const;
  void setIs_trashed(bool newIs_trashed);

  bool isThumbnailExists() const;
  void setIsThumbnailExists(bool newIsThumbnailExists);

  bool isPreviewExists() const;
  void setIsPreviewExists(bool newIsPreviewExists);

  double latitude() const;
  void setLatitude(double newLatitude);

  double local_offset_hours() const;
  void setLocal_offset_hours(double newLocal_offset_hours);

  double longitude() const;
  void setLongitude(double newLongitude);

  double ratio() const;
  void setRatio(double newRatio);

  OpenAPI::OAIAssetVisibility::eOAIAssetVisibility visibility() const;
  void
  setVisibility(OpenAPI::OAIAssetVisibility::eOAIAssetVisibility newVisibility);

  const QVector<QString> &stack() const;
  void setStack(const QVector<QString> &newStack);

signals:
  void cityChanged();

  void countryChanged();

  void durationChanged();

  void file_created_atChanged();

  void idChanged();

  void thumbhashChanged();

  void owner_idChanged();

  void projection_typeChanged();

  void live_photo_video_idChanged();

  void thumbnailPathChanged();

  void previewPathChanged();

  void is_favoriteChanged();

  void is_imageChanged();

  void is_trashedChanged();

  void isThumbnailExistsChanged();

  void isPreviewExistsChanged();

  void latitudeChanged();

  void local_offset_hoursChanged();

  void longitudeChanged();

  void ratioChanged();

  void visibilityChanged();

  void stackChanged();

private:
  QString m_city;
  QString m_country;
  QString m_duration;
  QString m_file_created_at;
  QString m_id;
  QString m_thumbhash;
  QString m_owner_id;
  QString m_projection_type;
  QString m_live_photo_video_id;
  QString m_thumbnailPath;
  QString m_previewPath;
  bool m_is_favorite = false;
  bool m_is_image = false;
  bool m_is_trashed = false;
  bool m_isThumbnailExists = false;
  bool m_isPreviewExists = false;
  double m_latitude = 0;
  double m_local_offset_hours = 0;
  double m_longitude = 0;
  double m_ratio = 0;
  OpenAPI::OAIAssetVisibility::eOAIAssetVisibility m_visibility = OpenAPI::
      OAIAssetVisibility::eOAIAssetVisibility::INVALID_VALUE_OPENAPI_GENERATED;
  QVector<QString> m_stack;
};

class TimebucketModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(RevealStatus status READ status NOTIFY statusChanged)
public:
  friend class MediaViewModel;
  TimebucketModel(QDate pDate, int pAssetCount, QObject *parent = nullptr);
  TimebucketModel(const QDate &pDate, const QVector<TimebucketModelAsset*> &pAssets,
                  QObject *parent = nullptr);

  enum RevealStatus {
    RevealStatusNotRevealed,
    RevealStatusLoading,
    RevealStatusRevealed
  };
  Q_ENUM(RevealStatus)

  enum Roles { AssetId = Qt::UserRole + 1, ThumbnailSource };
  Q_ENUM(Roles)

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  QDate date() const;

  RevealStatus status() const;
  void setStatus(RevealStatus pStatus);

signals:
  void needReveal();
  void statusChanged();

public slots:
  void updateThumbnail(const QString &pAssetId,
                       const QString &pPath); // TODO rename to slotBlaBla

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QVector<TimebucketModelAsset *> m_assets;
  QDate m_date;
  int m_assetCount = 0;
  RevealStatus m_status = RevealStatusNotRevealed;
  Thumbnailer *m_thumbnailer;
  //  mutable QHash<QString, QString> m_thumbnailsCache;
};

} // namespace Model
} // namespace Immich
#endif // TIMEBUCKETMODEL_H
