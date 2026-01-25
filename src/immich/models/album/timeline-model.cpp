#include "timeline-model.h"
#include <QDebug>
#include <src/immich/immich-plugin.h>

using namespace Immich::Model::Album;

TimelineAlbumModel::TimelineAlbumModel(QObject *parent)
    : QAbstractListModel(parent),
      mod_album(qobject_cast<Module::Album::ModAlbum *>(
          ImmichPlugin::albumsSingletonProvider(nullptr, nullptr))),
      rows(), m_assetCount(0) {}

int TimelineAlbumModel::rowCount(const QModelIndex &) const {
  return rows.size();
}

QVariant TimelineAlbumModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  int rowIdx = index.row();
  if (rowIdx < 0 || rowIdx >= rowCount())
    return QVariant();

  const auto timeBucketPtr = rows[rowIdx];

  switch (role) {
  case TimebucketModelRole:
    return QVariant::fromValue(timeBucketPtr);
  case DateRole:
    return QVariant::fromValue(timeBucketPtr->getDate());
  default:
    return QVariant();
  }
}

void TimelineAlbumModel::setTimeBuckets(
    const QVector<TimelineTimeBucketModel *> &pRows) {
  beginResetModel();
  qDeleteAll(rows);
  rows.clear();
  endResetModel();

  beginInsertRows(QModelIndex(), 0, pRows.size() - 1);
  rows.reserve(pRows.size());
  //  m_assetCount = 0;
  for (const auto tb : pRows) {
    rows.append(tb);
    //    const auto debugDate = tb->getDate();
    //    m_rowsIndex.insert(debugDate, tb);
    m_assetCount += tb->getBucketSize();
  }
  endInsertRows();
}

TimelineTimeBucketModel *
TimelineAlbumModel::getTimebucketModel(const TimebucketDate &pDate) const {
  //  return m_rowsIndex.value(pDate, nullptr); TODO optimization
  for (const auto tb : rows) {
    if (tb->getDate() == pDate)
      return tb;
  }

  return nullptr;
}

int TimelineAlbumModel::assetCount() const { return m_assetCount; }

QHash<int, QByteArray> TimelineAlbumModel::roleNames() const {
  return {{TimebucketModelRole, "timebucketModel"}, {DateRole, "date"}};
}

const QUuid &TimelineAlbumModel::albumId() const { return m_albumId; }

void TimelineAlbumModel::setAlbumId(const QUuid &newAlbumId) {
  if (m_albumId == newAlbumId)
    return;
  m_albumId = newAlbumId;
  emit albumIdChanged();
}

void TimelineAlbumModel::slotOnRowReveal() {
  auto *tb = qobject_cast<TimelineTimeBucketModel *>(sender());

  if (!tb) {
    qWarning() << "bad cast to TimelineTimeBucketModel";
    return;
  }

  if (tb->m_revealStatus) {
    qWarning() << "already loading" << tb->getDate();
    return;
  }

  qInfo() << "reveal timebucket:" << tb->getDate().first
          << tb->getDate().second;

  tb->m_revealStatus = TimelineTimeBucketModel::Started;

  mod_album->updateTimeBucket(tb, m_albumId);
}

TimelineTimeBucketModel::TimelineTimeBucketModel(QObject *parent)
    : QAbstractListModel(parent), m_bucketSize(0), m_date(),
      m_revealStatus(RevealStatus::NotRevealed) {}

TimelineTimeBucketModel::TimelineTimeBucketModel(const QDate &pDate, int pSize,
                                                 QObject *parent)
    : QAbstractListModel(parent), m_bucketSize(pSize),
      m_date(pDate.year(), pDate.month()),
      m_revealStatus(RevealStatus::NotRevealed) {}

int TimelineTimeBucketModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return m_dayRows.size();
}

QVariant TimelineTimeBucketModel::data(const QModelIndex &index,
                                       int role) const {
  if (!index.isValid())
    return QVariant();

  int rowIdx = index.row();
  if (rowIdx < 0 || rowIdx >= rowCount())
    return QVariant();

  auto r = m_dayRows[rowIdx];

  switch (role) {
  case DayRowModelRole:
    return QVariant::fromValue(r);
  case DateRole:
    return r->getDate();
  default:
    return QVariant();
  }
}

QHash<int, QByteArray> TimelineTimeBucketModel::roleNames() const {
  return {{DayRowModelRole, "dayRowModel"}, {DateRole, "date"}};
}

int TimelineTimeBucketModel::getBucketSize() const { return m_bucketSize; }

void TimelineTimeBucketModel::reveal(const QVector<TimelineDayRow *> &pAssets) {
  beginInsertRows(QModelIndex(), 0, pAssets.size() - 1);
  m_dayRows = pAssets;
  endInsertRows();
  m_revealStatus = Complete;
  emit revealStatusChanged();
}

TimebucketDate TimelineTimeBucketModel::getDate() const { return m_date; }

TimelineDayRow::TimelineDayRow(QObject *parent)
    : QAbstractListModel(parent), m_date(), m_assets(0) {}

TimelineDayRow::TimelineDayRow(const QDate &pDate,
                               const QVector<AssetWrapper> &pAssets,
                               QObject *parent)
    : QAbstractListModel(parent),
      m_date(pDate), m_assets(pAssets) {}

int TimelineDayRow::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent)
  return m_assets.size();
}

QVariant TimelineDayRow::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  int rowIdx = index.row();
  if (rowIdx < 0 || rowIdx >= rowCount())
    return QVariant();

  auto r = m_assets[rowIdx];

  switch (role) {
  case AssetId:
    return r.id;

  case ThumbnailPathRole: {
    if (!r.thumbnailPath.isEmpty())
      return r.thumbnailPath;

    emit const_cast<Model::Album::TimelineDayRow *>(this)->needThumbnail(
        r.id, Api::Models::AssetMediaSize::THUMBNAIL);
    return QVariant();
  }
  default:
    return QVariant();
  }
}

QHash<int, QByteArray> TimelineDayRow::roleNames() const {
  return {{AssetId, "assetId"}, {ThumbnailPathRole, "thumbnailPath"}};
}

const QDate &TimelineDayRow::getDate() const { return m_date; }

void TimelineDayRow::slotOnThumbnailUpdate(const QUuid pAssetId,
                                           const QString pPath) {
  for (int idx = 0; idx < m_assets.size(); ++idx) {
    if (m_assets[idx].id == pAssetId) {
      m_assets[idx].thumbnailPath = pPath;
      const QModelIndex modelIndex = index(idx);
      emit dataChanged(modelIndex, modelIndex, {ThumbnailPathRole});
    }
  }
}
