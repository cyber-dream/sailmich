#include "immich/api/dto/_albums.h"

#include "immich/api/api-albums.h"
#include "immich/api/api-timeline.h"
#include "immich/immich-plugin.h"
#include "immich/models/album/timeline-model.h"
#include "immich/models/albums/albums-list-model.h"

#include "mod-album.h"
#include <QDebug>
#include <QVector>

using namespace Immich::Module::Album;

ModAlbum::ModAlbum(QObject *parent) : BaseModule(parent) {}

Immich::ResultPromise<bool> *
ModAlbum::updateAlbumModel(Model::Album::TimelineAlbumModel *model,
                           const QUuid &albumId) const {
  return new ResultPromise<bool>([model, albumId]() {
    Api::Timeline::ApiTimeline::GetBucketsQueryParam params;
    if (!albumId.isNull()) {
      params.albumId = albumId;
    }

    params.order = Api::Models::AssetOrder::DESC;
    const auto &res = api.timeline->getBuckets(params);

    if (res.code() == ResultPromiseBase::Failed) {
      return ResultErrored<bool>(ResultError(res.error()));
    }

    QVector<Model::Album::TimelineTimeBucketModel *> rows;
    rows.reserve(res.data().size());
    for (int idx = 0; idx < res.data().size(); ++idx) {
      const auto &b = res.data()[idx];
      if (!b.isValid())
        return ResultErrored<bool>(
            ResultError("timebucket [" + QVariant(idx).toString() +
                        "] is not valid:" + b.errMessage()));

      const auto row = new Model::Album::TimelineTimeBucketModel(
          b.timeBucketDate, b.count, model);
      connect(row, &Model::Album::TimelineTimeBucketModel::needReveal, model,
              &Model::Album::TimelineAlbumModel::slotOnRowReveal);
      rows.append(row);
    }

    model->setTimeBuckets(rows);

    return ResultErrored<bool>(true);
  });
}

void ModAlbum::updateTimeBucket(Model::Album::TimelineTimeBucketModel *model,
                                const QUuid &albumId) {
  { // TODO check for null model
    QMutexLocker locker(&m_mutexTimebucket);
    //    qWarning() << "add" << model->getDate() << "to queue";
    TimebucketQueueTask task;
    task.model = model;
    if (!albumId.isNull())
      task.albumId = albumId;

    m_timebucketQueue.prepend(task);
  }

  if (m_isRunningTimebucket.load())
    return;

  m_isRunningTimebucket.store(true);

  QMetaObject::invokeMethod(this, "processNextTimebucket",
                            Qt::QueuedConnection);
}

void ModAlbum::processNextTimebucket() {
  QMutexLocker locker(&m_mutexTimebucket);
  if (m_timebucketQueue.isEmpty()) {
    m_isRunningTimebucket.store(0);
    //    qWarning() << "queue is empty";
    return;
  }
  m_isRunningTimebucket.store(1);

  const auto task = m_timebucketQueue.dequeue();
  locker.unlock();
  //  qWarning() << "run" << model->getDate();

  Api::Timeline::ApiTimeline::GetBucketQueryParam params;
  params.order = Api::Models::AssetOrder::DESC;
  if (task.albumId.has_value())
    params.albumId = task.albumId.value();

  const auto qDate =
      QDate(task.model->getDate().first, task.model->getDate().second, 1);

  const auto &respBucket = api.timeline->getBucket(qDate, params);
  if (respBucket.code() == ResultPromiseBase::Failed) {
    qWarning() << respBucket.error().message();
    QMetaObject::invokeMethod(this, "processNextTimebucket",
                              Qt::QueuedConnection);
    return;
  }

  QVector<Model::Album::TimelineDayRow *> newRows;

  const auto bucketData = respBucket.data();
  auto it = bucketData.begin();
  const auto end = bucketData.end();

  const auto mod_assets = qobject_cast<Module::Assets::ModAssets *>(
      ImmichPlugin::assetsSingletonProvider(nullptr,
                                            nullptr)); // TODO to class member

  while (it != end) {
    const QDate rowDate = it->fileCreatedAt.date();
    auto rowBegin = it;
    auto rowEnd = it;

    while (rowEnd != end && rowEnd->fileCreatedAt.date() == rowDate)
      ++rowEnd;

    QVector<Model::Album::AssetWrapper> rowAssets;
    rowAssets.reserve(std::distance(rowBegin, rowEnd));
    for (auto iter = rowBegin; iter != rowEnd; ++iter)
      rowAssets.append(*iter);
    const auto newRowModel =
        new Model::Album::TimelineDayRow(rowDate, rowAssets, task.model);
    connect(newRowModel, &Model::Album::TimelineDayRow::needThumbnail,
            mod_assets, &Module::Assets::ModAssets::slotRequestThumbnail);
    newRows.append(newRowModel);

    it = rowEnd;
  }

  task.model->reveal(newRows);
  QMetaObject::invokeMethod(this, "processNextTimebucket",
                            Qt::QueuedConnection);
  return;
}

Immich::ResultVariantPromise *
ModAlbum::q_updateAlbumModel(Model::Album::TimelineAlbumModel *model,
                             const QUuid &albumId) {
  return new ResultVariantPromise([this, model, albumId]() {
    const auto resp = updateAlbumModel(model, albumId);
    resp->waitForFinished();
    return resp->getResult().toVariant();
  });
}

Immich::ResultPromise<bool> *
ModAlbum::updateAlbumsListModel(Model::Albums::AlbumsListModel *model) {
  return new ResultPromise<bool>([model]() {
    const auto &res = api.albums->getAlbums();

    if (res.code() == ResultPromiseBase::Failed) {
      return ResultErrored<bool>(ResultError(res.error()));
    }

    QVector<Model::Albums::AlbumInfo> albums;
    albums.reserve(res.data().size());
    for (const auto &a : res.data()) {
      albums.append(Model::Albums::AlbumInfo(a));
    }

    model->setAlbums(albums);
    return ResultErrored<bool>(true);
  });
}

Immich::ResultVariantPromise *
ModAlbum::q_updateAlbumsListModel(Model::Albums::AlbumsListModel *model) {
  return new ResultVariantPromise([this, model]() {
    const auto resp = updateAlbumsListModel(model);
    resp->waitForFinished();
    return resp->getResult().toVariant();
  });
}
