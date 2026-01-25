#include "logger.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QMessageLogContext>
#include <QMutex>
#include <QQueue>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>
#include <QUuid>
#include <src/utils/uuid-str.h>

QQueue<QString> MessageHandler::logQueue;
QMutex MessageHandler::logMutex;

void MessageHandler::handle(QtMsgType type, const QMessageLogContext &context,
                            const QString &msg) {
  const QString time =
      QDateTime::currentDateTime().toString("yy.MM.dd hh:mm:ss");
  QString typeStr;
  switch (type) {
  case QtDebugMsg:
    typeStr = "DBG";
    break;
  case QtInfoMsg:
    typeStr = "INF";
    break;
  case QtWarningMsg:
    typeStr = "WRN";
    break;
  case QtCriticalMsg:
    typeStr = "CRT";
    break;
  case QtFatalMsg:
    typeStr = "FTL";
    break;
  default:
    typeStr = "UNK";
  }

  QString fullMsg =
      QString("[%1] %2 | (%3) %4").arg(typeStr, time, context.category, msg);

  fprintf(stderr, "%s\n", qPrintable(fullMsg));
  fflush(stderr);

  {
    QMutexLocker locker(&logMutex);
    logQueue.enqueue(fullMsg);
  }
}

LogWriter::LogWriter(int pInterval, QObject *parent) : QObject(parent) {
  QString logsPath =
      QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/logs";
  QDir dir(logsPath);

  if (!dir.exists()) { // TODO fallback dir to logs
    if (dir.mkpath(".")) {
      //      qWarning() << "logs folder created";
    } else {
      //      qCritical() << "can't create logs folder"; // TODO write to tmp
      //      logsPath = "/tmp/sailmich-log.txt";
    }
  } else {
    //    qWarning() << "logs forlder exists";
  }

  m_filePath = (logsPath + "/" + uuidStr(QUuid::createUuid()) + ".txt");

  m_timer.setInterval(pInterval);
  connect(&m_timer, &QTimer::timeout, this, &LogWriter::flushQueue);
  m_timer.start();

  QTimer::singleShot(100, this,
                     [this, logsPath]() { this->cleanOldFiles(logsPath, 2); });
}

void LogWriter::cleanOldFiles(const QString &dirPath, int daysOld) {
  QDir dir(dirPath);
  if (!dir.exists())
    return;

  QDirIterator it(dirPath, QDir::Files | QDir::NoDotAndDotDot);
  QDateTime limitDate = QDateTime::currentDateTime().addDays(-daysOld);

  while (it.hasNext()) {
    it.next();
    QFileInfo fileInfo = it.fileInfo();

    if (fileInfo.lastModified() < limitDate) {
      if (QFile::remove(fileInfo.absoluteFilePath())) {
        qDebug() << "deleted old log file:" << fileInfo.fileName();
      } else {
        qDebug() << "can't delete log file:" << fileInfo.fileName();
      }
    }
  }
}

LogWriter::~LogWriter() {
  m_timer.stop();
  flushQueue();
}

void LogWriter::flushQueue() {
  QQueue<QString> tempQueue;

  {
    QMutexLocker locker(&MessageHandler::logMutex);
    if (MessageHandler::logQueue.isEmpty())
      return;
    tempQueue = MessageHandler::logQueue;
    MessageHandler::logQueue.clear();
  }

  QFile file(m_filePath);
  if (file.open(QIODevice::Append | QIODevice::Text)) {
    QTextStream out(&file);
    while (!tempQueue.isEmpty()) {
      out << tempQueue.dequeue() << endl;
    }
  }
}
