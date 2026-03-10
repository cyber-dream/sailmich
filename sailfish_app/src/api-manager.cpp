#include "api-manager.h"

#include <QDir>
#include <QStandardPaths>

using namespace Immich;

ApiManager::ApiManager(QObject *parent)
    : QObject(parent),
      m_workingDirectory(
          QStandardPaths::writableLocation(QStandardPaths::CacheLocation) +
          "/openapi"), // TODO try create folder and if its unavailable use
                       // fallback
      m_networkManager(new QNetworkAccessManager(this)) {

  QDir dir;
  if (!dir.mkpath(m_workingDirectory)) {
    qWarning() << "can't create directory" << m_workingDirectory
               << ", use fallback path";
    m_workingDirectory = "/tmp/sailmich-fallback/cache/open-api";
  }

  connect(m_networkManager, &QNetworkAccessManager::sslErrors, this,
          &ApiManager::onSSLError);
}

ApiManager::~ApiManager()
{
    QDir dir(m_workingDirectory);
    if (!dir.exists()) return;

    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    for (const QFileInfo &fileInfo : files) {
        QFile::remove(fileInfo.absoluteFilePath());
    }
}

OpenAPI::OAIAuthenticationApi *ApiManager::auth() const {
  return apiFabric<OpenAPI::OAIAuthenticationApi>();
}

OpenAPI::OAITimelineApi *ApiManager::timeline() const {
  return apiFabric<OpenAPI::OAITimelineApi>();
}

OpenAPI::OAIAlbumsApi *ApiManager::albums() const {
  return apiFabric<OpenAPI::OAIAlbumsApi>();
}

OpenAPI::OAIAssetsApi *ApiManager::assets() const {
  return apiFabric<OpenAPI::OAIAssetsApi>();
}

OpenAPI::OAIServerApi *ApiManager::server() const {
  return apiFabric<OpenAPI::OAIServerApi>();
}

const QUrl &ApiManager::endpointUrl() const { return m_endpointUrl; }

void ApiManager::setEndpointUrl(const QUrl &newEndpointUrl) {
  if (m_endpointUrl == newEndpointUrl)
    return;
  m_endpointUrl = newEndpointUrl;
  emit endpointUrlChanged();
}

const Settings::AuthSettingsGroup::TokenType &ApiManager::tokenType() const {
  return m_tokenType;
}

void ApiManager::setTokenType(
    const Settings::AuthSettingsGroup::TokenType &newTokenType) {
  if (m_tokenType == newTokenType)
    return;
  m_tokenType = newTokenType;
  emit tokenTypeChanged();
}

const QString &ApiManager::token() const { return m_token; }

void ApiManager::setToken(const QString &newToken) {
  if (m_token == newToken)
    return;
  m_token = newToken;
  emit tokenChanged();
}

bool ApiManager::isIgnoreSSLErrors() const { return m_isIgnoreSSLErrors; }

void ApiManager::setIsIgnoreSSLErrors(bool newIsIgnoreSSLErrors) {
  if (m_isIgnoreSSLErrors == newIsIgnoreSSLErrors)
    return;
  m_isIgnoreSSLErrors = newIsIgnoreSSLErrors;

  emit isIgnoreSSLErrorsChanged();
}

void ApiManager::onSSLError(QNetworkReply *reply, const QList<QSslError> &) {
  if (m_isIgnoreSSLErrors)
    reply->ignoreSslErrors();
}

const QString &ApiManager::workingDirectory() const {
  return m_workingDirectory;
}

void ApiManager::setWorkingDirectory(const QString &newWorkingDirectory) {
  if (m_workingDirectory == newWorkingDirectory)
    return;
  m_workingDirectory = newWorkingDirectory;
  emit workingDirectoryChanged();
}
