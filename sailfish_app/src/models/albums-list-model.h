#ifndef ALBUMSLISTMODEL_H
#define ALBUMSLISTMODEL_H

#include <QAbstractListModel>

#include <OAIAlbumResponseDto.h>
#include <src/thumbnailer.h>

namespace Immich {
namespace Model {

class AlbumsListModel : public QAbstractListModel {
  Q_OBJECT
public:
  AlbumsListModel(QObject *parent = nullptr);

  enum Roles {
    AlbumIdRole = Qt::UserRole + 1,
    AlbumNameRole,
    AlbumSizeRole,
    AlbumThumbnailPathRole,
    AlbumInfoRole,
    AlbumIsSharedRole,
      AlbumDescriptionRole,
  };
  Q_ENUM(Roles)

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

signals:
  void needThumbnail(QString assetId, OpenAPI::OAIAssetMediaSize size,
                     bool isSkipQueue = false);
public slots:
  void slotAppendAlbumsBatch(
      const QVector<OpenAPI::OAIAlbumResponseDto> pAlbums);
  void slotThumbnailUpdate(const QString pAssetId, const QString pPath);

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QVector<OpenAPI::OAIAlbumResponseDto> m_albums;
  Immich::Thumbnailer *m_thumbnailer;
  QHash<QUuid, QString> m_thumbnailsCache;
};
} // namespace Model
} // namespace Immich
#endif // ALBUMSLISTMODEL_H
