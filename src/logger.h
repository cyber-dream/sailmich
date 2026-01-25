#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QUuid>
// #include <QString>

class QMessageLogContext;
class QString;
class QMutex;

class MessageHandler {
  friend class LogWriter;

public:
  static void handle(QtMsgType type, const QMessageLogContext &context,
                     const QString &msg);

private:
  static QQueue<QString> logQueue;
  static QMutex logMutex;
};

// FIXME clear old logs
class LogWriter : public QObject {
  Q_OBJECT
public:
  // default interval - 2s
  LogWriter(int pInterval = 2000, QObject *parent = nullptr);
  ~LogWriter();

public slots:
  void flushQueue();

private:
  QString m_filePath;
  QTimer m_timer;
};

#endif // LOGGER_H
