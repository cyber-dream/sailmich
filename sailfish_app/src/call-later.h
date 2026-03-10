#ifndef CALLLATER_H
#define CALLLATER_H

#include <QJSValue>
#include <QObject>
#include <QTimer>

class CallLater : public QObject {
  Q_OBJECT
public:
  explicit CallLater(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE void callLater(QJSValue callback) {
    if (!callback.isCallable())
      return;

    QTimer::singleShot(0, this, [callback]() mutable { callback.call(); });
  }
};

#endif // CALLLATER_H
