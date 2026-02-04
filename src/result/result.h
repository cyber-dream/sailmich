#ifndef RESULT_H
#define RESULT_H

#include <QEventLoop>
#include <QMetaObject>
#include <QObject>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <atomic>
#include <functional>
#include <src/tl-expected.h>

namespace Result {
struct Error {
  Q_GADGET
  Q_PROPERTY(QString message MEMBER m_message)
public:
  explicit Error() : m_message("empty error") {};
  Error(const QString &err) : m_message(err) {};

  QString message() const { return m_message; }

private:
  QString m_message;
};

template <typename T> using Result = tl::expected<T, Error>;

class PromiseBase : public QObject {
  Q_OBJECT
public:
  explicit PromiseBase(QObject *parent = nullptr)
      : QObject(parent), m_isFinished(false) {}

  Q_INVOKABLE void waitForFinished() const {
    if (m_isFinished.load(std::memory_order_acquire))
      return;

    QEventLoop loop;
    QObject::connect(this, &PromiseBase::finished, &loop, &QEventLoop::quit,
                     Qt::QueuedConnection);

    loop.exec();
  };

  bool getIsFinished() const {
    return m_isFinished.load(std::memory_order_acquire);
  }

  virtual tl::expected<QVariant, Error> getVariantResult() const = 0;

protected:
  std::atomic<bool> m_isFinished;
signals:
  void finished();
};

template <typename T> class Promise final : public PromiseBase {
public:
  using Result = tl::expected<T, Error>;

  Promise(std::function<Result()> work, QThread *pTargetThread)
      : PromiseBase(nullptr),
        m_result(tl::make_unexpected(Error("result is empty stub"))) {
    if (pTargetThread)
      this->moveToThread(pTargetThread);
    doWork(work);
  }

  Promise(std::function<Result()> work, QObject *parent = nullptr)
      : PromiseBase(parent),
        m_result(tl::make_unexpected(Error("result is empty stub"))) {
    doWork(work);
  };

public:
  Result getResult() const {
    Q_ASSERT(m_isFinished.load(std::memory_order_acquire));
    return m_result;
  }

  virtual tl::expected<QVariant, Error> getVariantResult() const override {
    Q_ASSERT(m_isFinished.load(std::memory_order_acquire));

    if (!m_result.has_value()) {
      return tl::make_unexpected(m_result.error());
    }

    return QVariant::fromValue(m_result.value());
  }

private:
  void doWork(std::function<Result()> work) {
    auto ptr = QPointer<Promise<T>>(this);
    QTimer::singleShot(0, this, [ptr, work]() {
      if (!ptr) {
        qCritical() << "promise already deleted, work skipped!";
        return;
      }

      ptr->m_result = work();
      ptr->m_isFinished.store(true, std::memory_order_release);
      emit ptr->finished();
    });
  }

  Result m_result;
};

class PromiseVariant : public QObject {
  Q_OBJECT
public:
  explicit PromiseVariant(PromiseBase *pPromise)
      : QObject(pPromise), m_promise(pPromise) {
    // if (!m_promise) return;  TODO check null and deleted

    connect(m_promise, &PromiseBase::finished, this,
            &PromiseVariant::onWrappedPromiseFinished, Qt::QueuedConnection);

    if (pPromise->getIsFinished()) {
      onWrappedPromiseFinished();
      return;
    }
  }

  Q_INVOKABLE bool isFinished() { return m_promise->getIsFinished(); }

  Q_INVOKABLE bool isSucceeded() {
    if (!m_promise->getIsFinished()) {
      qWarning() << "promise not finished";
      return false;
    }

    return m_promise->getVariantResult().has_value();
  };

  Q_INVOKABLE QVariant data() const {
    if (!m_promise->getIsFinished() ||
        !m_promise->getVariantResult().has_value()) {
      return QVariant();
    };
    return m_promise->getVariantResult().value();
  }

  Q_INVOKABLE Error error() const {
    if (!m_promise->getIsFinished()) {
      return Error("promise not finished");
    };

    if (m_promise->getVariantResult().has_value()) {
      return Error("promise complete without error, this is stub error");
    }

    return m_promise->getVariantResult().error();
  }

signals:
  void finished();

public slots:
  void onWrappedPromiseFinished() { emit finished(); };

private:
  QPointer<PromiseBase> m_promise;
};

} // namespace Result
#endif // RESULT_H
