TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    immichapi \
    sailfish_app

sailfish_app.depends = immichapi

OTHER_FILES += \
    rpm/sailmich.spec \
    rpm/sailmich.changes.in \
    rpm/sailmich.changes.run.in
