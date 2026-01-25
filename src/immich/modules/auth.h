#ifndef AUTH_H
#define AUTH_H

#include "_base.h"
#include <src/immich/api/dto/_authentication.h>
#include <src/result/result.h>

namespace Immich {
namespace Module {
namespace Auth {

struct LoginResponse : public Immich::Api::Authentication::LoginResponse {
  Q_GADGET

  Q_PROPERTY(QUrl baseUrl MEMBER baseUrl)
  Q_PROPERTY(QUrl apiUrl READ apiUrl)
public:
  LoginResponse() = default;
  explicit LoginResponse(
      QUrl pBaseUrl, const Immich::Api::Authentication::LoginResponse &parent)
      : Api::Authentication::LoginResponse(parent), baseUrl(pBaseUrl) {}
  QUrl baseUrl;
  QUrl apiUrl() const { return baseUrl.resolved(QUrl("api")); };
};

class Auth : public BaseModule {
  Q_OBJECT
public:
  explicit Auth(QObject *parent = nullptr);

  Result::Promise<bool> *validateToken();

   Result::Promise<LoginResponse> *login(const QString &pAddress,
                                      const QString &pEmail,
                                      const QString &pPassword);

   Result::Promise<Api::Authentication::LogoutResponse> *logout();

  Q_INVOKABLE Result::PromiseVariant *q_login(const QString &pAddress,
                                            const QString &pEmail,
                                            const QString &pPassword);

  Q_INVOKABLE Result::PromiseVariant *q_logout();
signals:
  void loginFinished(const Result::Result<LoginResponse> &response);
  void logoutFinished(
      const Result::Result<Api::Authentication::LogoutResponse> &response);
};
} // namespace Auth
} // namespace Module
} // namespace Immich
Q_DECLARE_METATYPE(Immich::Module::Auth::LoginResponse);

#endif // AUTH_H
