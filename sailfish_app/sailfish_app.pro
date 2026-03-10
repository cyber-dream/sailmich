TARGET = sailmich

CONFIG += sailfishapp
QT += quick dbus concurrent

# Uncomment for debug
#CONFIG -= release
#CONFIG += debug
#QMAKE_CXXFLAGS_DEBUG += -O0 -g3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_DEBUG -Wall -Wextra
#QMAKE_CFLAGS_DEBUG   += -O0 -g
#DEFINES -= QT_NO_DEBUG
#DEFINES += QT_DEBUG
#QMAKE_STRIP =

HEADERS += $$files(src/*.h, true) \
    src/call-later.h \
    src/utils/asset-media-size-str.h \
    src/utils/oai-request-params-qobject.h

SOURCES += $$files(src/*.cpp, true)

# Immich OpenAPI generated files
INCLUDEPATH += ../immichapi/src/generated

# Link OpenAPI
LIBS += $$OUT_PWD/../immichapi/libimmichapi.a

# Include Sailfish Secrets
INCLUDEPATH += /usr/include/Sailfish

# Link Sailfish secrets
LIBS += -lsailfishsecrets

# Include tl::optional
INCLUDEPATH += third-party/optional/include/tl

# Include tl::expected
INCLUDEPATH += third-party/expected/include/tl

include(third-party/QInjection/src/src.pri)


DISTFILES += $$files(qml/pages, true) \
             $$files(qml/components, true) \
             $$files(qml/cover, true) \
             $$files(qml/cover, true) \
             \
             qml/components/BigButton.qml \
             rpm/sailmich.changes.in \
             rpm/sailmich.changes.run.in \
             rpm/sailmich.spec \
             translations/*.ts \
             sailmich.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

CONFIG += sailfishapp_i18n
CONFIG += sailfishapp_i18n_idbased
SAILFISHAPP_LRELEASE = lrelease -idbased

TRANSLATIONS += translations/sailmich-en_US.ts \
 translations/sailmich-nb_NO.ts \
 translations/sailmich-ru.ts \
 translations/sailmich-et.ts
