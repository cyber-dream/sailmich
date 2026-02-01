#ifndef IMMICHPLUGIN_H
#define IMMICHPLUGIN_H

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QStandardPaths>
#include <src/result/result.h>

namespace Immich {
namespace Api {
namespace Authentication {
struct LogoutResponse;
}
} // namespace Api
namespace Module {
namespace Auth {
struct LoginResponse;

class Auth;
} // namespace Auth
namespace Album {
class ModAlbum;
}
namespace Secrets {
class BaseSecretsModule;
}
namespace Ping {
class Ping;
}
namespace Assets {
class ModAssets;
}
} // namespace Module
class ImmichPlugin : public QObject {
  Q_OBJECT
  Q_PROPERTY(Immich::Module::Auth::Auth *auth READ getAuth() CONSTANT)
  Q_PROPERTY(Immich::Module::Album::ModAlbum *album READ getAlbum() CONSTANT)
  Q_PROPERTY(InitStatus initStatus READ getInitStatus NOTIFY initStatusChanged)
    Q_PROPERTY(bool isInitFinished READ getIsInitFinished NOTIFY isInitFinishedChanged)
public:
  explicit ImmichPlugin(QObject *parent = nullptr);

  void registerTypes(const char *uri);

  enum InitStatus {
    InitStatusNotStarted,
    InitStatusStarted,
    InitStatusBaseUrlNotFound,
    InitStatusAuthTokenNotFound,
    InitStatusApiPingFailed,
    InitStatusTokenInvalid,
    InitStatusComplete
  };
  Q_ENUM(InitStatus)

  void startInit();

  Module::Auth::Auth *getAuth() const;
  Module::Album::ModAlbum *getAlbum() const;
  static ImmichPlugin *getSingleton();
  static QObject *singletonProvider(QQmlEngine *, QJSEngine *);
  static QObject *albumsSingletonProvider(QQmlEngine *, QJSEngine *);
  static QObject *assetsSingletonProvider(QQmlEngine *, QJSEngine *);

  InitStatus getInitStatus() const { return m_initStatus; }
  bool getIsInitFinished() const {return m_initStatus != InitStatusNotStarted;}
signals:
  void initStatusChanged();
  void isInitFinishedChanged();
public slots:
  void slotOnLoginFinished(
      const Result::Result<Module::Auth::LoginResponse> &response);
  void slotOnLogoutFinished(
      const Result::Result<Api::Authentication::LogoutResponse> &response);

private:
  QSettings *m_settings;
  QUrl getBaseUrl();

  QThread *m_apiThread;

  static ImmichPlugin *m_singleton;

  Module::Album::ModAlbum *mod_album = nullptr;
  Module::Assets::ModAssets *mod_assets = nullptr;
  Module::Auth::Auth *mod_auth = nullptr;
  Module::Ping::Ping *mod_ping = nullptr;
  Module::Secrets::BaseSecretsModule *m_secrets = nullptr;

  InitStatus m_initStatus;
};
} // namespace Immich

#endif // IMMICHPLUGIN_H
