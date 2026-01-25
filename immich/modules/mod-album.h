#ifndef MODALBUM_H
#define MODALBUM_H

#include "_base-module.h"
#include "tl-optional.h"
#include <QAtomicInt>
#include <QMutex>
#include <immich/result/result.h>

namespace Immich {
namespace Model {
namespace Albums {
class AlbumsListModel;
}
namespace Album {
class TimelineAlbumModel;
class TimelineTimeBucketModel;
} // namespace Album
} // namespace Model
namespace Module {
namespace Album {

struct TimebucketQueueTask {
  Model::Album::TimelineTimeBucketModel *model;
  tl::optional<QUuid> albumId;
};

class ModAlbum : public BaseModule {
  Q_OBJECT
public:
  explicit ModAlbum(QObject *parent = nullptr);

  ResultPromise<bool> *updateAlbumModel(Model::Album::TimelineAlbumModel *model,
                                        const QUuid &albumId = QUuid()) const;
  void updateTimeBucket(Immich::Model::Album::TimelineTimeBucketModel *model,
                        const QUuid &albumId = QUuid());

  Q_INVOKABLE ResultVariantPromise *
  q_updateAlbumModel(Immich::Model::Album::TimelineAlbumModel *model,
                     const QUuid &albumId = QUuid());

  ResultPromise<bool> *
  updateAlbumsListModel(Model::Albums::AlbumsListModel *model);

  Q_INVOKABLE ResultVariantPromise *
  q_updateAlbumsListModel(Model::Albums::AlbumsListModel *model);

private:
  QQueue<TimebucketQueueTask> m_timebucketQueue;
  QAtomicInt m_isRunningTimebucket = 0;
  QMutex m_mutexTimebucket;

  Q_INVOKABLE void processNextTimebucket();
};
} // namespace Album
} // namespace Module
} // namespace Immich
Q_DECLARE_METATYPE(Immich::Module::Album::ModAlbum *)
#endif // MODALBUM_H
