#ifndef _AUTHENTICATION_H
#define _AUTHENTICATION_H

#include "_dto.h"
#include <QObject>

namespace Immich {
namespace Api {
namespace Authentication {

struct LoginResponse : public DtoStruct {
  Q_GADGET
  Q_PROPERTY(QString accessToken MEMBER accessToken)
  Q_PROPERTY(QString name MEMBER name)
  Q_PROPERTY(QString profileImagePath MEMBER profileImagePath)
  Q_PROPERTY(QString userEmail MEMBER userEmail)
  Q_PROPERTY(QString userId MEMBER userId)
  Q_PROPERTY(bool isAdmin MEMBER isAdmin)
  Q_PROPERTY(bool isOnboarded MEMBER isOnboarded)
  Q_PROPERTY(bool shouldChangePassword MEMBER shouldChangePassword)

public:
  LoginResponse() = default;
  explicit LoginResponse(const QJsonObject &obj);

  QString accessToken;
  QString name;
  QString profileImagePath;
  QString userEmail;
  QString userId;

  bool isAdmin = false;
  bool isOnboarded = false;
  bool shouldChangePassword = false;
};

struct LogoutResponse : public DtoStruct {
  Q_GADGET
  Q_PROPERTY(QUrl redirectUri MEMBER redirectUri)
  Q_PROPERTY(bool successful MEMBER successful)
public:
  LogoutResponse() = default;
  explicit LogoutResponse(const QJsonObject &obj);
  QUrl redirectUri;
  bool successful;
};

struct ValidateTokenResponse : public DtoStruct {
  Q_GADGET
  Q_PROPERTY(bool authStatus MEMBER authStatus)

public:
  ValidateTokenResponse() = default;
  explicit ValidateTokenResponse(const QJsonObject &obj);

  bool authStatus;
};

} // namespace Authentication
} // namespace Api
} // namespace Immich
Q_DECLARE_METATYPE(Immich::Api::Authentication::LoginResponse);
Q_DECLARE_METATYPE(Immich::Api::Authentication::LogoutResponse);
#endif // _AUTHENTICATION_H
