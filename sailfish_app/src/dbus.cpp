#include "dbus.h"
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

using namespace Immich;

DBus::DBus(QObject *parent) : QObject(parent) {
  auto connection = QDBusConnection::sessionBus();

  if (!connection.registerObject("/", this,
                                 QDBusConnection::ExportAllContents)) {
    qWarning() << "D-Bus: не удалось зарегистрировать объект на /:"
               << connection.lastError().message();
    return;
  }

  if (!connection.registerService(ServiceName())) {
    qDebug() << "D-Bus: Имя сервиса уже занято или ошибка:"
             << connection.lastError().message();
  }
}

void DBus::oAuth2CallbackUrl(const QString &url) {
  emit oAuth2CallbackUrlReceived(url);
  qDebug() << url;
}
