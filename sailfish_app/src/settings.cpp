#include "settings.h"

#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QUrl>
#include <src/settings.h>

using namespace Immich::Settings;

QString AuthSettingsGroup::FieldNameEndpointUrl = "base_url";
QString AuthSettingsGroup::FieldNameTokenType = "token_type";
QString AuthSettingsGroup::FieldNameIsIgnoreSSLErrors = "ignore_ssl_errors";

Settings::Settings(QObject *parent)
    : QObject(parent),
      m_settings(new QSettings(
          QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) +
              "/" + QCoreApplication::applicationName() + ".conf",
          QSettings::IniFormat)),
      m_auth(new AuthSettingsGroup("Auth", m_settings, this)) {

  //  qInfo() << "settings file path is:" << m_settings-;
  // TODO migration for old baseUrl
}

AuthSettingsGroup *Settings::auth() const { return m_auth; }

QUrl AuthSettingsGroup::endpointUrl() const {
  QUrl endpointUrl;
  asGroup([this, &endpointUrl]() {
    endpointUrl = m_settings->value(FieldNameEndpointUrl, QUrl()).toUrl();
  });

  return endpointUrl;
}

void AuthSettingsGroup::setEndpointUrl(const QUrl &pEndpointUrl) {
  if (!pEndpointUrl.isValid()) {
    qWarning() << FieldNameEndpointUrl << "is invalid";
    return;
  }

  asGroup([this, pEndpointUrl]() {
    if (m_settings->value(FieldNameEndpointUrl, QUrl()).toUrl() == pEndpointUrl)
      return;

    m_settings->setValue(FieldNameEndpointUrl, pEndpointUrl.toString());

    emit endpointAddressChanged();
  });
}

AuthSettingsGroup::TokenType AuthSettingsGroup::tokenType() const {
  TokenType tokenType;
  asGroup([this, &tokenType]() {
    tokenType = static_cast<TokenType>(
        m_settings->value(FieldNameTokenType, TokenType::TokenTypeAuth)
            .toInt());
  });

  return tokenType;
}

void AuthSettingsGroup::setTokenType(const TokenType pTokenType) {
  asGroup([this, pTokenType]() {
    if (m_settings->value(FieldNameTokenType, TokenType::TokenTypeAuth) == pTokenType)
      return;

    m_settings->setValue(FieldNameTokenType, pTokenType);
    emit tokenTypeChanged();
  });
}

bool AuthSettingsGroup::isIgnoreSSLErrors() const {
  bool isIgnoreSSLErrors = false;
  asGroup([this, &isIgnoreSSLErrors]() {
    isIgnoreSSLErrors =
        m_settings->value(FieldNameIsIgnoreSSLErrors, false).toBool();
  });

  return isIgnoreSSLErrors;
}
void AuthSettingsGroup::setIsIgnoreSSLErrors(const bool pIsIgnoreSSLErrors) {
  asGroup([this, &pIsIgnoreSSLErrors]() {
    if (m_settings->value(FieldNameIsIgnoreSSLErrors).toBool() ==
        pIsIgnoreSSLErrors)
      return;

    m_settings->setValue(FieldNameIsIgnoreSSLErrors, pIsIgnoreSSLErrors);
    emit isIgnoreSSLErrorsChanged();
  });
};
