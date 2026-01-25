#ifndef BOOLSTR_H
#define BOOLSTR_H
#include <QString>

inline static QString boolStr(const bool pBool) {
  return pBool ? "true" : "false";
}

#endif // BOOLSTR_H
