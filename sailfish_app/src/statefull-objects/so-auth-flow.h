#ifndef SOAuthFlow_H
#define SOAuthFlow_H

#include <QObject>
#include <QUrl>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>
#include <optional.hpp>
#include <src/api-manager.h>

#include <OAIAuthenticationApi.h>
#include <OAIServerApi.h>

namespace Immich {
class DBus;
class Secrets;
namespace SO {
class AuthFlow : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString endpointUrl READ endpointUrl WRITE setEndpointUrl NOTIFY
                 endpointUrlChanged)
  Q_PROPERTY(bool isIgnoreSSLErrors READ getIsIgnoreSSLErrors WRITE
                 setIsIgnoreSSLErrors NOTIFY isIgnoreSSLErrorsChanged)
  Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
  Q_PROPERTY(
      QString password READ password WRITE setPassword NOTIFY passwordChanged)
  Q_PROPERTY(AuthMetod authMethod READ authMethod WRITE setAuthMethod NOTIFY
                 authMethodChanged)
  Q_PROPERTY(QUrl oAuth2Link READ oAuth2Link WRITE setOAuth2Link NOTIFY
                 oAuth2LinkChanged)
  Q_PROPERTY(
      QString apiToken READ apiToken WRITE setApiToken NOTIFY apiTokenChanged)
  Q_PROPERTY(QVariantMap features READ features WRITE setFeatures NOTIFY
                 featuresChanged)
public:
  enum AuthMetod {
    AuthMethodLoginPassword,
    AuthMethodOAuth2,
    AuthMetodApiToken
  };
  Q_ENUM(AuthMetod)

  explicit AuthFlow(QObject *parent = nullptr);

  QString endpointUrl() const;

  // TODO overload func with QString, test it in qml
  void setEndpointUrl(const QString &pEndpointUrl);

  bool getIsIgnoreSSLErrors() const;
  void setIsIgnoreSSLErrors(bool pIsIgnoreSSLErrors);

  const QString &email() const;
  void setEmail(const QString &newEmail);

  const QString &password() const;
  void setPassword(const QString &newPassword);

  AuthMetod authMethod() const;
  void setAuthMethod(AuthMetod newAuthMethod);

  const QUrl &oAuth2Link() const;
  void setOAuth2Link(const QUrl &newOAuth2Link);

  const QString &apiToken() const;
  void setApiToken(const QString &newApiToken);

  Q_INVOKABLE void startAuth();
  Q_INVOKABLE void verifyEndpointUrl();

  QVariantMap features() const;
  void setFeatures(QVariantMap newFeatures);

public slots:
  void slotOnOAuth2CallbackUrl(const QString &url);
signals:
  void endpointUrlChanged();
  void isIgnoreSSLErrorsChanged();
  void serverFeaturesChanged();
  void emailChanged();
  void passwordChanged();
  void authMethodChanged();
  void oAuth2LinkChanged();

  void endpointVerificationCompleted();
  void endpointVerificationFailed(QString error);

  void loginCompleted();
  void loginFailed(QString error);

  void apiTokenChanged();

  void featuresChanged();

private:
  struct OAuth2Stored {
    QString state;
    QByteArray codeVerifier;
  };

  DBus *m_dBusHandler;

  Settings::Settings *m_settings;
  Secrets *m_secrets;

  void startLogin();
  void startLoginOAuth2();
  void startLoginApiToken();
  void finishLogin(const QString &pAccessToken,
                   Settings::AuthSettingsGroup::TokenType pTokenType);

  QUrl m_oAuth2Link;

  QString m_email;
  QString m_password;

  QUrl m_endpointUrl;
  QString m_apiToken;

  bool m_isIgnoreSSLErrors = false;

  AuthMetod m_authMethod = AuthMethodLoginPassword;

  tl::optional<OAuth2Stored> m_storedOAuth2;
  OpenAPI::OAIServerFeaturesDto m_features;

  ApiManager *m_apiManager;

  inline QUrl apiUrl() const;
};
} // namespace SO
} // namespace Immich
#endif // SOAuthFlow_H
