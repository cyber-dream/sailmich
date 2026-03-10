#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <OAIAlbumsApi.h>
#include <OAIAssetsApi.h>
#include <OAIAuthenticationApi.h>
#include <OAIServerApi.h>
#include <OAITimelineApi.h>
#include <QObject>
#include <src/settings.h>

namespace Immich {

class ApiManager : public QObject {
  Q_OBJECT
  Q_PROPERTY(QUrl endpointUrl READ endpointUrl WRITE setEndpointUrl NOTIFY
                 endpointUrlChanged)
  Q_PROPERTY(Settings::AuthSettingsGroup::TokenType tokenType READ tokenType
                 WRITE setTokenType NOTIFY tokenTypeChanged)
  Q_PROPERTY(QString token READ token WRITE setToken NOTIFY tokenChanged)
  Q_PROPERTY(bool isIgnoreSSLErrors READ isIgnoreSSLErrors WRITE
                 setIsIgnoreSSLErrors NOTIFY isIgnoreSSLErrorsChanged)
public:
  explicit ApiManager(QObject *parent = nullptr);
    ~ApiManager();

  OpenAPI::OAIAuthenticationApi *auth() const;
  OpenAPI::OAITimelineApi *timeline() const;
  OpenAPI::OAIAlbumsApi *albums() const;
  OpenAPI::OAIAssetsApi *assets() const;
  OpenAPI::OAIServerApi *server() const;

  const QUrl &endpointUrl() const;
  void setEndpointUrl(const QUrl &newEndpointUrl);

  const Settings::AuthSettingsGroup::TokenType &tokenType() const;
  void setTokenType(const Settings::AuthSettingsGroup::TokenType &newTokenType);

  const QString &token() const;
  void setToken(const QString &newToken);

  bool isIgnoreSSLErrors() const;
  void setIsIgnoreSSLErrors(bool newIsIgnoreSSLErrors);

  const QString &workingDirectory() const;
  void setWorkingDirectory(const QString &newWorkingDirectory);

signals:
  void endpointUrlChanged();
  void tokenTypeChanged();
  void tokenChanged();
  void isIgnoreSSLErrorsChanged();

  void workingDirectoryChanged();

public slots:
  void onSSLError(QNetworkReply *reply, const QList<QSslError> &errors);

private:
  template <typename T> T *apiFabric() const {
    auto api = new T();
    api->setNewServerForAllOperations(m_endpointUrl);
    api->setNetworkAccessManager(m_networkManager);
    api->setWorkingDirectory(m_workingDirectory);

    switch (m_tokenType) {
    case Settings::AuthSettingsGroup::TokenType::TokenTypeApi:
      //      api->setApiKey("x-api-key", m_token);
      api->addHeaders("x-api-key", m_token);
      break;
    case Settings::AuthSettingsGroup::TokenType::TokenTypeAuth:
      api->setBearerToken(m_token);
      break;
    }

    return api;
  };

  QUrl m_endpointUrl;
  Settings::AuthSettingsGroup::TokenType m_tokenType;
  QString m_token;
  QString m_workingDirectory;
  QNetworkAccessManager *m_networkManager;
  bool m_isIgnoreSSLErrors = false;
  Q_PROPERTY(QString workingDirectory READ workingDirectory WRITE
                 setWorkingDirectory NOTIFY workingDirectoryChanged)
};
} // namespace Immich
#endif // APIMANAGER_H
