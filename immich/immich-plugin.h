#ifndef IMMICHPLUGIN_H
#define IMMICHPLUGIN_H

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include <QStandardPaths>
#include <immich/result/result.h>

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
class Secrets;
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
public:
  explicit ImmichPlugin(QObject *parent = nullptr);

  void registerTypes(const char *uri);

  enum InitResult { OK, OFFLINE, NEED_AUTH };
  Q_ENUM(InitResult)

  // TODO Make async
  Q_INVOKABLE InitResult init();

  Module::Auth::Auth *getAuth() const;
  Module::Album::ModAlbum *getAlbum() const;
  static ImmichPlugin *getSingleton();
  static QObject *singletonProvider(QQmlEngine *, QJSEngine *);
  static QObject *albumsSingletonProvider(QQmlEngine *, QJSEngine *);
  static QObject *assetsSingletonProvider(QQmlEngine *, QJSEngine *);

  enum QStandardLocation {
    DesktopLocation,
    DocumentsLocation,
    FontsLocation,
    ApplicationsLocation,
    MusicLocation,
    MoviesLocation,
    PicturesLocation,
    TempLocation,
    HomeLocation,
    DataLocation,
    CacheLocation,
    GenericDataLocation,
    RuntimeLocation,
    ConfigLocation,
    DownloadLocation,
    GenericCacheLocation,
    GenericConfigLocation,
    AppDataLocation,
    AppConfigLocation,
    AppLocalDataLocation = DataLocation
  };

  Q_ENUM(QStandardLocation)

  Q_INVOKABLE QString qStandartPath(QStandardLocation pLoc) const {
    return QStandardPaths::writableLocation(
        static_cast<QStandardPaths::StandardLocation>(pLoc));
  }

  // signals:

public slots:
  void slotOnLoginFinished(
      const ResultErrored<Module::Auth::LoginResponse> &response);
  void slotOnLogoutFinished(
      const ResultErrored<Api::Authentication::LogoutResponse> &response);

private:
  QSettings *m_settings;
  QUrl getBaseUrl();
  Module::Secrets::Secrets *m_secrets;
  static QThread *m_apiThread;
  Module::Auth::Auth *mod_auth;
  Module::Ping::Ping *mod_ping;

  static Module::Album::ModAlbum *mod_album;
  static Module::Assets::ModAssets *mod_assets;
  static ImmichPlugin *m_singleton;

};
} // namespace Immich

#endif // IMMICHPLUGIN_H
