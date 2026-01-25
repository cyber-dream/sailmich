#ifndef BASE_API_H
#define BASE_API_H

#include "_networkmanager.h"
#include <QEventLoop>
#include <QObject>
#include <QUuid>

namespace Immich {
namespace Api {

class Base : public QObject {
  Q_OBJECT
public:
  explicit Base(QObject *parent = nullptr);

  static void setAuthToken(const QString &token);
  static void setApiUrl(const QUrl &url);

protected:
  static NetworkManager *nm;
  static QUrl apiUrl;
};
} // namespace Api
} // namespace Immich

#endif // BASE_API_H
