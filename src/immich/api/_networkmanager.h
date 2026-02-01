#ifndef _NETWORKMANAGER_H
#define _NETWORKMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

namespace Immich {
namespace Api {

struct Request {
public:
  explicit Request(const QUrl &url = QUrl());

public:
  void setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value);

  QNetworkRequest getQRequest();
  void setAuthToken(const QByteArray &token); //TODO inline
  QTimer *getTimer();
  int timeout = 30000;

private:
  QNetworkRequest m_request;
  QTimer *m_timer;
};
// class Reply : public QObject {
//   Q_OBJECT
// public:
//   Reply(QNetworkReply *reply, QObject *parent = nullptr);

// private:
//   QNetworkReply *m_reply;
// };

class NetworkManager : QObject {
  Q_OBJECT
public:
  NetworkManager(QObject *parent = nullptr);

  void setAuthToken(const QString &);

  QNetworkReply *head(Request &request);
  QNetworkReply *get(Request &request);
  QNetworkReply *post(Request &request, QIODevice *data);
  QNetworkReply *post(Request &request, const QByteArray &data);
  QNetworkReply *post(Request &request);
  //  QNetworkReply *post(const QNetworkRequest &request,
  //                      QHttpMultiPart *multiPart);
  //    QNetworkReply *put(const QNetworkRequest &request, QIODevice *data);
  //    QNetworkReply *put(const QNetworkRequest &request, const QByteArray
  //    &data); QNetworkReply *put(const QNetworkRequest &request,
  //    QHttpMultiPart *multiPart); QNetworkReply *deleteResource(const
  //    QNetworkRequest &request); QNetworkReply *sendCustomRequest(const
  //    QNetworkRequest &request, const QByteArray &verb, QIODevice *data =
  //    Q_NULLPTR);
private:
  QNetworkAccessManager *nm;
  QByteArray token;
};
} // namespace Api
} // namespace Immich
#endif // _NETWORKMANAGER_H
