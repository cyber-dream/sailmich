#ifndef SECRETSBASE_H
#define SECRETSBASE_H

#include <QObject>
#include <src/immich/modules/_base.h>
#include <src/result/result.h>
namespace Immich {
namespace Module {
namespace Secrets {

class BaseSecretsModule : public BaseModule {
  Q_OBJECT
public:
  BaseSecretsModule(QObject *parent = nullptr) : BaseModule(parent) {}
  virtual ~BaseSecretsModule() = default;

  virtual  Result::Promise<QString> *getAuthToken() = 0;
  virtual  Result::Promise<bool> *storeAuthToken(const QString &pToken) = 0;
  virtual  Result::Promise<bool> *deleteAuthToken() = 0;
};
} // namespace Secrets
} // namespace Module
} // namespace Immich

#endif // SECRETSBASE_H
