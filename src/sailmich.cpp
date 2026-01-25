#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include "logger.h"
#include <QGuiApplication>
#include <QQuickView>
#include <QStandardPaths>
#include <sailfishapp.h>
#include <src/immich/immich-plugin.h>

int main(int argc, char *argv[]) {
  qInstallMessageHandler(MessageHandler::handle);

  QGuiApplication *app = SailfishApp::application(argc, argv);
  app->setOrganizationName(QStringLiteral("ru.brzezinski"));
  //  app->setOrganizationDomain(QStringLiteral"ru.brzezinski");
  app->setApplicationName(QStringLiteral("sailmich"));

  QThread *logThread = new QThread(app);
  auto logger = new LogWriter(2000, logThread);
  QObject::connect(app, &QCoreApplication::aboutToQuit, logThread,
                   &QThread::quit);
  QObject::connect(logThread, &QThread::finished, logger,
                   &QObject::deleteLater);

  qmlRegisterSingletonType<Immich::ImmichPlugin>(
      "Immich", 1, 0, "Immich", Immich::ImmichPlugin::singletonProvider);

  const auto immichPlugin = Immich::ImmichPlugin::getSingleton();
  immichPlugin->registerTypes("Immich");
  immichPlugin->startInit();
  QQuickView *view = SailfishApp::createView();

  // view->engine()->addImageProvider("blurhash", new BlurHashImageProvider());
  // view->engine()->addImageProvider("thumbhash", new Immich::Thumbhash());
  // view->engine()->addImageProvider("imasset", new
  // Immich::AssetImageProvider(plugin->getSingleton()->getAssets()));
  view->setSource(SailfishApp::pathToMainQml());
  view->show();

  return app->exec();
}
