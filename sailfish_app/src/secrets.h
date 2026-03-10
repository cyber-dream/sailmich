#ifndef SECRETSBASE_H
#define SECRETSBASE_H

#include <QFuture>
#include <QObject>
#include <expected.hpp>

namespace Immich {
class Secrets : public QObject {
  Q_OBJECT
public:
  struct Error {
    Error(int pCode, const QString &pMessage)
        : code(pCode), message(pMessage) {};
    int code;
    QString message;
  };

  Secrets(QObject *parent = nullptr) : QObject(parent) {}
  virtual ~Secrets() = default;

  virtual QFuture<tl::expected<QString, Error>> getAuthToken() = 0;
  virtual QFuture<tl::expected<bool, Error>>
  storeAuthToken(const QString &pToken) = 0;
  virtual QFuture<tl::expected<bool, Error>> deleteAuthToken() = 0;
//  virtual QFuture<tl::expected<bool, Error>> checkAuthTokenExists() = 0;

  Q_INVOKABLE virtual void q_deleteAuthToken() = 0;
};
} // namespace Immich

#endif // SECRETSBASE_H
