#-------------------------------------------------
#
# Project created by QtCreator 2020-07-23T20:24:18
#
#-------------------------------------------------

QT       += widgets gui-private widgets-private

TARGET = PhoneAssistantUIService
TEMPLATE = lib

DEFINES += UISERVICE_LIBRARY

CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget
PKGCONFIG += gio-unix-2.0 gio-qt udisks2-qt5 taglib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

include(app/app.pri)
include(file/file.pri)
include(base/base.pri)
include(ImageVideo/ImageVideo.pri)
include(musicEbook/musicEbook.pri)
include(phoneui/phoneui.pri)
include(widget/widget.pri)

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        UIService.cpp

HEADERS += \
        UIService.h \
    uiservice_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    images/images.qrc \
    assets/icons.qrc


unix:!macx: LIBS += -L$$OUT_PWD/../ThreadService/ -lPhoneAssistantThreadService

INCLUDEPATH += $$PWD/../ThreadService
DEPENDPATH += $$PWD/../ThreadService

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/TranslationService/ -lPhoneAssistantTranslationService

INCLUDEPATH += $$PWD/../BasicService/TranslationService
DEPENDPATH += $$PWD/../BasicService/TranslationService

unix:!macx: LIBS += -L$$OUT_PWD/../MountService/ -lPhoneAssistantMountService

INCLUDEPATH += $$PWD/../MountService
DEPENDPATH += $$PWD/../MountService

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/Model/ -lPhoneAssistantModel

INCLUDEPATH += $$PWD/../BasicService/Model
DEPENDPATH += $$PWD/../BasicService/Model

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/Util/ -lPhoneAssistantUtil

INCLUDEPATH += $$PWD/../BasicService/Util
DEPENDPATH += $$PWD/../BasicService/Util

unix:!macx: LIBS += -L$$OUT_PWD/../Engine/ -lPhoneAssistantEngine

INCLUDEPATH += $$PWD/../Engine
DEPENDPATH += $$PWD/../Engine
