#include "authentication.h"

#include <QBuffer>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

using namespace Immich::Api::Authentication;

AuthenticationApi::AuthenticationApi(QObject *parent) : Base(parent) {}

Result::Result<ValidateTokenResponse>
AuthenticationApi::validateToken() const {
  QUrl loginUrl = apiUrl.resolved(QUrl("auth/validateToken"));
  Request req(loginUrl);

  const auto rep = nm->post(req, "");

  QEventLoop loop;
  QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  if (rep->error())
    return tl::make_unexpected(Result::Error(rep->errorString()));

  QByteArray ReplyText = rep->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(ReplyText);
  if (!doc.isObject())
    return tl::make_unexpected(Result::Error("json is not an object"));

  const auto validateTokenResponse =
      Authentication::ValidateTokenResponse(doc.object());
  if (!validateTokenResponse.isValid())
    return tl::make_unexpected(Result::Error("validateTokenResponse is not valid:" +
                        validateTokenResponse.errMessage()));

  return {validateTokenResponse};
}

Result::Result<LoginResponse>
AuthenticationApi::login(const QUrl &pAddress, const QString &pEmail,
                         const QString &pPassword) const {
  QUrl loginUrl = pAddress.resolved(QUrl("api/auth/login"));
  Request req(loginUrl);

  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject reqJson;
  reqJson["email"] = pEmail;
  reqJson["password"] = pPassword;
  QByteArray jsonData = QJsonDocument(reqJson).toJson(QJsonDocument::Compact);

  QBuffer *buffer = new QBuffer();
  buffer->setData(jsonData);
  buffer->open(QIODevice::ReadOnly);

  const auto rep = nm->post(req, buffer);

  QEventLoop loop;
  QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  if (rep->error())
    return tl::make_unexpected(Result::Error(rep->errorString()));

  QByteArray ReplyText = rep->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(ReplyText);
  if (!doc.isObject())
    return tl::make_unexpected(Result::Error("json is not an object"));

  const auto loginResponse = LoginResponse(doc.object());
  if (!loginResponse.isValid())
    return tl::make_unexpected(Result::Error("loginResponse is not valid: " +
                        loginResponse.errMessage()));

  return {loginResponse};
}

Result::Result<LogoutResponse> AuthenticationApi::logout() const {
  Request req(apiUrl.resolved(QUrl("auth/logout")));

  const auto rep = nm->post(req);

  QEventLoop loop;
  QObject::connect(rep, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  if (rep->error())
    return tl::make_unexpected(Result::Error(rep->errorString()));

  QByteArray ReplyText = rep->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(ReplyText);
  if (!doc.isObject())
    return tl::make_unexpected(Result::Error("json is not an object"));

  const auto logoutResponse = Authentication::LogoutResponse(doc.object());
  if (logoutResponse.isValid()) {
    return tl::make_unexpected(Result::Error("logoutResponse is not valid:" +
                        logoutResponse.errMessage()));
  }

  return {logoutResponse};
}
