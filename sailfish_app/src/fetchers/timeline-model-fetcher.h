#ifndef TIMELINEMODELFETCHER_H
#define TIMELINEMODELFETCHER_H

#include <OAIEnum.h>
#include <QObject>
#include <QPointer>

// TODO to forward

#include <optional.hpp>
#include <src/models/timebucket-model.h>
#include <src/models/timeline-model.h>
#include <src/utils/oai-request-params-qobject.h>

namespace Immich {
namespace Fetcher {
class TimelineModelFetcherParams : public QObject, OpenAPI::OAIRequestParams {
  Q_OBJECT
  Q_PROPERTY(QString album_id READ getAlbum_id WRITE setAlbum_id NOTIFY
                 album_idChanged)
  Q_PROPERTY(bool is_favorite READ getIs_favorite WRITE setIs_favorite NOTIFY
                 is_favoriteChanged)
  Q_PROPERTY(bool is_trashed READ getIs_trashed WRITE setIs_trashed NOTIFY
                 is_trashedChanged)
  Q_PROPERTY(QString key READ getKey WRITE setKey NOTIFY keyChanged)
  Q_PROPERTY(OpenAPI::OAIAssetOrder::eOAIAssetOrder order READ getOrder WRITE
                 setOrder NOTIFY orderChanged)
  Q_PROPERTY(QString person_id READ getPerson_id WRITE setPerson_id NOTIFY
                 person_idChanged)
  Q_PROPERTY(QString slug READ getSlug WRITE setSlug NOTIFY slugChanged)
  Q_PROPERTY(QString tag_id READ getTag_id WRITE setTag_id NOTIFY tag_idChanged)
  Q_PROPERTY(
      QString user_id READ getUser_id WRITE setUser_id NOTIFY user_idChanged)
  Q_PROPERTY(AssetVisibility visibility READ getVisibility WRITE setVisibility
                 NOTIFY visibilityChanged)
  Q_PROPERTY(bool with_coordinates READ getWith_coordinates WRITE
                 setWith_coordinates NOTIFY with_coordinatesChanged)
  Q_PROPERTY(bool with_partners READ getWith_partners WRITE setWith_partners
                 NOTIFY with_partnersChanged)
  Q_PROPERTY(bool with_stacked READ getWith_stacked WRITE setWith_stacked NOTIFY
                 with_stackedChanged)
  friend class TimelineModelFetcher;

public:
  TimelineModelFetcherParams(QObject *parent = nullptr);

  enum AssetVisibility {
    AssetVisibilityArchive = static_cast<int>(
        OpenAPI::OAIAssetVisibility::eOAIAssetVisibility::ARCHIVE),
    AssetVisibilityTimeline = static_cast<int>(
        OpenAPI::OAIAssetVisibility::eOAIAssetVisibility::TIMELINE),
    AssetVisibilityHidden = static_cast<int>(
        OpenAPI::OAIAssetVisibility::eOAIAssetVisibility::HIDDEN),
    AssetVisibilityLocked = static_cast<int>(
        OpenAPI::OAIAssetVisibility::eOAIAssetVisibility::LOCKED)
  };
  Q_ENUM(AssetVisibility)

  QString getAlbum_id() const;
  void setAlbum_id(const QString &newAlbum_id);

  bool getIs_favorite() const;
  void setIs_favorite(const bool newIs_favorite);

  bool getIs_trashed() const;
  void setIs_trashed(const bool newIs_trashed);

  QString getKey() const;
  void setKey(const QString &newKey);

  OpenAPI::OAIAssetOrder::eOAIAssetOrder getOrder() const;
  void setOrder(const OpenAPI::OAIAssetOrder::eOAIAssetOrder newOrder);

  QString getPerson_id() const;
  void setPerson_id(const QString &newPerson_id);

  QString getSlug() const;
  void setSlug(const QString &newSlug);

  QString getTag_id() const;
  void setTag_id(const QString &newTag_id);

  QString getUser_id() const;
  void setUser_id(const QString &newUser_id);

  AssetVisibility getVisibility() const;
  void setVisibility(const AssetVisibility newVisibility);

  bool getWith_coordinates() const;
  void setWith_coordinates(const bool newWith_coordinates);

  bool getWith_partners() const;
  void setWith_partners(const bool newWith_partners);

  bool getWith_stacked() const;
  void setWith_stacked(const bool newWith_stacked);

signals:
  void album_idChanged();

  void is_favoriteChanged();

  void is_trashedChanged();

  void keyChanged();

  void orderChanged();

  void person_idChanged();

  void slugChanged();

  void tag_idChanged();

  void user_idChanged();

  void visibilityChanged();

  void with_coordinatesChanged();

  void with_partnersChanged();

  void with_stackedChanged();

private:
  OpenAPI::OptionalParam<QString> album_id;
  OpenAPI::OptionalParam<bool> is_favorite;
  OpenAPI::OptionalParam<bool> is_trashed;
  OpenAPI::OptionalParam<QString> key;
  OpenAPI::OptionalParam<OpenAPI::OAIAssetOrder> order;
  OpenAPI::OptionalParam<QString> person_id;
  OpenAPI::OptionalParam<QString> slug;
  OpenAPI::OptionalParam<QString> tag_id;
  OpenAPI::OptionalParam<QString> user_id;
  AssetVisibility visibility = AssetVisibility::AssetVisibilityTimeline;
  OpenAPI::OptionalParam<bool> with_coordinates;
  OpenAPI::OptionalParam<bool> with_partners;
  OpenAPI::OptionalParam<bool> with_stacked;
};

class TimelineModelFetcher : public QObject {
  Q_OBJECT
  Q_PROPERTY(Immich::Model::TimelineModel *model READ model WRITE setModel
                 NOTIFY modelChanged)
  Q_PROPERTY(TimelineModelFetcherParams *params READ params WRITE setParams
                 NOTIFY paramsChanged)
public:
  explicit TimelineModelFetcher(QObject *parent = nullptr);
  ~TimelineModelFetcher();

  Model::TimelineModel *model() const;
  void setModel(Model::TimelineModel *newModel);

  TimelineModelFetcherParams *params() const;
  void setParams(TimelineModelFetcherParams *newParams);

  Q_INVOKABLE void fetch() ;

public slots:
  void slotOnTimelineNeedReveal(const QDate &pDate);

signals:
  void modelChanged();

  void paramsChanged();

private:
  QPointer<Model::TimelineModel > m_model;
  ApiManager *m_apiManager;
  TimelineModelFetcherParams *m_params;
};

} // namespace Fetcher
} // namespace Immich
#endif // TIMELINEMODELFETCHER_H
