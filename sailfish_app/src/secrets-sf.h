#ifndef SF_SECRETS_H
#define SF_SECRETS_H

#include <Secrets/result.h>
#include <Secrets/secret.h>
#include <Secrets/secretmanager.h>
#include <expected.hpp>
#include <src/secrets.h>

namespace Immich {
class SecretsSailfish final : public Secrets {
  Q_OBJECT
public:
  explicit SecretsSailfish(QObject *parent = nullptr);

  QFuture<tl::expected<QString, Secrets::Error>> getAuthToken() override;
  QFuture<tl::expected<bool, Secrets::Error>>
  storeAuthToken(const QString &pToken) override;
  QFuture<tl::expected<bool, Secrets::Error>> deleteAuthToken() override;


  Q_INVOKABLE void q_deleteAuthToken() override;
signals:
protected:
private:
  Sailfish::Secrets::Secret::Identifier m_authTokenIdentifier;
  Sailfish::Secrets::SecretManager m_secretManager;
};
} // namespace Immich
#endif // SF_SECRETS_H
