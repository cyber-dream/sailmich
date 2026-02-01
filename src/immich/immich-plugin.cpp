#include "immich-plugin.h"
#include <src/immich/api/dto/_albums.h>

#include <src/result/result.h>

#include <QQmlEngine>
#include <QtQml>

#include "is_debug.h"

#include <src/immich/api/api-albums.h>
#include <src/immich/api/api-assets.h>
#include <src/immich/api/api-ping.h>
#include <src/immich/api/api-timeline.h>
#include <src/immich/api/authentication.h>

#include <src/immich/modules/album.h>
#include <src/immich/modules/assets.h>
#include <src/immich/modules/auth.h>
#include <src/immich/modules/ping.h>
// TODO ifdef for aurora
#include <src/immich/modules/secrets/sf_secrets.h>

#include <src/immich/models/album/pathview-model.h>
#include <src/immich/models/album/timeline-model.h>
#include <src/immich/models/albums/albums-list-model.h>

using namespace Immich;

ImmichPlugin *ImmichPlugin::m_singleton = nullptr;
// Module::Album::ModAlbum *ImmichPlugin::mod_album = nullptr;
// Module::Assets::ModAssets *ImmichPlugin::mod_assets = nullptr;
// QThread *ImmichPlugin::m_apiThread = new QThread();

ImmichPlugin::ImmichPlugin(QObject *parent) : QObject(parent) {
  // we may have only one instance
  Q_ASSERT(!m_singleton);
  m_singleton = this;

  m_apiThread = new QThread(this);
  m_apiThread->start();

  BaseModule::setApi({new Api::Authentication::AuthenticationApi(m_apiThread),
                      new Api::Ping::PingApi(m_apiThread),
                      new Api::Timeline::ApiTimeline(m_apiThread),
                      new Api::Assets::ApiAssets(m_apiThread),
                      new Api::Albums::ApiAlbums(m_apiThread)});

  m_secrets = qobject_cast<Module::Secrets::BaseSecretsModule *>(
      new Module::Secrets::SailfishSecretsModule(this));

  mod_auth = new Module::Auth::Auth(this);
  mod_ping = new Module::Ping::Ping(this);
  mod_album = new Module::Album::ModAlbum(this);
  mod_assets = new Module::Assets::ModAssets(this);

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
  /*                Result                   */

  qmlRegisterUncreatableType<Result::PromiseVariant>(
      uri, 1, 0, "PromiseVariant", "Created in C++ only");
  qRegisterMetaType<Result::PromiseVariant *>("PromiseVariant*");

  //  qmlRegisterUncreatableType<Result::Result<QVariant>>(
  //      uri, 1, 0, "ResultVariant", "Created in c++ only");
  //  qRegisterMetaType<Result::Result<QVariant>>("ResultVariant*");

  qRegisterMetaType<Result::Error>("Error");

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
      uri, 1, 0, "Albums", this->albumsSingletonProvider);

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

  qmlRegisterUncreatableType<Api::Albums::AlbumResponseDto>(
      uri, 1, 0, "AlbumResponseDto", "returns from c++");
  //    qRegisterMetaType<Api::Albums::AlbumResponseDto>("AlbumResponseDto");
  //    qmlRegisterUncreatableType<ResultResponse<Auth::LoginResponse>>(
  //        "Immich.Types", 1, 0, "LoginResponsePromise", "Returned from C++
  //        API");

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

  //  qmlRegisterType<Result::PromiseBase>(uri, 1, 0, "ResultPromise");
}

void ImmichPlugin::startInit() {
  const auto res = new Result::Promise<ImmichPlugin::InitStatus>(
      [this]() -> Result::Result<ImmichPlugin::InitStatus> {
        m_initStatus = InitStatusStarted;

        const auto baseUrl = getBaseUrl();
        if (baseUrl.isEmpty()) {
          qInfo() << "baseUrl empty";
          return {InitStatusBaseUrlNotFound};
        }

        Api::Base::setApiUrl(baseUrl.resolved(QUrl("api/")));

        auto resultAuthToken = m_secrets->getAuthToken();
        resultAuthToken->waitForFinished();
        if (!resultAuthToken->getResult().has_value()) {
          qWarning() << "can't get auth token:"
                     << resultAuthToken->getResult().error().message();
          return {InitStatusAuthTokenNotFound};
        }

        Api::Base::setAuthToken(resultAuthToken->getResult().value());

        const auto resPing = mod_ping->ping();
        resPing->waitForFinished();
        if (!resPing->getResult().has_value()) {
          qWarning() << "ping api host failed:"
                     << resPing->getResult().error().message();
          return {InitStatusApiPingFailed};
        }

        const auto reqObj = mod_auth->validateToken();
        reqObj->waitForFinished();
        if (!reqObj->getResult().has_value()) {
          qWarning() << "token invalid:"
                     << reqObj->getResult().error().message();
          return {InitStatusTokenInvalid};
        }

        if (!reqObj->getResult().value()) { // TODO delete bad token
          qWarning() << "token invalid:"
                     << reqObj->getResult().error().message();
          return {InitStatusTokenInvalid};
        }

        qInfo() << "token valid";
        return {InitStatusComplete};
      });

  connect(res, &Result::PromiseBase::finished, [this, res]() {
    if (!res->getResult().has_value()) {
      qDebug() << "error in intialization"
               << res->getResult().error().message();
      return;
    }

    this->m_initStatus = res->getResult().value();
    emit initStatusChanged();
    emit isInitFinishedChanged();
    //    res->deleteLater();
  });
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
  //  const auto modAlbum =
  //  pEngine->setObjectOwnership(modAlbum, QQmlEngine::CppOwnership);
  return qobject_cast<QObject *>(getSingleton()->mod_album);
}

QObject *ImmichPlugin::assetsSingletonProvider(QQmlEngine *, QJSEngine *) {
  return qobject_cast<QObject *>(getSingleton()->mod_assets);
}

void ImmichPlugin::slotOnLoginFinished(
    const Result::Result<Module::Auth::LoginResponse> &response) {

  if (!response.has_value()) {
    qWarning() << "login error";
    return;
  }

  m_settings->beginGroup("Auth");
  m_settings->setValue("base_url", response.value().baseUrl.toString());
  m_settings->endGroup();
  m_settings->sync();

  const auto storeTokenResult =
      m_secrets->storeAuthToken(response.value().accessToken);
  storeTokenResult->waitForFinished();

  if (!storeTokenResult->getResult().has_value()) {
    qWarning() << "can't save token:"
               << storeTokenResult->getResult().error().message();
  }
  Api::Base::setAuthToken(response.value().accessToken);
  Api::Base::setApiUrl(response.value().apiUrl());
}

void ImmichPlugin::slotOnLogoutFinished(
    const Result::Result<Api::Authentication::LogoutResponse> &response) {
  Q_UNUSED(response);
  m_settings->beginGroup("Auth");
  m_settings->remove("base_url");
  m_settings->endGroup();
  m_settings->sync();

  m_secrets->deleteAuthToken();

  Api::Base::setAuthToken("");
  Api::Base::setApiUrl(QUrl());
}
