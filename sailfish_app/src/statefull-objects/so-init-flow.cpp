#include "so-init-flow.h"

#include <QDebug>

using namespace Immich::SO;

InitFlow::InitFlow(QObject *parent)
    : QObject(parent), m_secrets(QInjection::Pointer<Secrets>()),
      m_settings(QInjection::Pointer<Settings::Settings>()),
      m_apiManager(QInjection::Pointer<Immich::ApiManager>()) {}

void InitFlow::start() {
  const auto endpointUrl = m_settings->auth()->endpointUrl();
  if (endpointUrl.isEmpty()) {
    qInfo() << "endpointUrl is empty";
    setInitStatus(StatusEndpointUrlNotFound);
    emit finished();
    return;
  }

  const auto futureToken = m_secrets->getAuthToken();
  const auto watcher = new QFutureWatcher<tl::expected<QString, Secrets::Error>>();
  watcher->setFuture(futureToken);

  connect(
      watcher, &QFutureWatcherBase::finished,
      [this, futureToken, endpointUrl]() -> void {
        if (!futureToken.result().has_value()) {
          qWarning() << "can't retrive token:" <<  futureToken.result().error().code <<  futureToken.result().error().message;
          setInitStatus(StatusAuthTokenNotFound);
          emit finished();
          return;
        }

        if (futureToken.result().value().isEmpty() ||
            futureToken.result().value().isNull()) {
          qWarning() << "auth token is empty";
          setInitStatus(StatusAuthTokenNotFound);
          emit finished();
          return;
        }

        m_apiManager->setEndpointUrl(endpointUrl.resolved(QUrl("api")));
        m_apiManager->setTokenType(m_settings->auth()->tokenType());
        m_apiManager->setToken(futureToken.result().value());
        m_apiManager->setIsIgnoreSSLErrors(
            m_settings->auth()->isIgnoreSSLErrors());

        const auto serverApi = m_apiManager->server();

        connect(
            serverApi, &OpenAPI::OAIServerApi::pingServerSignal,
            [this, serverApi](OpenAPI::OAIServerPingResponse) {
              qInfo() << "server ping successfull";

              const auto authApi = m_apiManager->auth();

              connect(authApi,
                      &OpenAPI::OAIAuthenticationApi::validateAccessTokenSignal,
                      [this, authApi, serverApi](
                          OpenAPI::OAIValidateAccessTokenResponseDto response) {
                        if (!response.isAuthStatus()) {
                          qWarning() << "auth status is false";
                          setInitStatus(StatusTokenInvalid);
                          emit finished();
                        } else {
                          qInfo() << "token valid";
                          setInitStatus(StatusComplete);
                          emit finished();
                        }

                        authApi->deleteLater();
                        serverApi->deleteLater();
                      });
              connect(
                  authApi,
                  &OpenAPI::OAIAuthenticationApi::
                      validateAccessTokenSignalError,
                  [this, authApi, serverApi](
                      OpenAPI::OAIValidateAccessTokenResponseDto,
                      QNetworkReply::NetworkError, const QString &error_str) {
                    qWarning() << "validate auth token failed:" << error_str;

                    setInitStatus(StatusTokenInvalid);
                    emit finished();
                    authApi->deleteLater();
                    serverApi->deleteLater();
                  });

              authApi->validateAccessToken();
            });
        connect(serverApi, &OpenAPI::OAIServerApi::pingServerSignalError,
                [this, serverApi](OpenAPI::OAIServerPingResponse,
                                  QNetworkReply::NetworkError,
                                  const QString &error_str) {
                  qWarning() << "server ping failed:" << error_str;
                  setInitStatus(StatusApiPingFailed);
                  emit finished();
                  serverApi->deleteLater();
                });

        serverApi->pingServer();
      });
}

InitFlow::Status InitFlow::initStatus() const { return m_initStatus; }

void InitFlow::setInitStatus(Status newInitStatus) {
  if (m_initStatus == newInitStatus)
    return;
  m_initStatus = newInitStatus;
  emit initStatusChanged();
}

