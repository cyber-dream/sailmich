#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "_base-api.h"
#include <QObject>
#include <src/result/result.h>
#include "dto/_authentication.h"

namespace Immich {
namespace Api {
namespace Authentication {

class AuthenticationApi : public Base {
  Q_OBJECT
public:
  explicit AuthenticationApi(QObject *parent = nullptr);

  Result::Result<ValidateTokenResponse> validateToken() const;

  Result::Result<LoginResponse> login(const QUrl &pAddress,
                                     const QString &pEmail,
                                     const QString &pPassword) const;
  Result::Result<LogoutResponse> logout() const;

signals:
};
} // namespace Authentication
} // namespace Api
} // namespace Immich
#endif // AUTHENTICATION_H
