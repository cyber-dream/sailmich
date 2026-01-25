#include "albums-list-model.h"
#include <src/immich/api/dto/_albums.h>
#include <src/immich/immich-plugin.h>
#include <src/immich/modules/assets.h>

using namespace Immich::Model::Albums;

AlbumsListModel::AlbumsListModel(QObject *parent) : QAbstractListModel(parent) {
  const auto mod_assets = qobject_cast<Module::Assets::ModAssets *>(
      ImmichPlugin::assetsSingletonProvider(nullptr, nullptr));

  connect(this, &AlbumsListModel::needThumbnail, mod_assets,
          &Module::Assets::ModAssets::slotRequestThumbnail);
}

AlbumsListModel::AlbumsListModel(const QVector<AlbumInfo> &pAlbums,
                                 QObject *parent)
    : QAbstractListModel(parent), m_albums(pAlbums) {
  const auto mod_assets = qobject_cast<Module::Assets::ModAssets *>(
      ImmichPlugin::assetsSingletonProvider(nullptr, nullptr));

  connect(this, &AlbumsListModel::needThumbnail, mod_assets,
          &Module::Assets::ModAssets::slotRequestThumbnail);
}

int AlbumsListModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return m_albums.size();
}

QVariant AlbumsListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  int rowIdx = index.row();
  if (rowIdx < 0 || rowIdx >= rowCount())
    return QVariant();

  const auto &album = m_albums[rowIdx];

  switch (role) {
  case AlbumIdRole:
    return album.info.id;
  case AlbumNameRole:
    return album.info.albumName;
  case AlbumSizeRole:
    return album.info.assetCount;
  case AlbumThumbnailPathRole:
    if (!album.thumbnailPath.isEmpty())
      return album.thumbnailPath;

    emit const_cast<Model::Albums::AlbumsListModel *>(this)->needThumbnail(
        album.info.albumThumbnailAssetId, Api::Models::AssetMediaSize::THUMBNAIL);

    return QVariant();
  case AlbumInfoRole:
    return QVariant::fromValue(album.info);
  default:
    return QVariant();
  }
};

void AlbumsListModel::setAlbums(const QVector<AlbumInfo> &pAlbums) {
  beginResetModel();
  m_albums.clear();
  endResetModel();

  beginInsertRows(QModelIndex(), 0, pAlbums.size() - 1);
  m_albums.reserve(pAlbums.size());
  m_albums = pAlbums;
  endInsertRows();
}

void AlbumsListModel::slotOnThumbnailUpdate(const QUuid pAssetId,
                                            const QString pPath) {
  for (int idx = 0; idx < m_albums.size(); idx++) {
    if (m_albums[idx].info.albumThumbnailAssetId != pAssetId)
      continue;

    m_albums[idx].thumbnailPath = pPath;
    const QModelIndex modelIndex = index(idx);
    emit dataChanged(modelIndex, modelIndex, {AlbumThumbnailPathRole});
    break;
  }
}

QHash<int, QByteArray> AlbumsListModel::roleNames() const {
  return {{AlbumIdRole, "albumId"},
          {AlbumNameRole, "albumName"},
          {AlbumSizeRole, "albumSize"},
          {AlbumThumbnailPathRole, "albumThumbnailPath"},
          {AlbumInfoRole, "albumInfo"}};
}
