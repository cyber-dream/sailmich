#include "secrets.h"
#include <QDebug>
#include <Secrets/collectionnamesrequest.h>
#include <Secrets/createcollectionrequest.h>
#include <Secrets/deletesecretrequest.h>
#include <Secrets/storedsecretrequest.h>
#include <Secrets/storesecretrequest.h>
#include <Secrets/findsecretsrequest.h>

using namespace Immich::Module::Secrets;
const auto auth_token_name = QLatin1String("sailmich_auth_token");
Secrets::Secrets(QObject *parent)
    : QObject(parent),
      m_authTokenIdent(
          auth_token_name, QLatin1String(""),
          Sailfish::Secrets::SecretManager::DefaultStoragePluginName) {}

void Secrets::storeAuthToken(const QString &pToken) {
  Sailfish::Secrets::Secret secret(m_authTokenIdent);
  secret.setData(pToken.toUtf8());
  secret.setType(Sailfish::Secrets::Secret::TypeBlob);

  const auto res = getAuthToken();
  if (!res.result.isEmpty()) {
    deleteAuthToken();
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

  if (ssr.result().code() == Sailfish::Secrets::Result::Failed) {
    qWarning() << "Failed to store secret:" << ssr.result().errorMessage();
  } else {
    qInfo() << "Auth token stored successfully";
  }
}

Secrets::ResultWithErr<QString> Secrets::getAuthToken() {
  Sailfish::Secrets::SecretManager sm;
  Sailfish::Secrets::StoredSecretRequest ssr;
  ssr.setManager(&sm);
  ssr.setIdentifier(m_authTokenIdent);
  ssr.setUserInteractionMode(
      Sailfish::Secrets::SecretManager::SystemInteraction);
  ssr.startRequest();
  ssr.waitForFinished();

  if (ssr.result().code() == Sailfish::Secrets::Result::Failed) {
    const QString errMsg =
        "Failed to retrieve secret:" + ssr.result().errorMessage();
    qWarning() << errMsg;
    return {nullptr, errMsg};
  }

  return {QString::fromUtf8(ssr.secret().data()), nullptr};
}

QString Secrets::deleteAuthToken() {
  Sailfish::Secrets::SecretManager sm;
  Sailfish::Secrets::DeleteSecretRequest dsr;
  dsr.setManager(&sm);
  dsr.setIdentifier(m_authTokenIdent);
  dsr.setUserInteractionMode(
      Sailfish::Secrets::SecretManager::SystemInteraction);
  dsr.startRequest();
  dsr.waitForFinished();
  if (dsr.result().code() == Sailfish::Secrets::Result::Failed) {
    const QString errMsg =
        "Failed to delete secrets collection:" + dsr.result().errorMessage();
    qWarning() << errMsg;
    return errMsg;
  }

  return nullptr;
}
