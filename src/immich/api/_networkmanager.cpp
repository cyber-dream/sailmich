#include "_networkmanager.h"

#include <QPointer>

Immich::Api::NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), nm(new QNetworkAccessManager(this)) {}

void Immich::Api::NetworkManager::setAuthToken(const QString &pToken) {
  token = pToken.toUtf8();
}

QNetworkReply *Immich::Api::NetworkManager::head(Request &request) {
  request.setAuthToken(token);
  request.getTimer()->start(request.timeout);

  QPointer<QNetworkReply> safeReply = nm->head(request.getQRequest());

  connect(request.getTimer(), &QTimer::timeout, [safeReply]() {
    if (!safeReply)
      return;
    if (!safeReply->isFinished())
      safeReply->abort();

    safeReply->deleteLater();
  });

  return safeReply;
}

QNetworkReply *Immich::Api::NetworkManager::get(Request &request) {
  request.setAuthToken(token);
  request.getTimer()->start(request.timeout);

  QPointer<QNetworkReply> safeReply = nm->get(request.getQRequest());

  connect(request.getTimer(), &QTimer::timeout, [safeReply]() {
    if (!safeReply)
      return;
    if (!safeReply->isFinished())
      safeReply->abort();

    safeReply->deleteLater();
  });

  return safeReply;
}

QNetworkReply *Immich::Api::NetworkManager::post(Request &request,
                                                 QIODevice *data) {
  request.setAuthToken(token);
  request.getTimer()->start(request.timeout);

  QPointer<QNetworkReply> safeReply = nm->post(request.getQRequest(), data);

  connect(request.getTimer(), &QTimer::timeout, [safeReply]() {
    //    qWarning() << "reply aborted";
    if (!safeReply)
      return;
    if (!safeReply->isFinished())
      safeReply->abort();

    safeReply->deleteLater();
  });

  return safeReply;
}

QNetworkReply *Immich::Api::NetworkManager::post(Request &request,
                                                 const QByteArray &data) {
  request.setAuthToken(token);
  request.getTimer()->start(request.timeout);

  QNetworkReply *reply = nm->post(request.getQRequest(), data);

  //  connect(request.getTimer(), &QTimer::timeout, [&reply]() { reply->abort();
  //  });

  return reply;
}

QNetworkReply *Immich::Api::NetworkManager::post(Request &request) {
  return post(request, {});
};

Immich::Api::Request::Request(const QUrl &url)
    : m_request(url), m_timer(new QTimer()) {}

void Immich::Api::Request::setHeader(QNetworkRequest::KnownHeaders header,
                                     const QVariant &value) {
  m_request.setHeader(header, value);
}

QNetworkRequest Immich::Api::Request::getQRequest() { return m_request; }

QTimer *Immich::Api::Request::getTimer() { return m_timer; }

void Immich::Api::Request::setAuthToken(const QByteArray &token) {
  m_request.setRawHeader("x-immich-user-token", token);
}

// Immich::Api::Reply::Reply(QNetworkReply *reply, QObject *parent)
//     : QObject(parent), m_reply(reply) {}
