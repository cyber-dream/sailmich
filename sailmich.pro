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
#CONFIG += debug
#QMAKE_CXXFLAGS_DEBUG += -O0 -g
#QMAKE_CFLAGS_DEBUG   += -O0 -g

INCLUDEPATH += /usr/include/Sailfish
LIBS += -lsailfishsecrets

SOURCES += src/sailmich.cpp \
    immich/api/_base-api.cpp \
    immich/api/_networkmanager.cpp \
    immich/api/api-albums.cpp \
    immich/api/api-assets.cpp \
    immich/api/api-ping.cpp \
    immich/api/api-timeline.cpp \
    immich/api/authentication.cpp \
    immich/api/dto/_albums.cpp \
    immich/api/dto/_authentication.cpp \
    immich/api/dto/_dto.cpp \
    immich/api/dto/_timeline.cpp \
    immich/immich-plugin.cpp \
    immich/models/album/pathview-model.cpp \
    immich/models/album/timeline-model.cpp \
    immich/models/albums/albums-list-model.cpp \
    immich/modules/_base-module.cpp \
    immich/modules/auth.cpp \
    immich/modules/mod-album.cpp \
    immich/modules/mod-assets.cpp \
    immich/modules/ping.cpp \
    immich/modules/secrets.cpp \
    src/logger.cpp

DISTFILES += qml/sailmich.qml \
    qml/components/GalleryOverlay.qml \
    qml/components/MediaViewer.qml \
    qml/components/Thumbnail.qml \
    qml/components/ThumbnailPulsator.qml \
    qml/components/timeline/TimebucketGrid.qml \
    qml/components/timeline/TimebucketStubGrid.qml \
    qml/components/timeline/thumbhash.js \
    qml/cover/CoverPage.qml \
    qml/pages/AlbumTimelinePage.qml \
    qml/pages/AlbumsPage.qml \
    qml/pages/AssetDetails.qml \
    qml/pages/DebugPage.qml \
    qml/pages/InitPage.qml \
    qml/pages/MainPage.qml \
   \
    qml/components/MainPageButton.qml \
    \
    qml/components/timeline/Timeline.qml \
    rpm/sailmich.changes \
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
TRANSLATIONS += translations/sailmich-de.ts

HEADERS += \
    immich/api/_base-api.h \
    immich/api/_networkmanager.h \
    immich/api/api-albums.h \
    immich/api/api-assets.h \
    immich/api/api-ping.h \
    immich/api/api-timeline.h \
    immich/api/authentication.h \
    immich/api/dto/_albums.h \
    immich/api/dto/_authentication.h \
    immich/api/dto/_dto.h \
    immich/api/dto/_timeline.h \
    immich/api/dto/json-parser.h \
    immich/immich-plugin.h \
    immich/is_debug.h \
    immich/models/album/pathview-model.h \
    immich/models/album/timeline-model.h \
    immich/models/albums/albums-list-model.h \
    immich/modules/_base-module.h \
    immich/modules/auth.h \
    immich/modules/mod-album.h \
    immich/modules/mod-assets.h \
    immich/modules/ping.h \
    immich/modules/secrets.h \
    immich/result/result.h \
    src/logger.h \
    src/utils/bool-str.h \
    src/utils/uuid-str.h \
    src/utils/work-queue.h \
    tl-optional.h
