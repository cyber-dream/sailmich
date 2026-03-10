#ifndef ALBUMSFETCHER_H
#define ALBUMSFETCHER_H

#include <OAIAlbumsApi.h>
#include <QObject>
#include <src/api-manager.h>
#include <src/models/albums-list-model.h>
#include <src/utils/oai-request-params-qobject.h>

namespace Immich {
namespace Fetcher {

class AlbumsListModelFetcherParams : public QObject, OpenAPI::OAIRequestParams {
  Q_OBJECT
  Q_PROPERTY(QString asset_id READ getAsset_id WRITE setAsset_id NOTIFY
                 asset_idChanged)
  Q_PROPERTY(OwnershipType ownershipType READ getOwnershipType WRITE setOwnershipType
                 NOTIFY ownershipTypeChanged)
  friend class AlbumsListModelFetcher;

public:
  AlbumsListModelFetcherParams(QObject *parent = nullptr) : QObject(parent) {}

  enum OwnershipType {
    OwnershipTypeOwned,
    OwnershipTypeShared,
    OwnershipTypeAll
  };
  Q_ENUM(OwnershipType)

  QString getAsset_id() const;
  void setAsset_id(const QString &newAsset_id);

  OwnershipType getOwnershipType() const;
  void setOwnershipType(OwnershipType newOwnershipType);

signals:
  void asset_idChanged();

  void ownershipTypeChanged();

private:
  OpenAPI::OptionalParam<QString> asset_id;
  OwnershipType ownershipType;
};

class AlbumsListModelFetcher : public QObject {
  Q_OBJECT
  Q_PROPERTY(Immich::Model::AlbumsListModel *model READ model WRITE setModel
                 NOTIFY modelChanged)
  Q_PROPERTY(AlbumsListModelFetcherParams *params READ params WRITE setParams
                 NOTIFY paramsChanged)
public:
  explicit AlbumsListModelFetcher(QObject *parent = nullptr);

  Model::AlbumsListModel *model() const;
  void setModel(Model::AlbumsListModel *newModel);

  AlbumsListModelFetcherParams *params() const;
  void setParams(AlbumsListModelFetcherParams *newParams);

signals:
  void modelChanged();

  void paramsChanged();

private:
  void fetchAlbums(bool pIsShared) const;

  AlbumsListModelFetcherParams *m_params = nullptr;
  Model::AlbumsListModel *m_model = nullptr;
  ApiManager *m_apiManager = nullptr;
};
} // namespace Fetcher
} // namespace Immich
#endif // ALBUMSFETCHER_H
