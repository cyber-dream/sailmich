#include "_authentication.h"
using namespace Immich::Api::Authentication;

ValidateTokenResponse::ValidateTokenResponse(const QJsonObject &obj) {
  *this = fromJson<ValidateTokenResponse>(obj);
}

LogoutResponse::LogoutResponse(const QJsonObject &obj) {
  *this = fromJson<LogoutResponse>(obj);
}

LoginResponse::LoginResponse(const QJsonObject &obj) {
  *this = fromJson<LoginResponse>(obj);
}
