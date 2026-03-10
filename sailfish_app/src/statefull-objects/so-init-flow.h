#ifndef FAPLUGININIT_H
#define FAPLUGININIT_H

#include "dependencypointer.h"
#include <QObject>
#include <src/api-manager.h>
#include <src/secrets.h>
#include <src/settings.h>

namespace Immich {
namespace SO {

class InitFlow : public QObject {
  Q_OBJECT
  Q_PROPERTY(Status initStatus READ initStatus WRITE setInitStatus NOTIFY
                 initStatusChanged)
public:
  enum Status {
    StatusNotStarted,
    StatusStarted,
    StatusEndpointUrlNotFound,
    StatusAuthTokenNotFound,
    StatusApiPingFailed,
    StatusTokenInvalid,
    StatusComplete
  };
  Q_ENUM(Status)

  explicit InitFlow(QObject *parent = nullptr);
  Q_INVOKABLE void start();
  Status initStatus() const;
  void setInitStatus(Status newInitStatus);

signals:
  void finished();
  void initStatusChanged();

private:
  void _start();
  Status m_initStatus = StatusNotStarted;
  Secrets *m_secrets;
  Settings::Settings *m_settings;
  ApiManager *m_apiManager;
};
} // namespace SO
} // namespace Immich
#endif // FAPLUGININIT_H
