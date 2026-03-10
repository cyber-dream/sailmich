#include <QDebug>
#include <QString>
#include <QtConcurrent/QtConcurrent>
#include <Secrets/collectionnamesrequest.h>
#include <Secrets/createcollectionrequest.h>
#include <Secrets/deletesecretrequest.h>
#include <Secrets/findsecretsrequest.h>
#include <Secrets/storedsecretrequest.h>
#include <Secrets/storesecretrequest.h>
#include <src/secrets-sf.h>

using namespace Immich;

const auto auth_token_name = QLatin1String("sailmich_auth_token");
SecretsSailfish::SecretsSailfish(QObject *parent)
    : Secrets(parent),
      m_authTokenIdentifier(
          auth_token_name, QLatin1String(""),
          Sailfish::Secrets::SecretManager::DefaultStoragePluginName) {}

QFuture<tl::expected<bool, Secrets::Error>>
SecretsSailfish::storeAuthToken(const QString &pToken) {
  auto promise = QSharedPointer<
      QFutureInterface<tl::expected<bool, Secrets::Error>>>::create();
  promise->reportStarted();

  auto deleteOldTokenWatcher =
      new QFutureWatcher<tl::expected<bool, Secrets::Error>>(this);
  deleteOldTokenWatcher->setFuture(deleteAuthToken());

  connect(
      deleteOldTokenWatcher, &QFutureWatcherBase::finished, this,
      [this, promise, pToken, deleteOldTokenWatcher]() {
        const auto deleteResult = deleteOldTokenWatcher->result();

        if (!deleteResult.has_value() && deleteResult.error().code != 40) {
          qWarning() << "delete token error:" << deleteResult.error().code
                     << deleteResult.error().message;
          return;
        }

        Sailfish::Secrets::Secret secret(m_authTokenIdentifier);
        secret.setData(pToken.toUtf8());

        auto ssr = new Sailfish::Secrets::StoreSecretRequest(this);

        ssr->setManager(&m_secretManager);
        ssr->setSecret(secret);
        ssr->setSecretStorageType(
            Sailfish::Secrets::StoreSecretRequest::StandaloneDeviceLockSecret);
        ssr->setDeviceLockUnlockSemantic(
            Sailfish::Secrets::SecretManager::DeviceLockKeepUnlocked);
        ssr->setAccessControlMode(
            Sailfish::Secrets::SecretManager::OwnerOnlyMode);
        ssr->setEncryptionPluginName(
            Sailfish::Secrets::SecretManager::DefaultEncryptionPluginName);
        ssr->setUserInteractionMode(
            Sailfish::Secrets::SecretManager::SystemInteraction);

        QObject::connect(
            ssr, &Sailfish::Secrets::StoreSecretRequest::statusChanged,
            [promise, ssr]() {
              if (ssr->status() != Sailfish::Secrets::Request::Status::Finished)
                return;

              if (ssr->result().code() == Sailfish::Secrets::Result::Failed)
                promise->reportResult(tl::make_unexpected(Error(
                    ssr->result().errorCode(), ssr->result().errorMessage())));
              else
                promise->reportResult(true);

              promise->reportFinished();
              ssr->deleteLater();
            });

        ssr->startRequest();
        deleteOldTokenWatcher->deleteLater();
      });

  return promise->future();
}

QFuture<tl::expected<QString, Secrets::Error>> SecretsSailfish::getAuthToken() {
  auto promise = QSharedPointer<
      QFutureInterface<tl::expected<QString, Secrets::Error>>>::create();
  promise->reportStarted();

  auto ssr = new Sailfish::Secrets::StoredSecretRequest();
  ssr->setManager(&m_secretManager);
  ssr->setIdentifier(m_authTokenIdentifier);
  ssr->setUserInteractionMode(
      Sailfish::Secrets::SecretManager::SystemInteraction);
  QObject::connect(
      ssr, &Sailfish::Secrets::StoredSecretRequest::statusChanged,
      [promise, ssr]() mutable {
        if (ssr->status() != Sailfish::Secrets::Request::Status::Finished)
          return;

        if (ssr->result().code() == Sailfish::Secrets::Result::Failed) {
          const auto err = tl::make_unexpected(
              Error(ssr->result().errorCode(), ssr->result().errorMessage()));
          promise->reportResult(err);
        } else {
          QString token = QString::fromUtf8(ssr->secret().data());
          promise->reportResult(token);
        }

        promise->reportFinished();
        ssr->deleteLater();
      });

  ssr->startRequest();

  return promise->future();
}

QFuture<tl::expected<bool, Secrets::Error>> SecretsSailfish::deleteAuthToken() {
  auto promise = QSharedPointer<
      QFutureInterface<tl::expected<bool, Secrets::Error>>>::create();
  promise->reportStarted();

  auto dsr = new Sailfish::Secrets::DeleteSecretRequest();
  dsr->setManager(&m_secretManager);
  dsr->setIdentifier(m_authTokenIdentifier);
  dsr->setUserInteractionMode(
      Sailfish::Secrets::SecretManager::SystemInteraction);

  QObject::connect(
      dsr, &Sailfish::Secrets::DeleteSecretRequest::statusChanged,
      [promise, dsr]() mutable {
        if (dsr->status() != Sailfish::Secrets::Request::Status::Finished)
          return;

        if (dsr->result().code() == Sailfish::Secrets::Result::Failed) {
          const auto error = tl::make_unexpected(
              Error(dsr->result().errorCode(), dsr->result().errorMessage()));
          promise->reportResult(error);
        } else
          promise->reportResult(true);

        promise->reportFinished();
      });

  dsr->startRequest();

  return promise->future();
}

void SecretsSailfish::q_deleteAuthToken() {
  auto watcher = new QFutureWatcher<tl::expected<bool, Secrets::Error>>(this);
  watcher->setFuture(deleteAuthToken());

  connect(watcher, &QFutureWatcherBase::finished, this, [watcher]() {
    if (watcher->result().has_value()) {
      qInfo() << "token deleted";
    } else {
      if (watcher->result().error().code == 40) {
        qInfo() << "token not saved";
      }
      qWarning() << "token delete failed: code = "
                 << watcher->result().error().code
                 << watcher->result().error().message;
    }

    watcher->deleteLater();
  });
}
