#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QQmlEngine>
#include <QQmlContext>

#include "logger.h"

#include "dependencypointer.h"
#include "dependencypool.h"

#include <QGuiApplication>
#include <QLocale>
#include <QQuickView>
#include <QStandardPaths>
#include <QTranslator>
#include <QtDBus>
#include <sailfishapp.h>

#include <src/secrets-sf.h>

// #include <src/immich/modules/api-manager.h>
// #include <src/immich/modules/dbus.h>

#include <src/api-manager.h>
#include <src/dbus.h>
#include <src/settings.h>

#include <src/statefull-objects/so-auth-flow.h>
#include <src/statefull-objects/so-init-flow.h>

#include <src/fetchers/albums-list-model-fetcher.h>
#include <src/fetchers/timeline-model-fetcher.h>

#include <src/models/albums-list-model.h>
#include <src/models/media-view-model.h>
#include <src/models/timebucket-model.h>
#include <src/models/timeline-model.h>

#include <src/call-later.h>

#include <OAIQmlDeclares.h>

void showNotification(const QString &summary, const QString &body) {
  QDBusInterface iface(
      "org.freedesktop.Notifications", "/org/freedesktop/Notifications",
      "org.freedesktop.Notifications", QDBusConnection::sessionBus());

  if (!iface.isValid())
    return;

  QVariantMap hints;
  hints["x-nemo-display-on"] = true;
  hints["x-nemo-priority"] = 1;

  QList<QVariant> args;
  args << "Sailmich";           // app_name
  args << uint(0);              // replaces_id
  args << "icon-lock-sailmich"; // app_icon
  args << summary;              // summary
  args << body;                 // body
  args << QStringList();        // actions
  args << hints;                // hints
  args << int(-1);              // expire_timeout (ms)

  iface.callWithArgumentList(QDBus::NoBlock, "Notify", args);
}

Immich::Settings::Settings *settings;

int main(int pArgCount, char *pArgVector[]) {
  /* Logging override */
  qInstallMessageHandler(MessageHandler::handle);

  /* Arguments */
  if (pArgCount > 1) {

    const auto arg1 = QString(pArgVector[1]);

    if (arg1.startsWith("app.immich:/")) {
      if (QDBusConnection::sessionBus().interface()->isServiceRegistered(
              Immich::DBus::ServiceName())) {

        QDBusInterface remoteApp(Immich::DBus::ServiceName(), "/",
                                 Immich::DBus::InterfaceName());

        if (!remoteApp.isValid()) {
          showNotification("OAuth2 failed",
                           "App instance not found on dbus"); // TODO translate
          qCritical() << "OAuth2 failed" << "App instance not found on dbus";
          return 1;
        }

        remoteApp.call("oAuth2CallbackUrl", arg1);
        return 0;
      }

      showNotification(
          "OAuth2 failed",
          "Service not registered (Sailmich should be runned)"); // TODO
                                                                 // translate
    }
  }

  QGuiApplication *app = SailfishApp::application(pArgCount, pArgVector);
  app->setOrganizationName(QStringLiteral("ru.brzezinski"));
  //  app->setOrganizationDomain(QStringLiteral"ru.brzezinski");
  app->setApplicationName(QStringLiteral("sailmich"));

  /* Translations */
  {
    qDebug() << "Initializing translations";
    const QString translationsPath =
        SailfishApp::pathTo("translations").toLocalFile();

    const auto trDefault = new QTranslator(app);
    trDefault->load("sailmich-en_US", translationsPath);
    app->installTranslator(trDefault);

    QTranslator *trSystem = new QTranslator(app);
    if (trSystem->load(QLocale::system(), "sailmich", "-", translationsPath)) {
      app->installTranslator(trSystem);
      qInfo() << "System translation for" << QLocale::system().name()
              << "loaded";
    } else {
      qWarning() << "System translation for << NOT loaded, using fallback";
    }
  }

  /* Logs */
  qDebug() << "Initializing logs writer";
  //  QThread *logThread = new QThread(app);
  auto logger = new LogWriter(2000);
  //  QObject::connect(app, &QCoreApplication::aboutToQuit, logThread,
  //                   &QThread::quit);
  //  QObject::connect(logThread, &QThread::finished, logger,
  //                   &QObject::deleteLater);

  //  qmlRegisterSingletonType<LogWriter>(
  //      "Immich.Secrets", 1, 0, "Secrets",
  //      [logger](QQmlEngine *, QJSEngine *) -> QObject * {
  //        return  static_cast<QObject*>(logger);
  //      });

  /* Immich objects */
  {
    qInfo() << "Registering Immich objects in qml engine";

    /* DBus*/
    {
      QInjection::addSingleton(new Immich::DBus(app));
    }

    /* Settings */
    {
      QInjection::addSingleton(new Immich::Settings::Settings(app));
    }

    /* Api */
    {
      const auto apiManger = new Immich::ApiManager(app);
      QInjection::addSingleton<Immich::ApiManager>(apiManger);
    }

    /* Auth */
    {
      qmlRegisterType<Immich::SO::AuthFlow>("Immich.Auth", 1, 0, "SOAuthFlow");
      qRegisterMetaType<Immich::SO::AuthFlow *>("FlowActor::Auth::SOAuthFlow*");
    }

    /* Secrets */
    {
      QInjection::addSingleton(
          static_cast<Immich::Secrets *>(new Immich::SecretsSailfish(app)));
      qmlRegisterSingletonType<Immich::Secrets>(
          "Immich.Secrets", 1, 0, "Secrets",
          [](QQmlEngine *, QJSEngine *) -> QObject * {
            return QInjection::Pointer<Immich::Secrets>();
          });
    }

    /* Thumbnailer */
    {
      QInjection::addSingleton<Immich::Thumbnailer>(
          new Immich::Thumbnailer(app));
      qmlRegisterSingletonType<Immich::Thumbnailer>(
          "Immich.Thumbnailer", 1, 0, "Thumbnailer",
          [](QQmlEngine *, QJSEngine *) -> QObject * {
            return QInjection::Pointer<Immich::Thumbnailer>();
          });

      qRegisterMetaType<Immich::ThumbnailWatcher *>();
    }

    /* Timeline */
    {
      qmlRegisterType<Immich::Fetcher::TimelineModelFetcher>(
          "Immich.Timeline", 1, 0, "FeTimeline");

      qmlRegisterType<Immich::Fetcher::TimelineModelFetcherParams>(
          "Immich.Timeline", 1, 0, "FeTimelineParams");

      qmlRegisterType<Immich::Model::TimelineModel>("Immich.Timeline", 1, 0,
                                                    "ModTimeline");
      qRegisterMetaType<Immich::Model::TimelineModel *>();

      qmlRegisterUncreatableType<Immich::Model::TimebucketModel>(
          "Immich.Timeline", 1, 0, "ModTimebucket",
          "Created in c++ in TimelineModel only");
      qRegisterMetaType<Immich::Model::TimebucketModel *>();

      qRegisterMetaType<QVector<Immich::Model::TimebucketModel *>>();

      qmlRegisterType<Immich::Model::TimebucketModelAsset >(
          "Immich.Timeline", 1, 0, "ModTimebucketAsset");
      qRegisterMetaType<Immich::Model::TimebucketModelAsset *>();
    }

    /* Media View */
    {
      qmlRegisterType<Immich::Model::MediaViewModel>("Immich.MediaView", 1, 0,
                                                     "ModMediaView");
    }

    /* Albums */

    {
      QInjection::addSingleton<Immich::Fetcher::AlbumsListModelFetcher>(
          new Immich::Fetcher::AlbumsListModelFetcher(app));
      qmlRegisterType<Immich::Fetcher::AlbumsListModelFetcher>(
          "Immich.Album", 1, 0, "FeAlbumsList");

      qmlRegisterType<Immich::Fetcher::AlbumsListModelFetcherParams>(
          "Immich.Album", 1, 0, "FeAlbumsListParams");

      qmlRegisterType<Immich::Model::AlbumsListModel>("Immich.Album", 1, 0,
                                                      "ModAlbumsList");
      qRegisterMetaType<Immich::Model::AlbumsListModel *>();
      qRegisterMetaType<QVector<OpenAPI::OAIAlbumResponseDto>>();
    }

    /* Misc */
    {
      qRegisterMetaType<
          OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize>(); // TODO delete
    }
    /* Initialization */

    {
      Immich::SO::InitFlow *initFlow = new Immich::SO::InitFlow(app);
      QInjection::addSingleton(initFlow);
      //      initFlow->start();
      qmlRegisterSingletonType<Immich::SO::InitFlow>(
          "Immich.Init", 1, 0, "Init",
          [](QQmlEngine *, QJSEngine *) -> QObject * {
            return QInjection::Pointer<Immich::SO::InitFlow>();
          });
    }
  }

  /* QML */

  qDebug() << "Creating QQuickView";

  QQuickView *view = SailfishApp::createView();
  view->engine()->rootContext()->setContextProperty("Logger", logger);

  const auto callLater = new CallLater();
  view->engine()->rootContext()->setContextProperty("CallLater", callLater);

  // view->engine()->addImageProvider("blurhash", new BlurHashImageProvider());
  // view->engine()->addImageProvider("thumbhash", new Immich::Thumbhash());
  // view->engine()->addImageProvider("imasset", new
  // Immich::AssetImageProvider(plugin->getSingleton()->getAssets()));
  view->setSource(SailfishApp::pathToMainQml());
  view->show();

  return app->exec();
}
