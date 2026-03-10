#include "thumbnailer.h"
#include <QtConcurrent>
#include <dependencypointer.h>
#include <src/utils/asset-media-size-str.h>

using namespace Immich;

Thumbnailer::Thumbnailer(QObject *parent)
    : QObject(parent), m_thumbnailsPath(QStandardPaths::writableLocation(
                                            QStandardPaths::CacheLocation) +
                                        "/thumbnails"),
      m_apiManager(QInjection::Pointer<ApiManager>()) {
  QDir dir;

  createDir(OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::ORIGINAL);
  createDir(OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::FULLSIZE);
  createDir(OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::PREVIEW);
  createDir(OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::THUMBNAIL);
}

void Thumbnailer::createDir(
    const OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize pSize) const {
  const auto path = m_thumbnailsPath + "/" + assetMediaSizeToStr(pSize);

  QDir dir;
  if (!dir.mkpath(path)) {
    qWarning() << "can't create directory for" << assetMediaSizeToStr(pSize)
               << "size";
  }
}

ThumbnailWatcher *Thumbnailer::getThumbnailSize(const QString &pAssetId) {
  return getAssetThumbnail(
      pAssetId, OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::THUMBNAIL);
}

ThumbnailWatcher *Thumbnailer::getPreviewSize(const QString &pAssetId) {
  return getAssetThumbnail(
      pAssetId, OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::PREVIEW);
}

ThumbnailWatcher *Thumbnailer::getAssetThumbnail(
    const QString &pAssetId,
    const OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize pSize) {
  if (pAssetId.isNull()) {
    qWarning() << "assetId is null";
    return nullptr;
  }

  if (m_currentTasks.contains(pAssetId)) {
    if (m_currentTasks[pAssetId] != nullptr)
      return m_currentTasks[pAssetId];
    else
      m_currentTasks.remove(pAssetId);
  }

  ThumbnailWatcher *watcher = new ThumbnailWatcher();
  connect(watcher, &ThumbnailWatcher::destroyed, this,
          [this, pAssetId]() { m_currentTasks.remove(pAssetId); });

  m_currentTasks.insert(pAssetId, watcher);

  QtConcurrent::run([watcher, pAssetId, pSize, this]() {
    const QString fullPath = assetFullPath(pAssetId, pSize);

    if (QFileInfo::exists(fullPath)) {
      watcher->setPath(fullPath);
      return;
    }

    QMetaObject::invokeMethod(
        this, "slotNeedNetworkFetch", Qt::QueuedConnection,
        Q_ARG(QString, pAssetId),
        Q_ARG(OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize, pSize),
        Q_ARG(QString, fullPath), Q_ARG(Immich::ThumbnailWatcher *, watcher));
  });

  return watcher;
}

void Thumbnailer::slotNeedNetworkFetch(
    const QString &pAssetId,
    OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize pSize,
    const QString &fullPath, Immich::ThumbnailWatcher *watcher)

{
  const auto assetsApi = m_apiManager->assets();

  OpenAPI::OAIAssetMediaSize size;
  size.setValue(pSize);

  QPointer<ThumbnailWatcher> safeWatcher(watcher);

  connect(assetsApi, &OpenAPI::OAIAssetsApi::viewAssetSignal,
          [assetsApi, safeWatcher, fullPath](OpenAPI::OAIHttpFileElement file) {
            QtConcurrent::run([safeWatcher, assetsApi, file, fullPath]() {
              if (!safeWatcher) {
                assetsApi->deleteLater();
                return;
              }

              if (QFile::exists(fullPath)) {
                qWarning() << "file aready exists" << fullPath;
                safeWatcher->setPath(fullPath);
                assetsApi->deleteLater();
                return;
              }

              QString tempPath = file.local_filename;
              QString finalPath = fullPath;

              int maxWaitMs = 5000;
              int waited = 0;
              const int step = 10;

              while (!QFile::exists(tempPath) && waited < maxWaitMs) {
                QThread::msleep(step);
                waited += step;
              }

              if (!QFile::rename(tempPath, finalPath)) {
                if (QFile::copy(tempPath, finalPath)) {
                  QFile::remove(tempPath);
                  safeWatcher->setPath(fullPath);
                } else {
                  QFile::remove(tempPath);
                  qWarning()
                      << "Failed to move file" << tempPath << "->" << finalPath;
                  safeWatcher->setPath(fullPath);
                }
              } else {
                QFile::remove(tempPath);
                safeWatcher->setPath(fullPath);
              }

              QMetaObject::invokeMethod(assetsApi, "deleteLater");
            });
          });

  connect(assetsApi, &OpenAPI::OAIAssetsApi::viewAssetSignalError,
          [assetsApi](OpenAPI::OAIHttpFileElement, QNetworkReply::NetworkError,
                      QString error) {
            qWarning() << "Fetch error:" << error;
            assetsApi->deleteLater();
          });

  assetsApi->viewAsset(pAssetId, true, {}, size, {});
}

QString Thumbnailer::assetFullPath(
    QString pAssetId,
    OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize pSize) const {
  return m_thumbnailsPath + "/" + assetMediaSizeToStr(pSize) + "/" + pAssetId;
}

QString ThumbnailWatcher::path() const { return m_path; }

void ThumbnailWatcher::setPath(const QString &p) {
  m_path = p;
  emit finished();
}
