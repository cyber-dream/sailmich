#ifndef ALBUMSLISTMODEL_H
#define ALBUMSLISTMODEL_H

#include <QAbstractListModel>
#include <immich/api/dto/_albums.h>

namespace Immich {
// namespace Api {
// namespace Albums {
// struct AlbumResponseDto;
// }
// } // namespace Api
namespace Model {
namespace Albums {

struct AlbumInfo {
public:
  AlbumInfo() : info() {};
  AlbumInfo(const Api::Albums::AlbumResponseDto &pAlbum) : info(pAlbum) {};

  const Api::Albums::AlbumResponseDto info;
  QString thumbnailPath = "";
};

class AlbumsListModel : public QAbstractListModel {
  Q_OBJECT
public:
  AlbumsListModel(QObject *parent = nullptr);
  AlbumsListModel(const QVector<AlbumInfo> &pAlbums, QObject *parent = nullptr);
  enum Roles {
    AlbumIdRole = Qt::UserRole + 1,
    AlbumNameRole,
    AlbumSizeRole,
    AlbumThumbnailPathRole,
    AlbumInfoRole,
  };
  Q_ENUM(Roles)

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  void setAlbums(const QVector<AlbumInfo> &pAlbums);

signals:
  void needThumbnail(QUuid assetId, Api::Models::AssetMediaSize size,
                     bool isSkipQueue = false);
public slots:
  void slotOnThumbnailUpdate(const QUuid pAssetId, const QString pPath);

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QVector<AlbumInfo> m_albums;
};
} // namespace Albums
} // namespace Model
} // namespace Immich
#endif // ALBUMSLISTMODEL_H
