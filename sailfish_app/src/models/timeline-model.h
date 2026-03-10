#ifndef TIMELINEMODEL_H
#define TIMELINEMODEL_H

#include <QAbstractListModel>
#include <QDate>
#include <src/models/timebucket-model.h>

namespace Immich {
namespace Model {
class MediaViewModel;

class TimelineModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(bool isInitialized READ isInitialized NOTIFY isInitializedChanged)
public:
  friend class MediaViewModel;
  TimelineModel(QObject *parent = nullptr);

  enum Roles { Date = Qt::UserRole + 1, BucketModel, RevealStatus };
  Q_ENUM(Roles)

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  Q_INVOKABLE int assetsCount() const;
  Q_INVOKABLE QList<QDate> getDateRange() const;

  bool isInitialized() const;
  void setIsInitialized(bool newIsInitialized);

signals:
  void needReveal(const QDate &pDate, const QUuid &pAlbumId = QUuid());

  void albumIdChanged();

  void isInitializedChanged();

public slots:
  void slotEmptyInitialize();
  void slotAppendDataBatch(QVector<Immich::Model::TimebucketModel *> pBuckets);
  void
  slotRevealTimebucket(QDate pDate,
                       QVector<Immich::Model::TimebucketModel *> pBucketModels);
  void slotBucketNeedReveal();

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  QVector<TimebucketModel *> m_buckets;
  bool m_isInitialized = false;

};
} // namespace Model
} // namespace Immich
Q_DECLARE_METATYPE(Immich::Model::TimelineModel *)
#endif // TIMELINEMODEL_H
