#ifndef RESULT_H
#define RESULT_H

#include <QEventLoop>
#include <QMetaObject>
#include <QObject>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <atomic>
#include <functional>
// TODO move to main namespace
namespace Result {

class PromiseBase : public QObject {
  Q_OBJECT
public:
  //  enum ResultCode { Failed = 0, Succeced = 1 };
  //  Q_ENUM(ResultCode);

  explicit PromiseBase(QObject *parent = nullptr) : QObject(parent) {}

protected:
  std::atomic<bool> m_isFinished;
signals:
  void finished();
};

struct Error {
  Q_GADGET
  Q_PROPERTY(QString m_message MEMBER m_message)
public:
  explicit Error() : m_message("empty error") {};
  Error(const QString &err) : m_message(err) {};

  QString message() const { return m_message; }

private:
  QString m_message;
};

template <typename T> class Result {
public:
  //  Result()
  //      : m_status(StatusFailed),
  //        m_error("result was created as empty object without real error or
  //        data") {};
  Result(const T &pData) : m_status(StatusSucceed), m_data(T(pData)) {};

  Result(const Error &pErr) : m_status(StatusFailed), m_error(Error(pErr)) {};

  Result &operator=(const Result &other) {
    if (this == &other)
      return *this;
    this->~Result();

    new (this) Result(other);
    return *this;
  }

  Result(const Result &other) : m_status(other.m_status) {
    switch (m_status) {
    case Result::Result::StatusFailed:
      new (&m_error) Error(other.m_error);
      break;
    case Result::Result::StatusSucceed:
      new (&m_data) T(other.m_data);
      break;
    }
  }

  Result(Result &&other) : m_status(other.m_status) {
    switch (m_status) {
    case Result::Result::StatusFailed:
      new (&m_error) Error(std::move(other.m_error));
      break;
    case Result::Result::StatusSucceed:
      new (&m_data) T(std::move(other.m_data));
      break;
    }
  }

  ~Result() {
    switch (m_status) {
    case Result::Result::StatusFailed:
      m_error.~Error();
      break;
    case Result::Result::StatusSucceed:
      m_data.~T();
      break;
    }
  }

  Error error() const {
    Q_ASSERT(m_status == Result::Result::StatusFailed);
    return m_error;
  };

  T data() const {
    Q_ASSERT(m_status == Result::Result::StatusSucceed);
    return m_data;
  };

  bool isSucceeded() const {
    return m_status == Result::Result::StatusSucceed ? true : false;
  };

  Result<QVariant> toVariant() const {
    switch (m_status) {
    case Result::Result::StatusFailed:
      return Result<QVariant>(m_error);
    case Result::Result::StatusSucceed:
      return Result<QVariant>(QVariant::fromValue(m_data));
    default:
      qCritical() << "returning empty qWariant result";
      return Result<QVariant>(QVariant());
    }
  }

private:
  enum Status { StatusFailed, StatusSucceed };
  Status m_status;
  union {
    T m_data;
    Error m_error;
  };
};

class PromiseVariant : public PromiseBase {
  Q_OBJECT
public:
  explicit PromiseVariant(std::function<Result<QVariant>()> work,
                          QObject *parent = nullptr)
      : PromiseBase(parent), m_result(Error("result is empty stub")) {
    this->finish(work());
  };

  Q_INVOKABLE void waitForFinished() const {
    QEventLoop loop;
    QObject::connect(this, &PromiseVariant::finished, &loop, &QEventLoop::quit);
    loop.exec();
  };

  bool getIsFinished() { return m_isFinished; }

  QVariant getData() {
    Q_ASSERT(m_isFinished);

    return m_result.data();
  }

  QVariant getDataVariant() { return QVariant(getData()); }

  Error getError() {
    Q_ASSERT(m_isFinished);

    return m_result.error();
  }
signals:
  void finished();
  void finishedError(Error);
  void finishedSuccess(QVariant);

private:
  void finish(const Result<QVariant> &result) {
    QTimer::singleShot(0, this, [this, result]() {
      m_result = result;
      m_isFinished = true;
      emit finished();
      if (result.isSucceeded()) {
        emit finishedSuccess(m_result.data());
      } else {
        emit finishedError(m_result.error());
      }
    });
  }

  Result<QVariant> m_result;
  //  bool m_isFinished;

  //  Q_PROPERTY(ResultCode code READ getCode NOTIFY finished)
  Q_PROPERTY(QVariant data READ getDataVariant NOTIFY finished)
  Q_PROPERTY(Error error READ getError NOTIFY finished)
  Q_PROPERTY(bool isFinished READ getIsFinished NOTIFY finished)
};

template <typename T> class Promise final : public PromiseBase {
public:
  Promise(std::function<Result<T>()> work, QThread *pTargetThread)
      : PromiseBase(nullptr), m_result(Error("result is empty stub")) {
    if (pTargetThread)
      this->moveToThread(pTargetThread);
    doWork(work);
  }

  Promise(std::function<Result<T>()> work, QObject *parent = nullptr)
      : PromiseBase(parent), m_result(Error("result is empty stub")) {
    doWork(work);
  };

public:
  Q_INVOKABLE void waitForFinished() const {
    QEventLoop loop;
    QObject::connect(this, &PromiseBase::finished, &loop, &QEventLoop::quit,
                     Qt::QueuedConnection);

    if (m_isFinished)
      return;

    loop.exec();
  };

  bool getIsFinished() const { return m_isFinished; }

  Result<T> getResult() const {
    Q_ASSERT(m_isFinished);
    return m_result;
  }

private:
  void doWork(std::function<Result<T>()> work) {
    auto ptr = QPointer<Promise<T>>(this);
    QTimer::singleShot(0, this, [ptr, work]() {
      if (!ptr) {
        qCritical() << "promise already deleted, work skipped!";
        return;
      }

      ptr->m_result = work();
      ptr->m_isFinished = true;
      emit ptr->finished();
    });
  }

  Result<T> m_result;
};

} // namespace Result
#endif // RESULT_H
