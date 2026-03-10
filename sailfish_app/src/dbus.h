#ifndef DBUS_H
#define DBUS_H

#include <QObject>
namespace Immich {
class DBus final : public QObject {
  Q_OBJECT
  Q_CLASSINFO("D-Bus Interface", "ru.brzezinski.sailmich.DBusHandler")
public:
  explicit DBus(QObject *parent = nullptr);
  static inline QLatin1String ServiceName() {
    return QLatin1String("ru.brzezinski.sailmich");
  };
  static inline QLatin1String InterfaceName() {
    return QLatin1String("ru.brzezinski.sailmich.DBusHandler");
  }
public slots:
  void oAuth2CallbackUrl(const QString &url);
signals:
  void oAuth2CallbackUrlReceived(const QString &url);
signals:
};
} // namespace Immich
#endif // DBUS_H
