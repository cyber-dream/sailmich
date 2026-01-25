#include "auth.h"

#include <QDebug>
#include <src/immich/api/authentication.h>

using namespace Immich::Module::Auth;
Auth::Auth(QObject *parent) : BaseModule(parent) {}

Result::Promise<bool> *Auth::validateToken() {
  return new Result::Promise<bool>([]() -> Result::Result<bool> {
    const auto &apiResp = api.authentication->validateToken();

    if (!apiResp.isSucceeded())
      return {apiResp.error()};

    return {apiResp.data().authStatus};
  });
}

Result::Promise<LoginResponse> *Auth::login(const QString &pAddress,
                                            const QString &pEmail,
                                            const QString &pPassword) {
  return new Result::Promise<LoginResponse>(
      [this, pAddress, pEmail, pPassword]() {
        const auto &apiResp =
            api.authentication->login(pAddress, pEmail, pPassword);

        if (!apiResp.isSucceeded()) {
          const auto res = Result::Result<LoginResponse>(apiResp.error());
          emit loginFinished(res);
          return res;
        } else {
          const auto res = Result::Result<LoginResponse>(
              LoginResponse(pAddress, apiResp.data()));
          return res;
        }
      });
}

Result::Promise<Immich::Api::Authentication::LogoutResponse> *Auth::logout() {
  return new Result::Promise<Api::Authentication::LogoutResponse>([this]() {
    const auto &apiResp = api.authentication->logout();

    if (!apiResp.isSucceeded()) {
      const auto res =
          Result::Result<Api::Authentication::LogoutResponse>(apiResp.error());
      emit logoutFinished(res);
      return res;
    }

    const auto res =
        Result::Result<Api::Authentication::LogoutResponse>(apiResp.data());
    emit logoutFinished(res);
    return res;
  });
}

Result::PromiseVariant *Auth::q_login(const QString &pAddress,
                                      const QString &pEmail,
                                      const QString &pPassword) {
  return new Result::PromiseVariant([this, pAddress, pEmail, pPassword]() {
    const auto &resp = login(pAddress, pEmail, pPassword);
    resp->waitForFinished();
    return resp->getResult().toVariant();
  });
}

Result::PromiseVariant *Auth::q_logout() {
  return new Result::PromiseVariant([this]() {
    const auto &resp = logout();
    resp->waitForFinished();
    return resp->getResult().toVariant();
  });
}
