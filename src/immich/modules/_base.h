#ifndef _BASE_MODULE_H
#define _BASE_MODULE_H

#include <QObject>

namespace Immich {
namespace Api {
namespace Authentication {
class AuthenticationApi;
}
namespace Ping {
class PingApi;
}
namespace Timeline {
class ApiTimeline;
}
namespace Assets {
class ApiAssets;
}
namespace Albums {
class ApiAlbums;
}
} // namespace Api
class BaseModule : public QObject {
public:
  struct ApiStruct {
    Api::Authentication::AuthenticationApi *authentication;
    Api::Ping::PingApi *ping;
    Api::Timeline::ApiTimeline *timeline;
    Api::Assets::ApiAssets *assets;
    Api::Albums::ApiAlbums *albums;
  };

  explicit BaseModule(QObject *parent = nullptr);
  static void setApi(const ApiStruct &apiStruct);

protected:
  static ApiStruct api;

//  inline QString uuidStr(const QUuid &uuid) const {
//    return uuid.toString().replace("{", "").replace("}", "");
//  }
};
} // namespace Immich
#endif // _BASE_MODULE_H
