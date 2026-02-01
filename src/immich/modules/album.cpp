#include <src/immich/api/dto/_albums.h>

#include <src/immich/api/api-albums.h>
#include <src/immich/api/api-timeline.h>
#include <src/immich/immich-plugin.h>
#include <src/immich/models/album/timeline-model.h>
#include <src/immich/models/albums/albums-list-model.h>

#include "album.h"
#include <QDebug>
#include <QVector>

using namespace Immich::Module::Album;

ModAlbum::ModAlbum(QObject *parent) : BaseModule(parent) {}

Result::Promise<bool> *
ModAlbum::updateAlbumModel(Model::Album::TimelineAlbumModel *model,
                           const QUuid &albumId) const {
  return new Result::Promise<bool>([model, albumId]() -> Result::Result<bool> {
    Api::Timeline::ApiTimeline::GetBucketsQueryParam params;
    if (!albumId.isNull()) {
      params.albumId = albumId;
    }

    params.order = Api::Models::AssetOrder::DESC;
    const auto &res = api.timeline->getBuckets(params);

    if (!res.has_value()) {
      return tl::make_unexpected(res.error());
    }

    QVector<Model::Album::TimelineTimeBucketModel *> rows;
    rows.reserve(res.value().size());
    for (int idx = 0; idx < res.value().size(); ++idx) {
      const auto &b = res.value()[idx];
      if (!b.isValid())
        return tl::make_unexpected(Result::Error("timebucket [" + QVariant(idx).toString() +
                              "] is not valid:" + b.errMessage()));

      const auto row = new Model::Album::TimelineTimeBucketModel(
          b.timeBucketDate, b.count, model);
      connect(row, &Model::Album::TimelineTimeBucketModel::needReveal, model,
              &Model::Album::TimelineAlbumModel::slotOnRowReveal);
      rows.append(row);
    }

    model->setTimeBuckets(rows);

    return {true};
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
  if (!respBucket.has_value()) {
    qWarning() << respBucket.error().message();
    QMetaObject::invokeMethod(this, "processNextTimebucket",
                              Qt::QueuedConnection);
    return;
  }

  QVector<Model::Album::TimelineDayRow *> newRows;

  const auto bucketData = respBucket.value();
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

Result::PromiseVariant *
ModAlbum::q_updateAlbumModel(Model::Album::TimelineAlbumModel *model,
                             const QUuid &albumId) {
  return new Result::PromiseVariant(updateAlbumModel(model, albumId));
}

Result::Promise<bool> *
ModAlbum::updateAlbumsListModel(Model::Albums::AlbumsListModel *model) {
  return new Result::Promise<bool>([model]() -> Result::Result<bool> {
    const auto &res = api.albums->getAlbums();

    if (!res.has_value())
      return tl::make_unexpected(Result::Error(res.error()));

    QVector<Model::Albums::AlbumInfo> albums;
    albums.reserve(res.value().size());
    for (const auto &a : res.value()) {
      albums.append(Model::Albums::AlbumInfo(a));
    }

    model->setAlbums(albums);
    return {true};
  });
}

Result::PromiseVariant *
ModAlbum::q_updateAlbumsListModel(Model::Albums::AlbumsListModel *model) {
  return new Result::PromiseVariant(updateAlbumsListModel(model));
}
