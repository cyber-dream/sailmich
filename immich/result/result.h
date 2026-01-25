#ifndef RESULT_H
#define RESULT_H

#include <QEventLoop>
#include <QObject>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <functional>

namespace Immich {

class ResultPromiseBase : public QObject {
  Q_OBJECT
public:
  enum ResultCode { Succeeded = 0, Pending = 1, Failed = 2 };
  Q_ENUM(ResultCode);

  explicit ResultPromiseBase(QObject *parent = nullptr) : QObject(parent) {}
signals:
  void finished();
};

struct ResultError {
  Q_GADGET
  Q_PROPERTY(QString m_message MEMBER m_message)
public:
  explicit ResultError() : m_message("empty error") {};
  ResultError(const QString &err) : m_message(err) {};

  QString message() const { return m_message; }
private:
  QString m_message;
};

template <typename T> class ResultErrored {
public:
  ResultErrored() {};
  ResultErrored(const T &pData) : m_code(ResultPromiseBase::Succeeded) {
    new (&m_data) T(pData);
  }

  ResultErrored(const ResultError &pErr) : m_code(ResultPromiseBase::Failed) {
    new (&m_error) ResultError(pErr);
  }

  ResultErrored &operator=(const ResultErrored &other) {
    if (this == &other)
      return *this;
    this->~ResultErrored();
    m_code = other.m_code;
    if (m_code == ResultPromiseBase::Succeeded)
      new (&m_data) T(other.m_data);
    else if (m_code == ResultPromiseBase::Failed)
      new (&m_error) ResultError(other.m_error);
    return *this;
  }

  ResultErrored(const ResultErrored &other) {
    m_code = other.m_code;
    if (m_code) {
      new (&m_error) ResultError(other.m_error);
    } else {
      new (&m_data) T(other.m_data);
    }
  }

  ResultErrored(ResultErrored &&other) noexcept {
    m_code = other.m_code;
    if (m_code) {
      new (&m_error) ResultError(std::move(other.m_error));
    } else {
      new (&m_data) T(std::move(other.m_data));
    }
  }

  ~ResultErrored() {
    if (m_code == ResultPromiseBase::ResultCode::Succeeded)
      m_data.~T();
    else if (m_code == ResultPromiseBase::ResultCode::Failed)
      m_error.~ResultError();
  }

  ResultError error() const {
    Q_ASSERT(m_code == ResultPromiseBase::Failed);
    return m_error;
  };

  T data() const {
    Q_ASSERT(m_code == ResultPromiseBase::Succeeded);
    return m_data;
  };

  ResultPromiseBase::ResultCode code() const { return m_code; };
  bool isSucceeded() const {return m_code == ResultPromiseBase::Succeeded;};

  ResultErrored<QVariant> toVariant() const {
    Q_ASSERT(m_code != ResultPromiseBase::Pending);

    if (m_code == ResultPromiseBase::Succeeded) {
      return ResultErrored<QVariant>(QVariant::fromValue(m_data));
    }

    return ResultErrored<QVariant>(m_error);
  }

private:
  ResultPromiseBase::ResultCode m_code = ResultPromiseBase::Pending;

  union {
    T m_data;
    ResultError m_error;
  };
};

class ResultVariantPromise : public ResultPromiseBase {
  Q_OBJECT
public:
  explicit ResultVariantPromise(std::function<ResultErrored<QVariant>()> work,
                                QObject *parent = nullptr)
      : ResultPromiseBase(parent) {
    this->finish(work());
  };

  Q_INVOKABLE void waitForFinished() const {
    QEventLoop loop;
    QObject::connect(this, &ResultVariantPromise::finished, &loop,
                     &QEventLoop::quit);
    loop.exec();
  };

  bool getIsFinished() { return m_isFinished; }

  ResultCode getCode() {
    if (!m_isFinished) {
      return Pending;
    }

    return m_result.code();
  }

  QVariant getData() {
    Q_ASSERT(m_isFinished);

    return m_result.data();
  }

  QVariant getDataVariant() { return QVariant(getData()); }

  ResultError getError() {
    Q_ASSERT(m_isFinished);

    return m_result.error();
  }
signals:
  void finished();
  void finishedError(ResultError);
  void finishedSuccess(QVariant);

private:
  void finish(const ResultErrored<QVariant> &result) {
    QTimer::singleShot(0, this, [this, result]() {
      m_result = result;
      m_isFinished = true;
      emit finished();
      if (result.code() == Succeeded) {
        emit finishedSuccess(m_result.data());
      } else {
        emit finishedError(m_result.error());
      }
    });
  }

  ResultErrored<QVariant> m_result;
  bool m_isFinished;

  Q_PROPERTY(ResultCode code READ getCode NOTIFY finished)
  Q_PROPERTY(QVariant data READ getDataVariant NOTIFY finished)
  Q_PROPERTY(ResultError error READ getError NOTIFY finished)
  Q_PROPERTY(bool isFinished READ getIsFinished NOTIFY finished)
};

template <typename T> class ResultPromise : public ResultPromiseBase {
public:
//    ResultPromise(QObject *parent = nullptr): ResultPromiseBase(parent) {};
  explicit ResultPromise(std::function<ResultErrored<T>()> work,
                         QObject *parent = nullptr)
      : ResultPromiseBase(parent), m_work(work) {
    this->finish(m_work());
  };

  Q_INVOKABLE void waitForFinished() const {
    QEventLoop loop;
    QObject::connect(this, &ResultPromiseBase::finished, &loop,
                     &QEventLoop::quit);
    loop.exec();
  };

  //  ResultErrored<T> getResult() const { return m_result; };
  bool getIsFinished() { return m_isFinished; }

  ResultCode getCode() {
    if (!m_isFinished) {
      return Pending;
    }

    return m_result.code();
  }

  ResultErrored<T> getResult() {
    Q_ASSERT(m_isFinished);
    return m_result;
  }

private:
  void finish(const ResultErrored<T> &result) {
    QTimer::singleShot(0, this, [this, result]() {
      m_result = result;
      m_isFinished = true;
      emit finished();
    });
  }

  std::function<ResultErrored<T>()> m_work;
  ResultErrored<T> m_result;
  bool m_isFinished;
};

} // namespace Immich
#endif // RESULT_H
