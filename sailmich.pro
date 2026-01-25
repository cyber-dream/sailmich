# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = sailmich

CONFIG += sailfishapp
CONFIG += debug
QMAKE_CXXFLAGS_DEBUG += -O0 -g
QMAKE_CFLAGS_DEBUG   += -O0 -g

INCLUDEPATH += /usr/include/Sailfish
LIBS += -lsailfishsecrets

HEADERS += \
    src/immich/api/_base-api.h \
    src/immich/api/_networkmanager.h \
    src/immich/api/api-albums.h \
    src/immich/api/api-assets.h \
    src/immich/api/api-ping.h \
    src/immich/api/api-timeline.h \
    src/immich/api/authentication.h \
    src/immich/api/dto/_albums.h \
    src/immich/api/dto/_authentication.h \
    src/immich/api/dto/_dto.h \
    src/immich/api/dto/_timeline.h \
    src/immich/api/dto/json-parser.h \
    src/immich/immich-plugin.h \
    src/immich/is_debug.h \
    src/immich/models/album/pathview-model.h \
    src/immich/models/album/timeline-model.h \
    src/immich/models/albums/albums-list-model.h \
    src/immich/modules/_base.h \
    src/immich/modules/album.h \
    src/immich/modules/assets.h \
    src/immich/modules/auth.h \
    src/immich/modules/ping.h \
    src/immich/modules/secrets/_secrets-base.h \
    src/immich/modules/secrets/sf_secrets.h \
    src/result/result.h \
    src/logger.h \
    src/utils/bool-str.h \
    src/utils/uuid-str.h \
    src/utils/work-queue.h \
    tl-optional.h

SOURCES += src/sailmich.cpp \
    src/immich/api/_base-api.cpp \
    src/immich/api/_networkmanager.cpp \
    src/immich/api/api-albums.cpp \
    src/immich/api/api-assets.cpp \
    src/immich/api/api-ping.cpp \
    src/immich/api/api-timeline.cpp \
    src/immich/api/authentication.cpp \
    src/immich/api/dto/_albums.cpp \
    src/immich/api/dto/_authentication.cpp \
    src/immich/api/dto/_dto.cpp \
    src/immich/api/dto/_timeline.cpp \
    src/immich/immich-plugin.cpp \
    src/immich/models/album/pathview-model.cpp \
    src/immich/models/album/timeline-model.cpp \
    src/immich/models/albums/albums-list-model.cpp \
    src/immich/modules/_base.cpp \
    src/immich/modules/album.cpp \
    src/immich/modules/assets.cpp \
    src/immich/modules/auth.cpp \
    src/immich/modules/ping.cpp \
    src/immich/modules/secrets/sf_secrets.cpp \
    src/logger.cpp

DISTFILES += qml/sailmich.qml \
    qml/components/GalleryOverlay.qml \
    qml/components/MediaViewer.qml \
    qml/components/Thumbnail.qml \
    qml/components/ThumbnailPulsator.qml \
    qml/components/timeline/TimebucketGrid.qml \
    qml/components/timeline/TimebucketStubGrid.qml \
    qml/components/timeline/thumbhash.js \
    \
    qml/cover/CoverPage.qml \
    \
    qml/pages/AlbumTimelinePage.qml \
    qml/pages/AlbumsPage.qml \
    qml/pages/AssetDetails.qml \
    qml/pages/AuthDialog.qml \
    qml/pages/DebugPage.qml \
    qml/pages/InitPage.qml \
    qml/pages/MainPage.qml \
    \
    qml/components/MainPageButton.qml \
    qml/components/timeline/Timeline.qml \
    \
rpm/sailmich.changes.in \
    rpm/sailmich.changes.run.in \
    rpm/sailmich.spec \
    translations/*.ts \
    sailmich.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/sailmich-en_US.ts
