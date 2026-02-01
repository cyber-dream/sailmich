#include "auth.h"

#include <QDebug>
#include <src/immich/api/authentication.h>

using namespace Immich::Module::Auth;
Auth::Auth(QObject *parent) : BaseModule(parent) {}

Result::Promise<bool> *Auth::validateToken() {
  return new Result::Promise<bool>([]() -> Result::Result<bool> {
    const auto &apiResp = api.authentication->validateToken();

    if (!apiResp.has_value())
      return tl::make_unexpected(apiResp.error());

    return {apiResp.value().authStatus};
  });
}

Result::Promise<LoginResponse> *Auth::login(const QString &pAddress,
                                            const QString &pEmail,
                                            const QString &pPassword) {
  return new Result::Promise<LoginResponse>(
      [this, pAddress, pEmail, pPassword]() -> Result::Result<LoginResponse> {
        const auto &apiResp =
            api.authentication->login(pAddress, pEmail, pPassword);

        if (!apiResp.has_value()) {
          if (apiResp.error().message() == "Host requires authentication") {
            const auto res =
                tl::make_unexpected(Result::Error("Wrong credentials"));
            emit loginFinished(res);
            return res;
          }
          const auto res = tl::make_unexpected(apiResp.error());
          emit loginFinished(res);
          return res;
        } else {
          const auto lr = LoginResponse(pAddress, apiResp.value());
          const auto res = Result::Result<LoginResponse>(lr);
          emit loginFinished(res);
          return res;
        }
      });
}

Result::Promise<Immich::Api::Authentication::LogoutResponse> *Auth::logout() {
  return new Result::Promise<Api::Authentication::LogoutResponse>(
      [this]() -> Result::Result<Immich::Api::Authentication::LogoutResponse> {
        const auto &apiResp = api.authentication->logout();

        if (!apiResp.has_value()) {
          const auto res = tl::make_unexpected(apiResp.error());
          emit logoutFinished(res);
          return res;
        }

        const auto res =
            Result::Result<Immich::Api::Authentication::LogoutResponse>(
                apiResp.value());
        emit logoutFinished(res);
        return {res};
      });
}

Result::PromiseVariant *Auth::q_login(const QString &pAddress,
                                      const QString &pEmail,
                                      const QString &pPassword) {
  return new Result::PromiseVariant(login(pAddress, pEmail, pPassword));
}

Result::PromiseVariant *Auth::q_logout() {
  return new Result::PromiseVariant(logout());
}
