#-------------------------------------------------
#
# Project created by QtCreator 2020-07-23T20:24:18
#
#-------------------------------------------------

QT       += widgets concurrent

TARGET = PhoneAssistantThreadService
TEMPLATE = lib

DEFINES += THREADSERVICE_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ThreadService.cpp \
    PMTask.cpp \
    BatteryTask.cpp \
    PhoneUseTask.cpp \
    LocalApkVersionTask.cpp \
    filemanagethread.cpp \
    FileDirInfoTask.cpp \
    GetDefaultFileIconTask.cpp \
    PMThread.cpp

HEADERS += \
        ThreadService.h \
    PMTask.h \
    BatteryTask.h \
    PhoneUseTask.h \
    LocalApkVersionTask.h \
    filemanagethread.h \
    FileDirInfoTask.h \
    threadservice_global.h \
    GetDefaultFileIconTask.h \
    PMThread.h

include(Mount/Mount.pri)
include(FileOperate/FileOperate.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!macx: LIBS += -L$$OUT_PWD/../MountService/ -lPhoneAssistantMountService

INCLUDEPATH += $$PWD/../MountService
DEPENDPATH += $$PWD/../MountService

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/TranslationService/ -lPhoneAssistantTranslationService

INCLUDEPATH += $$PWD/../BasicService/TranslationService
DEPENDPATH += $$PWD/../BasicService/TranslationService

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/Model/ -lPhoneAssistantModel

INCLUDEPATH += $$PWD/../BasicService/Model
DEPENDPATH += $$PWD/../BasicService/Model

unix:!macx: LIBS += -L$$OUT_PWD/../BasicService/Util/ -lPhoneAssistantUtil

INCLUDEPATH += $$PWD/../BasicService/Util
DEPENDPATH += $$PWD/../BasicService/Util
