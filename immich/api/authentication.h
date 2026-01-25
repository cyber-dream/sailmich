#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "_base-api.h"
#include <QObject>
#include <immich/result/result.h>
#include "dto/_authentication.h"

namespace Immich {
namespace Api {
namespace Authentication {

class AuthenticationApi : public Base {
  Q_OBJECT
public:
  explicit AuthenticationApi(QObject *parent = nullptr);

  ResultErrored<ValidateTokenResponse> validateToken() const;

  ResultErrored<LoginResponse> login(const QUrl &pAddress,
                                     const QString &pEmail,
                                     const QString &pPassword) const;
  ResultErrored<LogoutResponse> logout() const;

signals:
};
} // namespace Authentication
} // namespace Api
} // namespace Immich
#endif // AUTHENTICATION_H
