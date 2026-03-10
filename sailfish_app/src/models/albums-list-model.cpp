#include "albums-list-model.h"
#include "dependencypointer.h"

using namespace Immich::Model;

AlbumsListModel::AlbumsListModel(QObject *parent)
    : QAbstractListModel(parent),
      m_thumbnailer(QInjection::Pointer<Immich::Thumbnailer>()) {}

int AlbumsListModel::rowCount(const QModelIndex &) const {
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
    return album.getId();
  case AlbumNameRole:
    return album.getAlbumName();
  case AlbumSizeRole:
    return album.getAssetCount();
  case AlbumThumbnailPathRole: {
    if (m_thumbnailsCache.contains(album.getAlbumThumbnailAssetId()))
      return m_thumbnailsCache.value(album.getAlbumThumbnailAssetId());

    const auto watcher =
        m_thumbnailer->getThumbnailSize(album.getAlbumThumbnailAssetId());

    connect(watcher, &ThumbnailWatcher::finished, [this, album, watcher]() {
      QMetaObject::invokeMethod(
          const_cast<AlbumsListModel *>(this), "slotThumbnailUpdate",
          Qt::QueuedConnection,
          Q_ARG(QString, album.getAlbumThumbnailAssetId()),
          Q_ARG(QString, watcher->path()));

      watcher->deleteLater();
    });

    return "";
  }
  case AlbumInfoRole:
    return QVariant::fromValue(album.asJsonObject());

  case AlbumDescriptionRole:
    return album.getDescription();
  case AlbumIsSharedRole:
    return album.isShared();
  default:
    return QVariant();
  }
}

void AlbumsListModel::slotAppendAlbumsBatch(
    const QVector<OpenAPI::OAIAlbumResponseDto> pAlbums) {
  if (pAlbums.isEmpty())
    return;

  int firstRow = m_albums.size();
  int lastRow = m_albums.size() + pAlbums.size() - 1;

  beginInsertRows(QModelIndex(), firstRow, lastRow);

  m_albums.append(pAlbums);

  endInsertRows();
};

void AlbumsListModel::slotThumbnailUpdate(const QString pAssetId,
                                          const QString pPath) {
  for (int idx = 0; idx < m_albums.size(); idx++) {
    if (m_albums[idx].getAlbumThumbnailAssetId() != pAssetId)
      continue;

    m_thumbnailsCache.insert(pAssetId, pPath);
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
          {AlbumInfoRole, "albumInfo"},
          {AlbumDescriptionRole, "albumDescription"},
          {AlbumIsSharedRole, "albumIsShared"}};
}
