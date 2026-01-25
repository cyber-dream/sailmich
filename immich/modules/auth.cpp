#include "auth.h"

#include <QDebug>
#include <immich/api/authentication.h>

using namespace Immich::Module::Auth;
Auth::Auth(QObject *parent) : BaseModule(parent) {}

Immich::ResultPromise<bool> *Auth::validateToken() {
  return new ResultPromise<bool>([]() {
    const auto &apiResp = api.authentication->validateToken();
    ResultErrored<bool> res;
    if (apiResp.code() == ResultPromiseBase::ResultCode::Failed)
      res = ResultErrored<bool>(apiResp.error());
    else
      res = ResultErrored<bool>(apiResp.data().authStatus);

    return res;
  });
}

Immich::ResultPromise<LoginResponse> *Auth::login(const QString &pAddress,
                                          const QString &pEmail,
                                          const QString &pPassword) {
  return new ResultPromise<LoginResponse>([this, pAddress, pEmail,
                                           pPassword]() {
    const auto &apiResp =
        api.authentication->login(pAddress, pEmail, pPassword);

    ResultErrored<LoginResponse> res;
    if (apiResp.code() == ResultPromiseBase::ResultCode::Failed) {
      res = ResultErrored<LoginResponse>(apiResp.error());
    } else {
      res =
          ResultErrored<LoginResponse>(LoginResponse(pAddress, apiResp.data()));
    }

    emit loginFinished(res);

    return res;
  });
}

Immich::ResultPromise<Immich::Api::Authentication::LogoutResponse> *Auth::logout() {
  return new ResultPromise<Api::Authentication::LogoutResponse>([this]() {
    const auto &apiResp = api.authentication->logout();

    ResultErrored<Api::Authentication::LogoutResponse> res;

    if (apiResp.code() == ResultPromiseBase::ResultCode::Failed) {
      res = ResultErrored<Api::Authentication::LogoutResponse>(
          apiResp.error());
    } else {
      res = ResultErrored<Api::Authentication::LogoutResponse>(
          apiResp.data());
    }

    emit logoutFinished(res);
    return res;
  });
}

Immich::ResultVariantPromise *Auth::q_login(const QString &pAddress,
                                    const QString &pEmail,
                                    const QString &pPassword) {
  return new ResultVariantPromise([this, pAddress, pEmail, pPassword]() {
    const auto &resp = login(pAddress, pEmail, pPassword);
    resp->waitForFinished();
    return resp->getResult().toVariant();
  });
}

Immich::ResultVariantPromise *Auth::q_logout() {
  return new ResultVariantPromise([this]() {
    const auto &resp = logout();
    resp->waitForFinished();
    return resp->getResult().toVariant();
  });
}
