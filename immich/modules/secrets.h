#ifndef SECRETS_H
#define SECRETS_H

#include <QObject>
#include <Secrets/secret.h>

namespace Immich {
namespace Module {
namespace Secrets {
class Secrets : public QObject {
  Q_OBJECT
public:
  template <typename T> struct ResultWithErr {
    T result;
    QString err;
  };
  explicit Secrets(QObject *parent = nullptr);

  enum CollectionExistsResult {
    NotExists,
    Exists,
    Error,
  };

  ResultWithErr<QString> getAuthToken();
  void storeAuthToken(const QString &pToken);
  QString deleteAuthToken();

signals:
protected:
private:
  Sailfish::Secrets::Secret::Identifier m_authTokenIdent;
};
} // namespace Secrets
} // namespace Module
} // namespace Immich
#endif // SECRETS_H
