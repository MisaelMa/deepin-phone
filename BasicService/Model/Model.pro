#-------------------------------------------------
#
# Project created by QtCreator 2020-07-04T10:08:54
#
#-------------------------------------------------

QT       += widgets gui

TARGET = PhoneAssistantModel
TEMPLATE = lib

DEFINES += MODEL_LIBRARY
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

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
        Model.cpp \
    DataService.cpp \
    PMFileIconProvider.cpp \
    PhoneMastetFileSystemModel.cpp \
    MountFileModel.cpp

HEADERS += \
        Model.h \
    model_global.h \
    GlobalDefine.h \
    defines.h \
    DataService.h \
    FileModel.h \
    PMFileIconProvider.h \
    PhoneMastetFileSystemModel.h \
    MountFileModel.h \
    singleton.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

unix:!macx: LIBS += -L$$OUT_PWD/../TranslationService/ -lPhoneAssistantTranslationService

INCLUDEPATH += $$PWD/../TranslationService
DEPENDPATH += $$PWD/../TranslationService
