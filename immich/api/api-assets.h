#ifndef APIASSETS_H
#define APIASSETS_H

#include "_base-api.h"
#include "dto/_dto.h"
#include <tl-optional.h>
#include <immich/result/result.h>

namespace Immich {
namespace Api {
namespace Assets {

class ApiAssets : public Base {
  Q_OBJECT
public:
  ApiAssets(QObject *parent = nullptr);

  struct ThumbnailQueryParams {
    tl::optional<QString> key;
    tl::optional<QString> slug;
    Models::AssetMediaSize size;
  };

  ResultErrored<QByteArray> thumbnail(const QUuid &id, const ThumbnailQueryParams& params = {}) const;
};
} // namespace Assets
} // namespace Api

} // namespace Immich
#endif // APIASSETS_H
