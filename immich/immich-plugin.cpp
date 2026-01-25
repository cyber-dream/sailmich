#include "immich-plugin.h"
#include <immich/api/dto/_albums.h>

#include <immich/result/result.h>

#include <QQmlEngine>
#include <QtQml>

#include "is_debug.h"

#include <immich/api/api-albums.h>
#include <immich/api/api-assets.h>
#include <immich/api/api-ping.h>
#include <immich/api/api-timeline.h>
#include <immich/api/authentication.h>

#include <immich/modules/auth.h>
#include <immich/modules/mod-album.h>
#include <immich/modules/mod-assets.h>
#include <immich/modules/ping.h>
#include <immich/modules/secrets.h>

#include <immich/models/album/pathview-model.h>
#include <immich/models/album/timeline-model.h>
#include <immich/models/albums/albums-list-model.h>

using namespace Immich;

ImmichPlugin *ImmichPlugin::m_singleton = nullptr;
Module::Album::ModAlbum *ImmichPlugin::mod_album = nullptr;
Module::Assets::ModAssets *ImmichPlugin::mod_assets = nullptr;
QThread *ImmichPlugin::m_apiThread = new QThread();

ImmichPlugin::ImmichPlugin(QObject *parent)
    : QObject(parent), m_secrets(new Module::Secrets::Secrets(this)) {

  BaseModule::setApi({new Api::Authentication::AuthenticationApi(m_apiThread),
                      new Api::Ping::PingApi(m_apiThread),
                      new Api::Timeline::ApiTimeline(m_apiThread),
                      new Api::Assets::ApiAssets(m_apiThread),
                      new Api::Albums::ApiAlbums(m_apiThread)});

  mod_auth = new Module::Auth::Auth(this);
  mod_ping = new Module::Ping::Ping(this);

  connect(mod_auth, &Module::Auth::Auth::loginFinished, this,
          &ImmichPlugin::slotOnLoginFinished);

  connect(mod_auth, &Module::Auth::Auth::logoutFinished, this,
          &ImmichPlugin::slotOnLogoutFinished);

  const QString configFile = QCoreApplication::applicationName() + ".conf";
  m_settings = new QSettings(
      QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
          "/" + configFile,
      QSettings::IniFormat);
};

void ImmichPlugin::registerTypes(const char *uri) {
  Q_UNUSED(uri)
  qmlRegisterUncreatableType<ResultVariantPromise>(
      uri, 1, 0, "ResultVariantPromise", "Created in C++ only");
  qRegisterMetaType<ResultVariantPromise *>("ResultVariantPromise*");

  qRegisterMetaType<ResultError>("ResultError");

  /*                DTO                   */

  qRegisterMetaType<Api::Timeline::TimeBucketAsset>(
      "Api::Timeline::TimeBucketAsset");
  qRegisterMetaType<Api::Models::AssetMediaSize>("Api::Models::AssetMediaSize");

  /*                Auth                  */

  qRegisterMetaType<Module::Auth::Auth *>("Module::Auth::Auth*");
  qmlRegisterUncreatableType<Module::Auth::LoginResponse>(
      uri, 1, 0, "Module::Auth::LoginResponse",
      "Module::Auth::LoginResponse is created in C++ only");

  /*                Album                 */

  qmlRegisterSingletonType<Immich::Module::Album::ModAlbum>(
      uri, 1, 0, "Albums", albumsSingletonProvider);

  qmlRegisterType<Immich::Model::Album::PathViewModel>(uri, 1, 0,
                                                       "PathViewModel");
  qRegisterMetaType<Immich::Model::Album::PathViewModel *>();

  qmlRegisterType<Immich::Model::Album::TimelineAlbumModel>(
      uri, 1, 0, "TimelineAlbumModel");
  qRegisterMetaType<Immich::Model::Album::TimelineAlbumModel *>();

  qmlRegisterType<Immich::Model::Album::TimelineTimeBucketModel>(
      uri, 1, 0, "TimelineTimeBucketModel");
  qRegisterMetaType<Immich::Model::Album::TimelineTimeBucketModel *>();

  qRegisterMetaType<Immich::Model::Album::TimebucketDate>("TimebucketDate");

  /*                Albums                 */
  qmlRegisterType<Immich::Model::Albums::AlbumsListModel>(uri, 1, 0,
                                                          "AlbumsListModel");

  qmlRegisterSingletonType<Immich::Model::Albums::AlbumsListModel>(
      uri, 1, 0, "GlobalAlbumsList",
      [](QQmlEngine *, QJSEngine *) -> QObject * {
        return new Immich::Model::Albums::AlbumsListModel();
      });
  qRegisterMetaType<Model::Albums::AlbumsListModel *>(
      "Model::Albums::AlbumsListModel*");

  qmlRegisterUncreatableType<Api::Albums::AlbumResponseDto>(uri, 1, 0, "AlbumResponseDto", "returns from c++");
//  qRegisterMetaType<Api::Albums::AlbumResponseDto>("AlbumResponseDto");
  //  qmlRegisterUncreatableType<ResultResponse<Auth::LoginResponse>>(
  //      "Immich.Types", 1, 0, "LoginResponsePromise", "Returned from C++
  //      API");

  //  {
  //    qmlRegisterUncreatableType<ApiAssets>(uri, 1, 0, "ApiAssets",
  //                                          "ApiAssets is created in C++
  //                                          only");
  //    qRegisterMetaType<ApiAssets *>("ApiAssets*");
  //  }

  //  {
  //    qmlRegisterUncreatableType<ApiTimeline>(
  //        uri, 1, 0, "ApiTimeline", "ApiTimeline is created in C++ only");
  //    qRegisterMetaType<ApiTimeline *>("ApiTimeline*");

  //    qmlRegisterType<TimelineModel>(uri, 1, 0, "TimelineModel");

  //    qmlRegisterUncreatableType<TimelineRowModel>(
  //        uri, 1, 0, "AlbumRowModel", "AlbumRowModel is created in C++ only");
  //    qRegisterMetaType<TimelineRowModel *>("TimelineRowModel*");
  //  }

  //  {
  //    qmlRegisterUncreatableType<ApiAlbums>(uri, 1, 0, "ApiAlbums",
  //                                          "ApiAlbums is created in C++
  //                                          only");
  //    qRegisterMetaType<ApiAlbums *>("ApiAlbums*");

  //    qmlRegisterType<AlbumModel>(uri, 1, 0, "AlbumModel");
  //    qmlRegisterType<AlbumsListModel>(uri, 1, 0, "AlbumsListModel");
  //  }

  qRegisterMetaType<tl::optional<QString>>("tl::optional<QString>");

  QMetaType::registerConverter<QString, tl::optional<QString>>(
      [](const QString &s) { return tl::optional<QString>(s); });

  QMetaType::registerConverter<tl::optional<QString>, QString>(
      [](const tl::optional<QString> &opt) { return opt.value_or(""); });
}

ImmichPlugin::InitResult ImmichPlugin::init() {
  const auto baseUrl = getBaseUrl();
  if (baseUrl.isEmpty()) {
    qInfo() << "baseUrl empty";
    return NEED_AUTH;
  }

  Api::Base::setApiUrl(baseUrl.resolved(QUrl("api/")));

  const auto resultAuthToken = m_secrets->getAuthToken();
  if (!resultAuthToken.err.isNull()) {
    qWarning() << "can't get auth token:" << resultAuthToken.err;
    return NEED_AUTH;
  }

  Api::Base::setAuthToken(resultAuthToken.result);

  const auto resPing = mod_ping->ping();
  resPing->waitForFinished();
  if (resPing->getCode() != ResultPromiseBase::ResultCode::Succeeded) {
    qWarning() << "ping api host failed:"
             << resPing->getResult().error().message();
    return OFFLINE;
  }

  const auto reqObj = mod_auth->validateToken();
  reqObj->waitForFinished();
  if (reqObj->getCode() != ResultPromiseBase::ResultCode::Succeeded) {
    qWarning() << "token invalid:" << reqObj->getResult().error().message();
    return NEED_AUTH;
  }

  if (!reqObj->getResult().data()) {
    qWarning() << "token invalid:" << reqObj->getResult().error().message();
    return NEED_AUTH;
  }

  qInfo() << "token valid";
  return OK;
}

Module::Auth::Auth *ImmichPlugin::getAuth() const { return mod_auth; }

Module::Album::ModAlbum *ImmichPlugin::getAlbum() const { return mod_album; }

QUrl ImmichPlugin::getBaseUrl() {
  m_settings->beginGroup("Auth");
  const auto baseUrl = m_settings->value("base_url");
  m_settings->endGroup();
  return QUrl(baseUrl.toString());
}

// void ImmichPlugin::initializeEngine(QQmlEngine *engine, const char *uri) {
//   Q_UNUSED(engine)
//   Q_UNUSED(uri)
//   //  engine->addImageProvider("blurhash", new BlurHashImageProvider());
// }

ImmichPlugin *ImmichPlugin::getSingleton() {
  if (m_singleton == nullptr) {
    m_singleton = new ImmichPlugin();
  }

  return m_singleton;
}

QObject *ImmichPlugin::singletonProvider(QQmlEngine *, QJSEngine *) {
  return qobject_cast<QObject *>(getSingleton());
}

QObject *ImmichPlugin::albumsSingletonProvider(QQmlEngine *, QJSEngine *) {
  if (mod_album == nullptr)
    mod_album = new Module::Album::ModAlbum();

  return qobject_cast<QObject *>(mod_album);
}

QObject *ImmichPlugin::assetsSingletonProvider(QQmlEngine *, QJSEngine *) {

  if (mod_assets == nullptr)
    mod_assets = new Module::Assets::ModAssets(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
            "/thumbnails",
        m_apiThread);

  return qobject_cast<QObject *>(mod_assets);
}

void ImmichPlugin::slotOnLoginFinished(
    const ResultErrored<Module::Auth::LoginResponse> &response) {

  if (response.code() == ResultPromiseBase::ResultCode::Failed) {
    qWarning() << "login error";
    return;
  }

  m_settings->beginGroup("Auth");
  m_settings->setValue("base_url", response.data().baseUrl.toString());
  m_settings->endGroup();
  m_settings->sync();

  m_secrets->storeAuthToken(response.data().accessToken);
  Api::Base::setAuthToken(response.data().accessToken);
  Api::Base::setApiUrl(response.data().apiUrl());
}

void ImmichPlugin::slotOnLogoutFinished(
    const ResultErrored<Api::Authentication::LogoutResponse> &response) {
  Q_UNUSED(response);
  m_settings->beginGroup("Auth");
  m_settings->remove("base_url");
  m_settings->endGroup();
  m_settings->sync();

  m_secrets->deleteAuthToken();

  Api::Base::setAuthToken("");
  Api::Base::setApiUrl(QUrl());
}
