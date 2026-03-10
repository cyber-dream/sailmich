#include "so-auth-flow.h"

#include <QDesktopServices>

#include <src/dbus.h>

#include <OAIAuthenticationApi.h>

#include <src/secrets.h>
#include <src/settings.h>

#include <QCryptographicHash>
#include <QString>
#include <algorithm>
#include <random>

#include <dependencypointer.h>
using namespace Immich::SO;

AuthFlow::AuthFlow(QObject *parent)
    : QObject(parent), m_dBusHandler(QInjection::Pointer<Immich::DBus>()),
      m_settings(QInjection::Pointer<Settings::Settings>()),
      m_secrets(QInjection::Pointer<Immich::Secrets>()),
      m_endpointUrl(m_settings->auth()->endpointUrl().toString()),
      m_isIgnoreSSLErrors(m_settings->auth()->isIgnoreSSLErrors()),
      m_features(OpenAPI::OAIServerFeaturesDto()),
      m_apiManager(QInjection::Pointer<Immich::ApiManager>()) {}

QString AuthFlow::endpointUrl() const { return m_endpointUrl.toString(); }

void AuthFlow::setEndpointUrl(const QString &pEndpointUrl) {
  if (m_endpointUrl == QUrl(pEndpointUrl))
    return;

  m_endpointUrl = QUrl(pEndpointUrl);
  m_settings->auth()->setEndpointUrl(m_endpointUrl);

  emit endpointUrlChanged();
}

bool AuthFlow::getIsIgnoreSSLErrors() const { return m_isIgnoreSSLErrors; }

void AuthFlow::setIsIgnoreSSLErrors(bool pIsIgnoreSSLErrors) {
  if (m_isIgnoreSSLErrors == pIsIgnoreSSLErrors)
    return;
  m_isIgnoreSSLErrors = pIsIgnoreSSLErrors;

  m_apiManager->setIsIgnoreSSLErrors(m_isIgnoreSSLErrors);
  m_settings->auth()->setIsIgnoreSSLErrors(m_isIgnoreSSLErrors);

  emit isIgnoreSSLErrorsChanged();
}

const QString &AuthFlow::email() const { return m_email; }

void AuthFlow::setEmail(const QString &newEmail) {
  if (m_email == newEmail)
    return;
  m_email = newEmail;
  emit emailChanged();
}

const QString &AuthFlow::password() const { return m_password; }

void AuthFlow::setPassword(const QString &newPassword) {
  if (m_password == newPassword)
    return;
  m_password = newPassword;
  emit passwordChanged();
}

AuthFlow::AuthMetod AuthFlow::authMethod() const { return m_authMethod; }

void AuthFlow::setAuthMethod(AuthMetod newAuthMethod) {
  if (m_authMethod == newAuthMethod)
    return;
  m_authMethod = newAuthMethod;
  emit authMethodChanged();
}

void AuthFlow::verifyEndpointUrl() { // TODO check prefixes, trails and etc
  const auto serverApi = m_apiManager->server();
  serverApi->setNewServerForAllOperations(apiUrl());

  connect(serverApi, &OpenAPI::OAIServerApi::pingServerSignal,
          [this, serverApi](OpenAPI::OAIServerPingResponse) {
            qInfo() << "Host pinged successfully";

            serverApi->getServerFeatures();

            connect(serverApi, &OpenAPI::OAIServerApi::getServerFeaturesSignal,
                    [this, serverApi](OpenAPI::OAIServerFeaturesDto summary) {
                      m_features = summary;
                      emit endpointVerificationCompleted();
                      serverApi->deleteLater();
                    });

            connect(serverApi,
                    &OpenAPI::OAIServerApi::getServerFeaturesSignalError,
                    [this, serverApi](OpenAPI::OAIServerFeaturesDto,
                                      QNetworkReply::NetworkError,
                                      QString errorStr) {
                      emit endpointVerificationFailed(errorStr);
                      serverApi->deleteLater();
                    });
          });

  connect(serverApi, &OpenAPI::OAIServerApi::pingServerSignalError,
          [this, serverApi](OpenAPI::OAIServerPingResponse,
                            QNetworkReply::NetworkError, QString errorStr) {
            qWarning() << "Ping error:" << errorStr;
            emit endpointVerificationFailed(errorStr);
            serverApi->deleteLater();
          });

  serverApi->pingServer();
}

void AuthFlow::startAuth() {
  switch (m_authMethod) {
  case AuthMethodLoginPassword:
    startLogin();
    return;
  case AuthMethodOAuth2:
    startLoginOAuth2();
    return;
  case AuthMetodApiToken:
    startLoginApiToken();
    return;
  }
}

void AuthFlow::startLogin() {
  const auto authApi = m_apiManager->auth();
  authApi->setNewServerForAllOperations(apiUrl());

  auto creds = OpenAPI::OAILoginCredentialDto{};
  creds.setEmail(m_email);
  creds.setPassword(m_password);

  connect(authApi, &OpenAPI::OAIAuthenticationApi::loginSignal,
          [this, authApi](OpenAPI::OAILoginResponseDto loginResponse) {
            qInfo() << "Login response succesfull";

            finishLogin(loginResponse.getAccessToken(),
                        Settings::AuthSettingsGroup::TokenType::TokenTypeAuth);

            authApi->deleteLater();
          });

  connect(authApi, &OpenAPI::OAIAuthenticationApi::loginSignalError,
          [this, authApi](OpenAPI::OAILoginResponseDto,
                          QNetworkReply::NetworkError, QString errorStr) {
            qWarning() << "Login failed" << errorStr;
            emit loginFailed(errorStr);
            authApi->deleteLater();
          });

  authApi->login(creds);
}

void AuthFlow::startLoginApiToken() {
  if (m_apiToken.isEmpty() || m_apiToken.isNull()) {
    emit loginFailed("empty api token");
    return;
  }

  const auto authApi = m_apiManager->auth();
  authApi->setNewServerForAllOperations(apiUrl());
  //  authApi->setApiKey("api_key", m_apiToken);
  authApi->addHeaders("x-api-key", m_apiToken);

  connect(authApi, &OpenAPI::OAIAuthenticationApi::getAuthStatusSignal,
          [this, authApi](OpenAPI::OAIAuthStatusResponseDto) {
            qInfo() << "Api Token validated";

            finishLogin(m_apiToken,
                        Settings::AuthSettingsGroup::TokenType::TokenTypeApi);
            authApi->deleteLater();
          });

  connect(authApi, &OpenAPI::OAIAuthenticationApi::getAuthStatusSignalError,
          [this, authApi](OpenAPI::OAIAuthStatusResponseDto,
                          QNetworkReply::NetworkError, QString errorStr) {
            qWarning() << "Api token validation failed:" << errorStr;
            emit loginFailed(errorStr);
            authApi->deleteLater();
          });

  authApi->getAuthStatus();
}

void AuthFlow::startLoginOAuth2() {
  const auto authApi = m_apiManager->auth();
  authApi->setNewServerForAllOperations(apiUrl());

  const char chars[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
  QByteArray codeVerifier;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, sizeof(chars) - 2);

  for (int i = 0; i < 64; ++i) {
    codeVerifier.append(chars[dis(gen)]);
  }

  const auto hash =
      QCryptographicHash::hash(codeVerifier, QCryptographicHash::Sha256);

  const auto codeChallenge = hash.toBase64(QByteArray::Base64UrlEncoding |
                                           QByteArray::OmitTrailingEquals);

  const auto state = QString::number(gen());

  m_storedOAuth2 = {state, codeVerifier};

  connect(m_dBusHandler, &DBus::oAuth2CallbackUrlReceived, this,
          &AuthFlow::slotOnOAuth2CallbackUrl);

  connect(authApi, &OpenAPI::OAIAuthenticationApi::startOAuthSignal, this,
          [this, authApi](OpenAPI::OAIOAuthAuthorizeResponseDto summary) {
            setOAuth2Link(summary.getUrl());

            authApi->deleteLater();
          });

  connect(authApi, &OpenAPI::OAIAuthenticationApi::startOAuthSignalError, this,
          [this, authApi](OpenAPI::OAIOAuthAuthorizeResponseDto,
                          QNetworkReply::NetworkError, QString errorStr) {
            qWarning() << "OAuth2 failed" << errorStr;
            emit loginFailed(errorStr);
            authApi->deleteLater();
          });

  auto params = OpenAPI::OAIOAuthConfigDto();
  params.setCodeChallenge(codeChallenge);
  params.setRedirectUri(
      m_endpointUrl.resolved(QUrl("api/oauth/mobile-redirect"))
          .toString()); // FIXME hardcoded
  params.setState(state);
  authApi->startOAuth(params);
}

const QUrl &AuthFlow::oAuth2Link() const { return m_oAuth2Link; }

void AuthFlow::setOAuth2Link(const QUrl &newOAuth2Link) {
  if (m_oAuth2Link == newOAuth2Link)
    return;
  m_oAuth2Link = newOAuth2Link;
  emit oAuth2LinkChanged();
}

void AuthFlow::slotOnOAuth2CallbackUrl(const QString &pUrl) {
  disconnect(m_dBusHandler, &DBus::oAuth2CallbackUrlReceived, this,
             &AuthFlow::slotOnOAuth2CallbackUrl);

  const auto callbackUrl = QUrl(pUrl);
  if (!callbackUrl.isValid()) {
    qWarning() << "oAuth2 callback url is invalid";
  }
  const auto query = QUrlQuery(callbackUrl);

  if (!query.hasQueryItem("state")) {
    qCritical() << "state not found in oAuth2Callback";
    return;
  }

  if (!query.hasQueryItem("code")) {
    qCritical() << "codeVerifier not found in oAuth2Callback";
    return;
  }

  const auto callbackState = query.queryItemValue("state");
  const auto callbackCode = query.queryItemValue("code");

  if (!m_storedOAuth2.has_value()) {

    return;
  }

  if (m_storedOAuth2->state != callbackState) {
    qWarning() << "unknown state from oAuth2 url callback";
    return;
  }

  auto oAuth = OpenAPI::OAIOAuthCallbackDto();
  oAuth.setCodeVerifier(m_storedOAuth2->codeVerifier);
  oAuth.setState(m_storedOAuth2->state);
  oAuth.setUrl(pUrl);

  const auto authApi = m_apiManager->auth();
  authApi->setNewServerForAllOperations(apiUrl());

  connect(authApi, &OpenAPI::OAIAuthenticationApi::finishOAuthSignal,
          [this, authApi](OpenAPI::OAILoginResponseDto summary) {
            qInfo() << "OAuth completed";
            finishLogin(summary.getAccessToken(),
                        Settings::AuthSettingsGroup::TokenType::TokenTypeAuth);

            authApi->deleteLater();
          });

  connect(authApi, &OpenAPI::OAIAuthenticationApi::finishOAuthSignalError,
          [this, authApi](OpenAPI::OAILoginResponseDto,
                          QNetworkReply::NetworkError, QString errorStr) {
            qWarning() << "OAuth2 failed:" << errorStr;
            emit loginFailed(errorStr);

            authApi->deleteLater();
          });

  authApi->finishOAuth(oAuth);
}

void AuthFlow::finishLogin(const QString &pToken,
                           Settings::AuthSettingsGroup::TokenType pTokenType) {
  m_settings->auth()->setEndpointUrl(m_endpointUrl);
  m_settings->auth()->setTokenType(pTokenType);
  m_settings->auth()->setIsIgnoreSSLErrors(m_isIgnoreSSLErrors);

  auto watcher = new QFutureWatcher<tl::expected<bool, Secrets::Error>>(this);

  connect(watcher, &QFutureWatcher<tl::expected<bool, QString>>::finished, this,
          [this, watcher]() {
            const auto result = watcher->result();

            if (result.has_value()) {
              qInfo() << "Token saved";
            } else {
              qWarning() << "Token save error:" << result.error().code
                         << result.error().message;
            }

            emit loginCompleted();
            watcher->deleteLater();
          });

  watcher->setFuture(m_secrets->storeAuthToken(pToken));

//  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
//    auto result = watcher->result();

//    // TODO work with error

//    emit loginCompleted();
//    watcher->deleteLater();
//  });
}

QUrl AuthFlow::apiUrl() const { return m_endpointUrl.resolved(QUrl("api")); }

QVariantMap AuthFlow::features() const {
  return m_features.asJsonObject().toVariantMap();
}

void AuthFlow::setFeatures(QVariantMap newFeatures) {
  QJsonObject json = QJsonObject::fromVariantMap(newFeatures);
  m_features.fromJsonObject(json);
  emit featuresChanged();
}

const QString &AuthFlow::apiToken() const { return m_apiToken; }

void AuthFlow::setApiToken(const QString &newApiToken) {
  if (m_apiToken == newApiToken)
    return;
  m_apiToken = newApiToken;
  emit apiTokenChanged();
}
