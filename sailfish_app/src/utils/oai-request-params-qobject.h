#ifndef OAIREQUESTPARAMSQOBJECT_H
#define OAIREQUESTPARAMSQOBJECT_H

#include <OAIHelpers.h>

namespace OpenAPI {

class OAIRequestParams {
protected:
  template <typename T>
  inline bool isEqual(const OpenAPI::OptionalParam<T> &a, const T &b) const {
    return (a.hasValue() && a.value() == b) || (!a.hasValue() && b == T());
  };

  template <typename T, typename E>
  bool isEnumEqual(const OpenAPI::OptionalParam<T> &param,
                   const E &newValue) const {
    return param.hasValue() && param.value().getValue() == newValue;
  }

  template <typename T, typename E>
  void updateOptionalEnum(OpenAPI::OptionalParam<T> &param,
                          const E &newValue) const {
    T obj;
    obj.setValue(newValue);
    param = obj;
  }

  template <typename T>
  inline T valueOrDefault(const OpenAPI::OptionalParam<T> &param) const {
    return param.hasValue() ? param.value() : T();
  }

  template <typename T>
  inline T valueOrEnumDefault(const OpenAPI::OptionalParam<T> &param) const {
    return param.hasValue() ? param.value() : T();
  }
};
} // namespace OpenAPI
#endif // OAIREQUESTPARAMSQOBJECT_H
