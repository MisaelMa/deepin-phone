#-------------------------------------------------
#
# Project created by QtCreator 2020-07-28T18:45:14
#
#-------------------------------------------------

QT += widgets dbus network

TARGET = PhoneAssistantMountService
TEMPLATE = lib

DEFINES += MOUNTSERVICE_LIBRARY

CONFIG += c++11 link_pkgconfig
PKGCONFIG += gio-unix-2.0 gio-qt udisks2-qt5

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include (Android/Android.pri)
include (Ios/Ios.pri)

SOURCES += \
    MountService.cpp \
    phoneconnectthread.cpp \
    phonethread.cpp \
    SocketThread.cpp

HEADERS += \
    MountService.h \
    phoneconnectthread.h \
    phonethread.h \
    mountservice_global.h \
    SocketThread.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/TranslationService/ -lPhoneAssistantTranslationService

INCLUDEPATH += $$PWD/../BasicService/TranslationService
DEPENDPATH += $$PWD/../BasicService/TranslationService

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/Model/ -lPhoneAssistantModel

INCLUDEPATH += $$PWD/../BasicService/Model
DEPENDPATH += $$PWD/../BasicService/Model

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/Util/ -lPhoneAssistantUtil

INCLUDEPATH += $$PWD/../BasicService/Util
DEPENDPATH += $$PWD/../BasicService/Util
