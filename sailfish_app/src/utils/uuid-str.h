#ifndef UUIDSTR_H
#define UUIDSTR_H
#include <QString>
#include <QUuid>

inline static QString uuidStr(const QUuid &uuid) {
  return uuid.toString().replace("{", "").replace("}", "");
}

#endif // UUIDSTR_H
