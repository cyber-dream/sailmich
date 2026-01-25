#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QGuiApplication>
#include "logger.h"
#include <QStandardPaths>
#include <immich/immich-plugin.h>
#include <sailfishapp.h>
#include <QQuickView>


int main(int argc, char *argv[]) {
  qInstallMessageHandler(MessageHandler::handle);


  QGuiApplication *app = SailfishApp::application(argc, argv);
  app->setOrganizationName(QStringLiteral("ru.brzezinski"));
  //  app->setOrganizationDomain(QStringLiteral"ru.brzezinski");
  app->setApplicationName(QStringLiteral("sailmich"));

  qmlRegisterSingletonType<Immich::ImmichPlugin>(
      "Immich", 1, 0, "Immich", Immich::ImmichPlugin::singletonProvider);

  Immich::ImmichPlugin::getSingleton()->registerTypes("Immich");

  QThread *logThread = new QThread(app);
  auto logger = new LogWriter(2000, logThread); // TODO to immichPlugin member
  QObject::connect(app, &QCoreApplication::aboutToQuit, logThread,
                   &QThread::quit);
  QObject::connect(logThread, &QThread::finished, logger,
                   &QObject::deleteLater);
  QQuickView *view = SailfishApp::createView();

  // view->engine()->addImageProvider("blurhash", new BlurHashImageProvider());
  // view->engine()->addImageProvider("thumbhash", new Immich::Thumbhash());
  // view->engine()->addImageProvider("imasset", new
  // Immich::AssetImageProvider(plugin->getSingleton()->getAssets()));
  view->setSource(SailfishApp::pathToMainQml());
  view->show();

  return app->exec();
}
