#include "pathview-model.h"

#include <immich/immich-plugin.h>

using namespace Immich::Model::Album;

PathViewModel::PathViewModel(QObject *parent)
    : QAbstractListModel(parent),
      mod_assets(qobject_cast<Module::Assets::ModAssets *>(
          ImmichPlugin::assetsSingletonProvider(nullptr, nullptr))),
      timelineModel(nullptr) {

  connect(this, &PathViewModel::needThumbnail, mod_assets,
          &Module::Assets::ModAssets::slotRequestThumbnail);
}

int PathViewModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  if (!timelineModel)
    return 0;

  return timelineModel->assetCount();
}

QVariant PathViewModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    qWarning() << "index invalid";
    return QVariant();
  }

  if (!timelineModel) {
    qWarning() << "timelineModel pointer is null";
    return QVariant();
  }

  int assetGlobalIdx = index.row();
  if (assetGlobalIdx < 0 || assetGlobalIdx >= rowCount()) {
    qWarning() << "assetGlobalIdx out of bounds:" << assetGlobalIdx;
    return QVariant();
  }

  const auto ssIndex = getSlideshowIndexByGlobalIndex(assetGlobalIdx);
  const auto tb = timelineModel->getTimebucketModel(ssIndex.tbDate);
  if (!tb) {
    qWarning() << "timebucket not found by ssIndex";
    return QVariant();
  };

  auto asset = Api::Timeline::TimeBucketAsset();

  auto counter = ssIndex.offset;
  for (const auto dr : *tb) {
    const auto drSize = dr->rowCount();
    if (counter >= drSize) {
      counter -= drSize;
      continue;
    }

    asset = dr->m_assets[counter];
    break;
  }

  if (!asset.isValid()) {
    qWarning() << "asset not found by ssIndex";
    return QVariant();
  }

  switch (role) {
  case PreviewPathRole: {
    if (m_previewPathes.contains(asset.id))
      return m_previewPathes.value(asset.id);

    emit const_cast<Model::Album::PathViewModel *>(this)->needThumbnail(
        asset.id, Api::Models::AssetMediaSize::PREVIEW, true);

    return QVariant();
  }
  case AssetIdRole:
    return asset.id;
    // case FileCreatedAtRole:w
    //   return r.fileCreatedAt;
  case AssetObjectRole:
    return QVariant::fromValue(asset);
  default:
    return QVariant();
  }
}

TimelineAlbumModel *PathViewModel::getTimelineModel() const {
  return timelineModel;
}

void PathViewModel::setTimelineModel(TimelineAlbumModel *pModel) {
  if (timelineModel == pModel)
    return;

  timelineModel = pModel;

  emit timelineModelChanged();
}

int PathViewModel::getCurrentGlobalIndex() const {
  int counter = 0;

  for (const auto &tb : *timelineModel) {
    if (tb->getDate().first != m_currentIndex.tbDate.first ||
        tb->getDate().second != m_currentIndex.tbDate.second) {
      counter += tb->getBucketSize();
      continue;
    }

    counter += m_currentIndex.offset;
    return counter;
  }

  qWarning() << "global index not found";
  return 0;
}

void PathViewModel::setCurrentGlobalIndex(int pIdx) {
  setCurrentIndex(getSlideshowIndexByGlobalIndex(pIdx));
}

void PathViewModel::setCurrentIndex(const TimebucketDate &pTimebucketDate,
                                    int pAssetIndex) {
  SlideShowIndex newIndex;
  newIndex.tbDate = pTimebucketDate;
  newIndex.offset = static_cast<quint32>(pAssetIndex);

  setCurrentIndex(newIndex);
}

void PathViewModel::setCurrentIndex(const TimebucketDate &pTimebucketDate,
                                    int pDayRowOffset, int pLocalAssetIndex) {
  SlideShowIndex newIndex;
  newIndex.tbDate = pTimebucketDate;
  newIndex.offset = 0;
  for (const auto tb : *timelineModel) {
    if (pTimebucketDate != tb->getDate())
      continue;

    int counter = 0;
    for (auto it = tb->begin(); it != tb->end() && counter < pDayRowOffset;
         ++it, ++counter) {
      newIndex.offset += (*it)->rowCount();
    }

    newIndex.offset += pLocalAssetIndex;
    break;
  }

  setCurrentIndex(newIndex);
}

void PathViewModel::setCurrentIndex(const SlideShowIndex &pIndex) {
  if (m_currentIndex == pIndex) {
    return;
  }

  m_currentIndex = pIndex;
  emit currentGlobalIndexChanged();
}

QHash<int, QByteArray> PathViewModel::roleNames() const {
  return {{PreviewPathRole, "previewPath"},
          {AssetIdRole, "assetId"},
          //{FileCreatedAtRole, "fileCreatedAt"},
          {AssetObjectRole, "assetDataObject"}};
}

SlideShowIndex PathViewModel::getSlideshowIndexByGlobalIndex(int pIdx) const {
  SlideShowIndex newIndex;

  for (const auto tb : *timelineModel) {
    int tbSize = tb->getBucketSize();
    if (pIdx >= tbSize) {
      pIdx -= tbSize;
      continue;
    }

    if (tb->revealStatus() ==
        TimelineTimeBucketModel::RevealStatus::NotRevealed)
      emit tb->needReveal();

    newIndex.tbDate = tb->getDate();
    newIndex.offset = pIdx;
    break;
  }

  return newIndex; // TODO return error
}

void PathViewModel::slotOnThumbnailUpdate(const QUuid pAssetId,
                                          const QString pPath) {

//  if (m_previewPathes.size() > 1000) // TODO more cool memory control
//    m_previewPathes.clear();

  m_previewPathes.insert(pAssetId, pPath);

  int globalIndex = 0;
  for (const auto tb : *timelineModel) {
    for (const auto dayRow : *tb) {
      for (const auto &asset : dayRow->m_assets) {
        if (asset.id == pAssetId) {
          const auto modelIndex = index(globalIndex);
          emit dataChanged(modelIndex, modelIndex, {PreviewPathRole});
          return;
        }
        globalIndex++;
      }
    }
  }

  qWarning() << "asset" << pAssetId << "not found in timeline";
}

bool SlideShowIndex::operator==(const SlideShowIndex &other) const {
  return this->tbDate == other.tbDate && this->offset == other.offset;
}

bool SlideShowIndex::operator!=(const SlideShowIndex &other) const {
  return !(*this == other);
}

SlideShowIndex::SlideShowIndex() : tbDate(), offset(0) {}
