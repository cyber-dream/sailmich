#include "mod-assets.h"

#include <QMetaObject>

#include <immich/models/album/timeline-model.h>
#include <immich/api/api-assets.h>
#include <src/utils/uuid-str.h>

using namespace Immich::Module::Assets;

ModAssets::ModAssets(QObject *parent)
    : BaseModule(parent), thumbnailsPath("/tmp/sailmich-fallback") {
} // TODO warning in log

ModAssets::ModAssets(const QString &thumbnailsPath, QObject *parent)
    : BaseModule(parent), thumbnailsPath(thumbnailsPath) {}

void ModAssets::requestThumbnail(QObject *model, const QUuid &assetId,
                                 const Api::Models::AssetMediaSize size,
                                 bool isSkipQueue) {
  if (assetId.isNull()) {
    qWarning() << "assetId is nil";
    return;
  }

  {
    QMutexLocker locker(&m_mutexTimebucket);
        qWarning() << "add" << assetId.toString() << "to queue";
    if (isSkipQueue) {
      m_timebucketQueue.prepend(ThumbnailTask{model, assetId, size});
    } else {
      m_timebucketQueue.enqueue(ThumbnailTask{model, assetId, size});
    }
  }

  if (m_isRunningTimebucket.load())
    return;

  m_isRunningTimebucket.store(true);

  QMetaObject::invokeMethod(this, "processNextThumbnail", Qt::QueuedConnection);
}

void ModAssets::slotRequestThumbnail(const QUuid &assetId,
                                     const Api::Models::AssetMediaSize size,
                                     bool isSkipQueue) {
  requestThumbnail(sender(), assetId, size, isSkipQueue);
}

void ModAssets::processNextThumbnail() {
  QMutexLocker locker(&m_mutexTimebucket);
  if (m_timebucketQueue.isEmpty()) {
    m_isRunningTimebucket.store(0);
    //    qWarning() << "queue is empty";
    return;
  }
  m_isRunningTimebucket.store(1);

  auto task = m_timebucketQueue.dequeue();
  locker.unlock();
  //  qWarning() << "run" << task.assetId.toString();

  const QString fullPath = thumbnailsPath + "/" +
                           Api::Models::AssetMediaSizeToString(task.size) +
                           "/" +uuidStr(task.assetId);

  //  ResultErrored<QString> res(fullPath);
  if (QFileInfo::exists(fullPath)) {
    QMetaObject::invokeMethod(this, "processNextThumbnail",
                              Qt::QueuedConnection);
    QMetaObject::invokeMethod(task.model, "slotOnThumbnailUpdate",
                              Qt::QueuedConnection, Q_ARG(QUuid, task.assetId),
                              Q_ARG(QString, fullPath));
    return;
  }

  Api::Assets::ApiAssets::ThumbnailQueryParams params;
  params.size = task.size;
  const auto &apiResult = api.assets->thumbnail(task.assetId, params);

  if (apiResult.code() != ResultPromiseBase::Succeeded) {
    QMetaObject::invokeMethod(this, "processNextThumbnail",
                              Qt::QueuedConnection);
    qWarning() << "error in thumbnail download:" << apiResult.error().message();
    return;
  }

  const auto dir = QFileInfo(fullPath).absoluteDir();
  if (!dir.exists()) {
    dir.mkpath(".");
  }

  QFile file(fullPath);
  if (!file.open(QIODevice::WriteOnly)) {
    QMetaObject::invokeMethod(this, "processNextThumbnail",
                              Qt::QueuedConnection);
    qWarning() << "can't open file" << fullPath;
    return;
  }

  file.write(apiResult.data());
  file.close();

  QMetaObject::invokeMethod(this, "processNextThumbnail", Qt::QueuedConnection);

  bool ok = QMetaObject::invokeMethod(
      task.model, "slotOnThumbnailUpdate", Qt::QueuedConnection,
      Q_ARG(QUuid, task.assetId), Q_ARG(QString, fullPath));
  if (!ok) {
    qWarning() << "Failed to invoke slotOnThumbnailUpdate! Check Q_INVOKABLE.";
  }
}
