#include "timeline-model.h"
#include <algorithm>
using namespace Immich::Model;

TimelineModel::TimelineModel(QObject *parent) : QAbstractListModel(parent) {}

int TimelineModel::rowCount(const QModelIndex &) const {
  return m_buckets.size();
}

QVariant TimelineModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  int rowIdx = index.row();
  if (rowIdx < 0 || rowIdx >= rowCount())
    return QVariant();

  auto b = m_buckets[rowIdx];

  switch (role) {
  case Date:
    return b->date();
  case BucketModel:
    return QVariant::fromValue(b);
  case RevealStatus:
    return b->status();
  default:
    return QVariant();
  }
}

int TimelineModel::assetsCount() const {
  int assetCount = 0;
  for (const auto tb : m_buckets)
    assetCount += tb->rowCount();

  return assetCount;
}

QList<QDate> TimelineModel::getDateRange() const {
  QList<QDate> range;

  if (m_buckets.isEmpty())
    return range;

  range.append(m_buckets[0]->date());

  if (m_buckets.size() > 1) {
    range.append(m_buckets[m_buckets.size() - 1]->date());
  }

  return range;
}

QHash<int, QByteArray> TimelineModel::roleNames() const {
  return {
      {Date, "date"},
      {BucketModel, "bucketModel"},
      {RevealStatus, "revealStatus"},
  };
}

bool TimelineModel::isInitialized() const { return m_isInitialized; }

void TimelineModel::setIsInitialized(bool newIsInitialized) {
  if (m_isInitialized == newIsInitialized || m_isInitialized == true)
    return;

  m_isInitialized = newIsInitialized;
  emit isInitializedChanged();
}

void TimelineModel::slotEmptyInitialize() { setIsInitialized(true); }

void TimelineModel::slotAppendDataBatch(QVector<TimebucketModel *> pBuckets) {
  if (pBuckets.isEmpty()) {
    setIsInitialized(true);
    return;
  }

  std::sort(pBuckets.begin(), pBuckets.end(),
            [](TimebucketModel *a, TimebucketModel *b) {
              return a->date() < b->date();
            });

    setIsInitialized(true);

  for (TimebucketModel *bucket : pBuckets) {
    bucket->setParent(this);
    QDate newDate = bucket->date();

    connect(bucket, &TimebucketModel::needReveal, this,
            &TimelineModel::slotBucketNeedReveal);

    auto it = std::lower_bound(m_buckets.begin(), m_buckets.end(), newDate,
                               [](TimebucketModel *&bucket, const QDate &date) {
                                 return bucket->date() > date;
                               });

    int row = std::distance(m_buckets.begin(), it);

    beginInsertRows(QModelIndex(), row, row);
    m_buckets.insert(it, bucket);
    endInsertRows();
  }
}

void TimelineModel::slotBucketNeedReveal() {
  const auto bucket = qobject_cast<TimebucketModel *>(sender());

  if (bucket == nullptr)
    return;

  if (bucket->status() != TimebucketModel::RevealStatusNotRevealed) {
    return;
  }

  bucket->setStatus(
      TimebucketModel::RevealStatusLoading); // TODO work with failed fetch
  emit needReveal(bucket->date());
}

void TimelineModel::slotRevealTimebucket(QDate pDate,
                                         QVector<TimebucketModel *> pBuckets) {
  if (pBuckets.isEmpty())
    return; // TODO warning

  int bucketIdx = -1;
  for (int idx = 0; idx < m_buckets.size(); ++idx) {
    if (m_buckets[idx]->date() == pDate) {
      if (m_buckets[idx]->status() == TimebucketModel::RevealStatusRevealed) {
        for (auto b : pBuckets)
          b->deleteLater();
        return;
      }

      bucketIdx = idx;
      break;
    }
  }

  if (bucketIdx < 0) {
    for (auto b : pBuckets)
      b->deleteLater();
    return;
  }

  int firstRow = bucketIdx;
  int lastRow = bucketIdx + pBuckets.size() - 1;

  beginRemoveRows(QModelIndex(), bucketIdx, bucketIdx);
  auto old = m_buckets.takeAt(bucketIdx);
  endRemoveRows();
  old->deleteLater();

  beginInsertRows(QModelIndex(), firstRow, lastRow);
  for (int i = 0; i < pBuckets.size(); ++i) {
    pBuckets[i]->setParent(this);
    m_buckets.insert(firstRow + i, pBuckets[i]);
  }
  endInsertRows();
}
