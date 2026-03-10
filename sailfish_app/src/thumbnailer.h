#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <OAIAssetsApi.h>
#include <QObject>
#include <src/api-manager.h>

namespace Immich {

class ThumbnailWatcher : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString path READ path NOTIFY finished)
public:
  explicit ThumbnailWatcher(QObject *parent = nullptr) : QObject(parent) {}

  QString path() const;
  void setPath(const QString &p);
signals:
  void finished();

private:
  QString m_path;
};

class Thumbnailer : public QObject {
  Q_OBJECT
public:
  explicit Thumbnailer(QObject *parent = nullptr);

  Q_INVOKABLE Immich::ThumbnailWatcher *
  getThumbnailSize(const QString &pAssetId);
  Q_INVOKABLE Immich::ThumbnailWatcher *getPreviewSize(const QString &pAssetId);

  Immich::ThumbnailWatcher *
  getAssetThumbnail(const QString &pAssetId,
                    const OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize pSize);
signals:
public slots:
  void slotNeedNetworkFetch(const QString& pAssetId, OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize pSize,
                            const QString &fullPath, Immich::ThumbnailWatcher *watcher);

private:
  inline void createDir(const OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize pSize )const;


  QString m_thumbnailsPath;
  ApiManager *m_apiManager;

  inline QString
  assetFullPath(QString pAssetId,
                OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize pSize) const;

  QHash<QString, ThumbnailWatcher*> m_currentTasks;
};
} // namespace Immich
#endif // THUMBNAILER_H
