#include <QDebug>
#include <QString>
#include <Secrets/collectionnamesrequest.h>
#include <Secrets/createcollectionrequest.h>
#include <Secrets/deletesecretrequest.h>
#include <Secrets/findsecretsrequest.h>
#include <Secrets/storedsecretrequest.h>
#include <Secrets/storesecretrequest.h>
#include <src/immich/modules/secrets/sf_secrets.h>

using namespace Immich::Module::Secrets;
const auto auth_token_name = QLatin1String("sailmich_auth_token");
SailfishSecretsModule::SailfishSecretsModule(QObject *parent)
    : BaseSecretsModule(parent),
      m_authTokenIdentifier(
          auth_token_name, QLatin1String(""),
          Sailfish::Secrets::SecretManager::DefaultStoragePluginName) {}

Result::Promise<bool> *
SailfishSecretsModule::storeAuthToken(const QString &pToken) {
  return new Result::Promise<bool>([this,
                                    pToken]() -> Result::Result<bool> const {
    Sailfish::Secrets::Secret secret(m_authTokenIdentifier);
    secret.setData(pToken.toUtf8());
    secret.setType(Sailfish::Secrets::Secret::TypeBlob);

    const auto res = getAuthToken();
    res->waitForFinished();
    if (!res->getResult().isSucceeded()) {
      return {Result::Error(("can't retrive auth token"))}; // FIXME broken flow
    }

    Sailfish::Secrets::SecretManager sm;
    Sailfish::Secrets::StoreSecretRequest ssr;
    ssr.setManager(&sm);
    ssr.setSecret(secret);

    // standalone secret (no collection)
    ssr.setSecretStorageType(
        Sailfish::Secrets::StoreSecretRequest::StandaloneDeviceLockSecret);
    ssr.setDeviceLockUnlockSemantic(
        Sailfish::Secrets::SecretManager::DeviceLockKeepUnlocked);
    ssr.setAccessControlMode(Sailfish::Secrets::SecretManager::OwnerOnlyMode);

    ssr.setEncryptionPluginName(
        Sailfish::Secrets::SecretManager::DefaultEncryptionPluginName);

    ssr.setUserInteractionMode(
        Sailfish::Secrets::SecretManager::SystemInteraction);

    ssr.startRequest();
    ssr.waitForFinished();

    if (ssr.result().code() == Sailfish::Secrets::Result::Failed)
      return {Result::Error("Failed to store secret: " +
                            ssr.result().errorMessage())};

    return {true};
  });
}

Result::Promise<QString> *SailfishSecretsModule::getAuthToken() {
  return new Result::Promise<QString>(
      [this]() -> Result::Result<QString>  {
        Sailfish::Secrets::SecretManager sm;
        Sailfish::Secrets::StoredSecretRequest ssr;
        ssr.setManager(&sm);
        ssr.setIdentifier(m_authTokenIdentifier);
        ssr.setUserInteractionMode(
            Sailfish::Secrets::SecretManager::SystemInteraction);
        ssr.startRequest();
        ssr.waitForFinished();

        if (ssr.result().code() == Sailfish::Secrets::Result::Failed) {
          return {Result::Error("Failed to retrieve secret:" +
                                ssr.result().errorMessage())};
        }

        return Result::Result<QString>(QString::fromUtf8(ssr.secret().data()));
      });
}

Result::Promise<bool> *SailfishSecretsModule::deleteAuthToken() {
  return new Result::Promise<bool>([this]() -> Result::Result<bool> const {
    Sailfish::Secrets::SecretManager sm;
    Sailfish::Secrets::DeleteSecretRequest dsr;
    dsr.setManager(&sm);
    dsr.setIdentifier(m_authTokenIdentifier);
    dsr.setUserInteractionMode(
        Sailfish::Secrets::SecretManager::SystemInteraction);
    dsr.startRequest();
    dsr.waitForFinished();
    if (dsr.result().code() == Sailfish::Secrets::Result::Failed)

      return {Result::Error("Failed to delete secrets collection:" +
                            dsr.result().errorMessage())};

    return {true};
  });
}
