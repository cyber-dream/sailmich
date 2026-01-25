#ifndef TIMELINEALBUMMODEL_H
#define TIMELINEALBUMMODEL_H

#include <QAbstractListModel>

#include <QPair>
#include <src/immich/api/dto/_timeline.h>
#include <src/immich/modules/album.h>
#include <src/immich/modules/assets.h>

namespace Immich {
namespace Model {
namespace Album {
struct AssetWrapper : public Api::Timeline::TimeBucketAsset {
public:
  AssetWrapper() = default;
  AssetWrapper(const Api::Timeline::TimeBucketAsset &pAsset)
      : Api::Timeline::TimeBucketAsset(pAsset) {};
  QString thumbnailPath = "";
};

class TimelineDayRow : public QAbstractListModel {
  Q_OBJECT
public:
  friend class PathViewModel;
  friend class TimelineAlbumModel;
  TimelineDayRow(QObject *parent = nullptr);
  TimelineDayRow(const QDate &pDate, const QVector<AssetWrapper> &pAssets,
                 QObject *parent = nullptr);
  enum Roles { AssetId = Qt::UserRole + 1, ThumbnailPathRole };
  Q_ENUM(Roles)

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  const QDate &getDate() const;
signals:
  void needThumbnail(QUuid assetId, Api::Models::AssetMediaSize size,
                     bool isSkipQueue = false);
public slots:
  void slotOnThumbnailUpdate(const QUuid assetId, const QString pPath);

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  //  Immich::Module::Assets::ModAssets *mod_assets;

  const QDate m_date;
  QVector<AssetWrapper> m_assets;

  Q_PROPERTY(QDate getDate READ getDate CONSTANT)
};

struct TimebucketDate : public QPair<int, int> {
  Q_GADGET
  Q_PROPERTY(int year READ getYear)
  Q_PROPERTY(int month READ getMonth)

public:
  TimebucketDate() : QPair(0, 0) {}
  TimebucketDate(const QPair<int, int> &other) : QPair(other) {}
  TimebucketDate(int y, int m) : QPair(y, m) {}

  quint16 getYear() const { return first; }
  quint8 getMonth() const { return second; }
};

class TimelineTimeBucketModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(TimebucketDate date READ getDate CONSTANT)
  Q_PROPERTY(
      RevealStatus revealStatus READ revealStatus NOTIFY revealStatusChanged)

  Q_PROPERTY(int bucketSize READ getBucketSize CONSTANT)
public:
  friend class TimelineAlbumModel;
  TimelineTimeBucketModel(QObject *parent = nullptr);
  TimelineTimeBucketModel(const QDate &pDate, int pSize,
                          QObject *parent = nullptr);
  enum Roles { DayRowModelRole = Qt::UserRole + 1, DateRole };
  Q_ENUM(Roles)

  enum RevealStatus { NotRevealed, Started, Complete };
  Q_ENUM(RevealStatus)

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  TimebucketDate getDate() const;

  RevealStatus revealStatus() const { return m_revealStatus; }

  void reveal(const QVector<TimelineDayRow *> &pAssets);
  int getBucketSize() const;

  QVector<TimelineDayRow *>::const_iterator begin() const {
    return m_dayRows.begin();
  }
  QVector<TimelineDayRow *>::const_iterator end() const {
    return m_dayRows.end();
  }
signals:
  void needReveal();
  void revealStatusChanged();

protected:
  QHash<int, QByteArray> roleNames() const override;

private:
  const int m_bucketSize;
  const TimebucketDate m_date;
  RevealStatus m_revealStatus;
  QVector<TimelineDayRow *> m_dayRows;
};

class TimelineAlbumModel : public QAbstractListModel {
  Q_OBJECT
  Q_PROPERTY(QUuid albumId READ albumId WRITE setAlbumId NOTIFY albumIdChanged)
public:
  enum Roles { TimebucketModelRole = Qt::UserRole + 1, DateRole };
  Q_ENUM(Roles)

  //  friend class PathViewModel;

  TimelineAlbumModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  void setTimeBuckets(const QVector<TimelineTimeBucketModel *> &pRows);

  TimelineTimeBucketModel *
  getTimebucketModel(const TimebucketDate &pDate) const;

  int assetCount() const;

  QVector<TimelineTimeBucketModel *>::const_iterator begin() const {
    return rows.begin();
  }
  QVector<TimelineTimeBucketModel *>::const_iterator end() const {
    return rows.end();
  }

  const QUuid &albumId() const;
  void setAlbumId(const QUuid &newAlbumId);

signals:
  void albumIdChanged();

public slots:
  void slotOnRowReveal();

protected:
  QHash<int, QByteArray> roleNames() const;

private:
  QUuid m_albumId;
  Immich::Module::Album::ModAlbum *mod_album;
  QVector<TimelineTimeBucketModel *> rows;

  // Cache
  //  QHash<TimebucketDate, TimelineTimeBucketModel *> m_rowsIndex;
  int m_assetCount;
};

} // namespace Album
} // namespace Model
} // namespace Immich
Q_DECLARE_METATYPE(Immich::Model::Album::TimebucketDate)
#endif // TIMELINEALBUMMODEL_H
