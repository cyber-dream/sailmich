#ifndef SETTINGS_H
#define SETTINGS_H

#include <QMetaProperty>
#include <QObject>
#include <QSettings>
#include <QUrl>
#include <functional>

namespace Immich {
class ImmichPlugin;
namespace Settings {
struct BaseSettingsGroup : public QObject {
  Q_OBJECT
public:
  explicit BaseSettingsGroup(const QString &pGroupName, QSettings *pSettings,
                             QObject *parent)
      : QObject(parent), m_groupName(pGroupName), m_settings(pSettings) {}
  virtual ~BaseSettingsGroup() = default;

protected:
  template <typename F> inline void asGroup(F &&pWork) const {
    m_settings->beginGroup(m_groupName);
    pWork();
    m_settings->endGroup();
    m_settings->sync();
  }

  QString m_groupName;
  QSettings *m_settings;
};

struct AuthSettingsGroup : public BaseSettingsGroup {
  Q_OBJECT
  Q_DISABLE_COPY(AuthSettingsGroup);
  Q_PROPERTY(QUrl endpointUrl READ endpointUrl WRITE setEndpointUrl NOTIFY
                 endpointAddressChanged)
  Q_PROPERTY(TokenType tokenType READ tokenType WRITE setTokenType NOTIFY
                 tokenTypeChanged)
  Q_PROPERTY(bool isIgnoreSSLErrors READ isIgnoreSSLErrors WRITE
                 setIsIgnoreSSLErrors NOTIFY isIgnoreSSLErrorsChanged)
public:
  AuthSettingsGroup(const QString &pGroupName, QSettings *pSettings,
                    QObject *parent = nullptr)
      : BaseSettingsGroup(pGroupName, pSettings, parent) {};

  QUrl endpointUrl() const;
  void setEndpointUrl(const QUrl &pEndpointUrl);

  bool isIgnoreSSLErrors() const;
  void setIsIgnoreSSLErrors(const bool);

  enum TokenType { TokenTypeAuth = 0, TokenTypeApi };
  Q_ENUM(TokenType)

  TokenType tokenType() const;
  void setTokenType(const TokenType pTokenType);

signals:
  void endpointAddressChanged();
  void tokenTypeChanged();
  void isIgnoreSSLErrorsChanged();

private:
  static QString FieldNameEndpointUrl;
  static QString FieldNameTokenType;
  static QString FieldNameIsIgnoreSSLErrors;
};

class Settings : public QObject {
  Q_OBJECT
  //  Q_PROPERTY(MainSettingsGroup *main READ main CONSTANT)
  Q_PROPERTY(AuthSettingsGroup *auth READ auth CONSTANT)
  Q_DISABLE_COPY(Settings);

public:
  Settings(QObject *parent = nullptr);

  AuthSettingsGroup *auth() const;

private:
  QSettings *m_settings;
  AuthSettingsGroup *m_auth;
};

} // namespace Settings
} // namespace Immich

#endif // SETTINGS_H
