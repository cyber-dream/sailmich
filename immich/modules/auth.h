#ifndef AUTH_H
#define AUTH_H

#include "_base-module.h"
#include <immich/api/dto/_authentication.h>
#include <immich/result/result.h>

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

  ResultPromise<bool> *validateToken();

  ResultPromise<LoginResponse> *login(const QString &pAddress,
                                      const QString &pEmail,
                                      const QString &pPassword);

  ResultPromise<Api::Authentication::LogoutResponse> *logout();

  Q_INVOKABLE ResultVariantPromise *q_login(const QString &pAddress,
                                            const QString &pEmail,
                                            const QString &pPassword);

  Q_INVOKABLE ResultVariantPromise *q_logout();
signals:
  void loginFinished(const ResultErrored<LoginResponse> &response);
  void logoutFinished(
      const ResultErrored<Api::Authentication::LogoutResponse> &response);
};
} // namespace Auth
} // namespace Module
} // namespace Immich
Q_DECLARE_METATYPE(Immich::Module::Auth::LoginResponse);

#endif // AUTH_H
