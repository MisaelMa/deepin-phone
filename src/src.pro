QT += core gui concurrent
QT += dbus
QT += gui-private widgets-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-phone-assistant
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget gio-unix-2.0 gio-qt udisks2-qt5 taglib

RESOURCES +=         \
    resources.qrc

LIBS +=  \
#        -lffmpegthumbnailer \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
#        -lpostproc \
        -lswresample \
        -lswscale \
        -ltag

DISTFILES += \
    apk/com.ut.phoneassistant.apk \
    deepin-phone-assistant.desktop

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/$$TARGET

target.path = $$INSTROOT$$BINDIR
desktop.path = $$INSTROOT$$APPDIR
desktop.files = $$PWD/deepin-phone-assistant.desktop

apk.path = /usr/share/$$TARGET/apk
apk.files = $$PWD/apk/com.ut.phoneassistant.apk

INSTALLS += target desktop apk

HEADERS += \
    accessible.h \
    accessibledefine.h \
    Application.h

SOURCES += \
    Application.cpp \
    main.cpp

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/TranslationService/ -lPhoneAssistantTranslationService

INCLUDEPATH += $$PWD/../BasicService/TranslationService
DEPENDPATH += $$PWD/../BasicService/TranslationService

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/Model/ -lPhoneAssistantModel

INCLUDEPATH += $$PWD/../BasicService/Model
DEPENDPATH += $$PWD/../BasicService/Model

unix:!macx: LIBS += -L$$OUT_PWD/../MountService/ -lPhoneAssistantMountService

INCLUDEPATH += $$PWD/../MountService
DEPENDPATH += $$PWD/../MountService

unix:!macx: LIBS += -L$$OUT_PWD/../ThreadService/ -lPhoneAssistantThreadService

INCLUDEPATH += $$PWD/../ThreadService
DEPENDPATH += $$PWD/../ThreadService

unix:!macx: LIBS += -L$$OUT_PWD/../UIService/ -lPhoneAssistantUIService

INCLUDEPATH += $$PWD/../UIService
DEPENDPATH += $$PWD/../UIService

#QMAKE_CXXFLAGS += "-fsanitize=undefined,address,leak -fno-omit-frame-pointer"
#QMAKE_CFLAGS += "-fsanitize=undefined,address,leak -fno-omit-frame-pointer"
#QMAKE_LFLAGS += "-fsanitize=undefined,address,leak -fno-omit-frame-pointer"

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/Util/ -lPhoneAssistantUtil

INCLUDEPATH += $$PWD/../BasicService/Util
DEPENDPATH += $$PWD/../BasicService/Util

unix:!macx: LIBS += -L$$OUT_PWD/../Engine/ -lPhoneAssistantEngine

INCLUDEPATH += $$PWD/../Engine
DEPENDPATH += $$PWD/../Engine
