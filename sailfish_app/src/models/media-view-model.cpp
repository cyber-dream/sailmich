#include "media-view-model.h"
#include "dependencypointer.h"
#include <QPair>
#include <src/models/timeline-model.h>
// TODO crash when swiping from 0 to last without loaded timeline (can't
// replay). Just move to new timeline algorithm
using namespace Immich::Model;

MediaViewModel::MediaViewModel(QObject *parent)
    : QAbstractListModel(parent),
      m_thumbnailer(QInjection::Pointer<Immich::Thumbnailer>()) {}

int MediaViewModel::rowCount(const QModelIndex &) const {
  if (m_timeline == nullptr)
    return 0;

  return m_timeline->assetsCount();
}

QVariant MediaViewModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (!m_timeline)
    return QVariant();

  int rowIdx = index.row();
  if (rowIdx < 0 || rowIdx >= rowCount())
    return QVariant();

  const auto locIdx = getLocalIndexByGlobalIndex(rowIdx);

  if (locIdx.first < 0 || locIdx.first >= m_timeline->m_buckets.size())
    return QVariant();

  auto *bucket = m_timeline->m_buckets[locIdx.first];

  if (locIdx.second < 0 || locIdx.second >= bucket->m_assets.size())
    return QVariant();

  if (bucket->m_status == TimebucketModel::RevealStatusNotRevealed) {
    emit bucket->needReveal();
    return QVariant();
  } else if (bucket->m_status == TimebucketModel::RevealStatusLoading)
    return QVariant();

  const auto asset =
      m_timeline->m_buckets[locIdx.first]->m_assets[locIdx.second];

  if (asset == nullptr) {
    return QVariant();
  }

  switch (role) {
  case AssetId:
    return asset->m_id;
  case ThumbnailPath: {
    if (asset->m_isPreviewExists)
      return asset->m_previewPath;

    const auto watcher = m_thumbnailer->getPreviewSize(asset->m_id);
    if (!watcher->path().isEmpty()) {
      QMetaObject::invokeMethod(const_cast<MediaViewModel *>(this),
                                "updateThumbnail", Qt::QueuedConnection,
                                Q_ARG(QString, asset->m_id),
                                Q_ARG(QString, watcher->path()));

      watcher->deleteLater();
    } else {
      connect(watcher, &ThumbnailWatcher::finished, [this, asset, watcher]() {
        QMetaObject::invokeMethod(const_cast<MediaViewModel *>(this),
                                  "updateThumbnail", Qt::QueuedConnection,
                                  Q_ARG(QString, asset->m_id),
                                  Q_ARG(QString, watcher->path()));

        watcher->deleteLater();
      });
    }

    if (asset->m_isThumbnailExists)
      return asset->m_thumbnailPath;

    return "";
  }
  case AssetObjectRole:
    return QVariant::fromValue(asset);
  default:
    return QVariant();
  }
}

Immich::Model::TimelineModel *MediaViewModel::timeline() const {
  return m_timeline;
}

void MediaViewModel::setTimeline(TimelineModel *newTimeline) {
  if (m_timeline == newTimeline)
    return;

  if (m_timeline != nullptr) {
    m_timeline->disconnect(this);
  }

  m_timeline = newTimeline;

  if (m_timeline) {
    connect(m_timeline, &QAbstractListModel::rowsInserted, this,
            &MediaViewModel::slotRowsInserted);

    connect(m_timeline, &QAbstractListModel::rowsAboutToBeRemoved, this,
            &MediaViewModel::slotRowsRemoved);

    connect(m_timeline, &QAbstractListModel::dataChanged, this,
            &MediaViewModel::slotDataChanged);
  }
  emit timelineChanged();
}

void MediaViewModel::slotRowsRemoved(const QModelIndex &, int first, int last) {
  beginRemoveRows(QModelIndex(), first, last);
  endRemoveRows();
}

void MediaViewModel::slotRowsInserted(const QModelIndex &, int first,
                                      int last) {
  beginInsertRows(QModelIndex(), first, last);
  endInsertRows();
}

void MediaViewModel::slotDataChanged(const QModelIndex &topLeft,
                                     const QModelIndex &bottomRight,
                                     const QVector<int> &roles) {
  emit dataChanged(index(topLeft.row()), index(bottomRight.row()), roles);
}

int MediaViewModel::currentGlobalIndex() const { return m_currentGlobalIndex; }

void MediaViewModel::setCurrentIndex(int newCurrentIndex) {
  if (m_currentGlobalIndex == newCurrentIndex)
    return;
  m_currentGlobalIndex = newCurrentIndex;
  emit currentIndexChanged();
}

void MediaViewModel::setCurrentIndexByLocalIndex(int pBucketIdx,
                                                 int pAssetIdx) {
  setCurrentIndex(getGlobalIndexByLocalIndex(pBucketIdx, pAssetIdx));
}

void MediaViewModel::updateThumbnail(const QString &pAssetId,
                                     const QString &pPath) {

  if (m_timeline == nullptr)
    return;
  const int globIdx = getGlobalIndexByAssetId(pAssetId);
  if (globIdx < 0)
    return;

  const auto locIdx = getLocalIndexByGlobalIndex(globIdx);

  if (locIdx.first < 0 || locIdx.first >= m_timeline->m_buckets.size())
    return;

  const auto bucket = m_timeline->m_buckets[locIdx.first];
  if (bucket == nullptr) {
    return;
  }

  if (locIdx.second < 0 || locIdx.second >= bucket->m_assets.size())
    return;

  const auto asset = bucket->m_assets[locIdx.second];
  if (!asset)
    return;

  asset->m_isPreviewExists = true;
  asset->m_previewPath = pPath;

  QModelIndex idx = index(globIdx);
  emit dataChanged(idx, idx, {ThumbnailPath});
}

QHash<int, QByteArray> MediaViewModel::roleNames() const {
  return {{AssetId, "assetId"},
          {ThumbnailPath, "thumbnailPath"},
          {AssetObjectRole, "assetObject"}};
}

QPair<int, int> MediaViewModel::getLocalIndexByGlobalIndex(int pIdx) const {
  QPair<int, int> newIndex = {-1, -1};

  if (!m_timeline) {
    return newIndex;
  }

  for (int idx = 0; idx < m_timeline->rowCount(); ++idx) {
    const auto tb = m_timeline->m_buckets[idx];

    int tbSize = tb->rowCount();
    if (pIdx >= tbSize) {
      pIdx -= tbSize;
      continue;
    }

    if (tb->status() == TimebucketModel::RevealStatus::RevealStatusNotRevealed)
      emit tb->needReveal(); // TODO will it work without revealing and what to
                             // show while revealing?

    newIndex.first = idx;
    newIndex.second = pIdx;
    break;
  }

  return newIndex;
}

int MediaViewModel::getGlobalIndexByLocalIndex(QPair<int, int> pLocalIdx) {
  return getGlobalIndexByLocalIndex(pLocalIdx.first, pLocalIdx.second);
}

int MediaViewModel::getGlobalIndexByLocalIndex(int pBucketIdx, int pAssetIdx) {
  if (!m_timeline)
    return 0;

  int globalIdx = 0;

  for (int idx = 0; idx < pBucketIdx; ++idx)
    globalIdx += m_timeline->m_buckets[idx]->rowCount();

  globalIdx += pAssetIdx;

  return globalIdx;
}

int MediaViewModel::getGlobalIndexByAssetId(const QString &pAssetId) {
  if (!m_timeline || pAssetId.isNull())
    return -1;

  int globalIdx = 0;

  for (int bIdx = 0; bIdx < m_timeline->m_buckets.size(); ++bIdx) {
    auto *bucket = m_timeline->m_buckets[bIdx];

    int assetIdxInBucket = -1;
    for (int idx = 0; idx < bucket->m_assets.size(); ++idx) {
      const auto asset = bucket->m_assets[idx];
      if (asset == nullptr)
        continue;

      if (asset->m_id == pAssetId) {
        assetIdxInBucket = idx;
        break;
      }
    }

    if (assetIdxInBucket != -1) {
      return globalIdx + assetIdxInBucket;
    }

    globalIdx += bucket->rowCount();
  }

  return -1;
}
