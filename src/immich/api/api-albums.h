#ifndef APIALBUMS_H
#define APIALBUMS_H

#include "_base-api.h"
#include <src/result/result.h>

namespace Immich {
namespace Api {
namespace Albums {
struct AlbumResponseDto;
class ApiAlbums : public Base {
  Q_OBJECT
public:
  explicit ApiAlbums(QObject *parent = nullptr);

  Result::Result<QVector<AlbumResponseDto>> getAlbums( bool isShared = true, const QUuid &assetId = QUuid()) const;
signals:
};
} // namespace Albums
} // namespace Api
} // namespace Immich

#endif // APIALBUMS_H
