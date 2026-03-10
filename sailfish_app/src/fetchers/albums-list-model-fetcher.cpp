#include "albums-list-model-fetcher.h"
#include <QtConcurrent>
#include <dependencypointer.h>

using namespace Immich::Fetcher;

AlbumsListModelFetcher::AlbumsListModelFetcher(QObject *parent)
    : QObject(parent), m_params(new AlbumsListModelFetcherParams()),
      m_apiManager(QInjection::Pointer<ApiManager>()) {}

Immich::Model::AlbumsListModel *AlbumsListModelFetcher::model() const {
  return m_model;
}

void AlbumsListModelFetcher::setModel(Model::AlbumsListModel *newModel) {
  if (m_model == newModel)
    return;

  if (m_model != nullptr) {
    m_model->disconnect(this);
  }

  m_model = newModel;

  emit modelChanged();

  if (m_model == nullptr)
    return;

  switch (m_params->ownershipType) {
  case Immich::Fetcher::AlbumsListModelFetcherParams::OwnershipTypeOwned:
    fetchAlbums(false);
    break;
  case Immich::Fetcher::AlbumsListModelFetcherParams::OwnershipTypeShared:
    fetchAlbums(true);
    break;
  case Immich::Fetcher::AlbumsListModelFetcherParams::OwnershipTypeAll:
    fetchAlbums(true);
    fetchAlbums(false);
    break;
  }
}

AlbumsListModelFetcherParams *AlbumsListModelFetcher::params() const {
  return m_params;
}

void AlbumsListModelFetcher::setParams(
    AlbumsListModelFetcherParams *newParams) {
  if (m_params == newParams)
    return;
  m_params = newParams;
  emit paramsChanged();
}
//TODO move from main thread
void AlbumsListModelFetcher::fetchAlbums(bool pIsShared) const {
  const auto apiAlbums = m_apiManager->albums();
  connect(apiAlbums, &OpenAPI::OAIAlbumsApi::getAllAlbumsSignal, this,
          [this](QList<OpenAPI::OAIAlbumResponseDto> res) {
            QVector<OpenAPI::OAIAlbumResponseDto> batch = res.toVector();

            QMetaObject::invokeMethod(
                m_model, "slotAppendAlbumsBatch", Qt::QueuedConnection,
                Q_ARG(QVector<OpenAPI::OAIAlbumResponseDto>, batch));
          });

  apiAlbums->getAllAlbums(m_params->asset_id, pIsShared ? true : false);
}

QString AlbumsListModelFetcherParams::getAsset_id() const {
  return valueOrDefault(asset_id);
}

void AlbumsListModelFetcherParams::setAsset_id(const QString &newAsset_id) {
  if (isEqual(asset_id, newAsset_id))
    return;

  asset_id = newAsset_id;
  emit asset_idChanged();
}

AlbumsListModelFetcherParams::OwnershipType
AlbumsListModelFetcherParams::getOwnershipType() const {
  return ownershipType;
}

void AlbumsListModelFetcherParams::setOwnershipType(
    OwnershipType newOwnershipType) {
  if (ownershipType == newOwnershipType)
    return;

  ownershipType = newOwnershipType;
  emit ownershipTypeChanged();
}
