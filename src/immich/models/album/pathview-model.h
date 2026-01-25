#ifndef PATHVIEWMODEL_H
#define PATHVIEWMODEL_H

#include "timeline-model.h"

#include <QAbstractListModel>
#include <QMap>

namespace Immich {
namespace Model {
namespace Album {

struct SlideShowIndex {
public:
  bool operator==(const SlideShowIndex &other) const;
  bool operator!=(const SlideShowIndex &other) const;
  SlideShowIndex();
  TimebucketDate tbDate;
  int offset;
};

class PathViewModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(TimelineAlbumModel *timelineModel READ getTimelineModel WRITE
                 setTimelineModel NOTIFY timelineModelChanged)
  Q_PROPERTY(int currentGlobalIndex READ getCurrentGlobalIndex WRITE
                 setCurrentGlobalIndex NOTIFY currentGlobalIndexChanged)
public:
  PathViewModel(QObject *parent = nullptr);

  enum Roles {
    PreviewPathRole = Qt::UserRole + 1,
    AssetIdRole,
    //      FileCreatedAtRole
    AssetObjectRole
  };
  Q_ENUM(Roles)

  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  TimelineAlbumModel *getTimelineModel() const;
  void setTimelineModel(TimelineAlbumModel *pModel);

  int getCurrentGlobalIndex() const;
  void setCurrentGlobalIndex(int pIdx);

  // TODO checks for invalid new index
  Q_INVOKABLE void setCurrentIndex(const TimebucketDate &pTimebucketDate,
                                   int pAssetIndex);
  // TODO checks for invalid new index
  Q_INVOKABLE void setCurrentIndex(const TimebucketDate &pTimebucketDate,
                                   int pDayRowOffset, int pLocalAssetIndex);
  // TODO checks for invalid new index
  Q_INVOKABLE void setCurrentIndex(const SlideShowIndex &pIndex);

signals:
  void needThumbnail(QUuid assetId, Api::Models::AssetMediaSize size,
                     bool isSkipQueue = false);
  void timelineModelChanged();
  void currentGlobalIndexChanged();
public slots:
  void slotOnThumbnailUpdate(const QUuid pAssetId, const QString pPath);

protected:
  QHash<int, QByteArray> roleNames() const;

private:
  Immich::Module::Assets::ModAssets *mod_assets;
  TimelineAlbumModel *timelineModel;
  //  QMap<QUuid, const Api::Timeline::TimeBucketAsset*> m_flatAssets;
    QHash<QUuid, QString> m_previewPathes; //TODO optimize, now its grow all time

  SlideShowIndex m_currentIndex;

  //  int m_currentGlobalIndex;

  inline SlideShowIndex getSlideshowIndexByGlobalIndex(int pIndex) const;
};

} // namespace Album
} // namespace Model
} // namespace Immich

#endif // PATHVIEWMODEL_H
