#ifndef SF_SECRETS_H
#define SF_SECRETS_H

#include <src/immich/modules/secrets/_secrets-base.h>

#include <QObject>
#include <Secrets/secret.h>

namespace Immich {
namespace Module {
namespace Secrets {
class SailfishSecretsModule final : public BaseSecretsModule {
  Q_OBJECT
public:
  // TODO replace with result.h
  template <typename T> struct ResultWithErr {
    T result;
    QString err;
  };
  explicit SailfishSecretsModule(QObject *parent = nullptr);

  enum CollectionExistsResult {
    NotExists,
    Exists,
    Error,
  };

   Result::Promise<QString> *getAuthToken() override;
   Result::Promise<bool> *storeAuthToken(const QString &pToken) override;
   Result::Promise<bool> *deleteAuthToken() override;

signals:
protected:
private:
  Sailfish::Secrets::Secret::Identifier m_authTokenIdentifier;
};
} // namespace Secrets
} // namespace Module
} // namespace Immich

#endif // SF_SECRETS_H
