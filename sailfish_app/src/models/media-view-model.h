#ifndef MEDIAVIEWMODEL_H
#define MEDIAVIEWMODEL_H

#include <QAbstractListModel>
#include <src/models/timeline-model.h>

namespace Immich {
namespace Model {

class MediaViewModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(TimelineModel *timeline READ timeline WRITE setTimeline NOTIFY
                 timelineChanged)
  Q_PROPERTY(int currentGlobalIndex READ currentGlobalIndex WRITE
                 setCurrentIndex NOTIFY currentIndexChanged)
public:
  explicit MediaViewModel(QObject *parent = nullptr);

  enum Roles { AssetId = Qt::UserRole + 1, ThumbnailPath, AssetObjectRole };
  Q_ENUM(Roles)

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  TimelineModel *timeline() const;
  void setTimeline(TimelineModel *newTimeline);

  int currentGlobalIndex() const;
  void setCurrentIndex(int newCurrentIndex);

  Q_INVOKABLE void setCurrentIndexByLocalIndex(int pBucketIdx, int pAssetIdx);

signals:
  void timelineChanged();
  void currentIndexChanged();

public slots:
  void updateThumbnail(const QString &pAssetId, const QString &pPath);

  void slotRowsRemoved(const QModelIndex &, int first, int last);

protected:
  QHash<int, QByteArray> roleNames() const override;

protected slots:
  void slotRowsInserted(const QModelIndex &, int first, int last);
  void slotDataChanged(const QModelIndex &topLeft,
                       const QModelIndex &bottomRight,
                       const QVector<int> &roles);

private:
  TimelineModel *m_timeline = nullptr;
  int m_currentGlobalIndex;
  QPair<int, int> getLocalIndexByGlobalIndex(int pIdx) const;
  int getGlobalIndexByLocalIndex(int pBucketIdx, int pAssetIdx);
  int getGlobalIndexByLocalIndex(QPair<int, int> pLocalIdx);
  int getGlobalIndexByAssetId(const QString &pAssetId);

  Immich::Thumbnailer *m_thumbnailer;
//  QHash<QString, QString> m_thumbnailsCache;
};

} // namespace Model
} // namespace Immich
#endif // MEDIAVIEWMODEL_H
