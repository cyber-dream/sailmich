#ifndef MOD_ASSETS_H
#define MOD_ASSETS_H

#include "_base-module.h"
#include <QUuid>
#include <immich/api/dto/_dto.h>

namespace Immich {

namespace Model {
namespace Album {
class TimelineDayRow;
}
} // namespace Model
namespace Module {
namespace Assets {

// TODO tries counter
struct ThumbnailTask {
  QObject *model;
  QUuid assetId;
  Api::Models::AssetMediaSize size;
};

class ModAssets : public BaseModule {
  Q_OBJECT
public:
  ModAssets(QObject *parent = nullptr);
  ModAssets(const QString &thumbnailsPath, QObject *parent = nullptr);

  //  ResultPromise<QString> *
  //  thumbnail(const QUuid &assetId, const Api::Models::AssetMediaSize size)
  //  const; Q_INVOKABLE ResultVariantPromise * q_thumbnail(const QUuid
  //  &assetId,
  //              const Api::Models::AssetMediaSize size) const;

  void requestThumbnail(QObject *model, const QUuid &assetId,
                        const Api::Models::AssetMediaSize size,
                        bool isSkipQueue = false);
public slots:
  void slotRequestThumbnail(const QUuid &assetId,
                            const Api::Models::AssetMediaSize size,
                            bool isSkipQueue = false);

private:
  const QString thumbnailsPath;

  QQueue<ThumbnailTask> m_timebucketQueue;
  QAtomicInt m_isRunningTimebucket = 0;
  QMutex m_mutexTimebucket;
  Q_INVOKABLE void processNextThumbnail();
};
} // namespace Assets
} // namespace Module
} // namespace Immich
#endif // MOD-ASSETS_H
