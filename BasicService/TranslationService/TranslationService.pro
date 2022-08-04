#-------------------------------------------------
#
# Project created by QtCreator 2020-07-21T20:38:05
#
#-------------------------------------------------

QT       -= gui

TARGET = PhoneAssistantTranslationService
TEMPLATE = lib

DEFINES += TRANSLATIONSERVICE_LIBRARY

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
        TranslationService.cpp \
    TrObject.cpp \
    settings_translation.cpp \
    ImportExportObbject.cpp

HEADERS += \
        TranslationService.h \
        translationservice_global.h \ 
    TrObject.h \
    ImportExportObbject.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

TRANSLATIONS += \
    ../../translations/deepin-phone-assistant.ts

CONFIG(release, debug|release) {
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    TRANSLATIONFILES= $$files($$PWD/../../translations/*.ts)
    for(tsfile, TRANSLATIONFILES) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
    #将qm文件添加到安装包
    dtk_translations.path = /usr/share/deepin-phone-assistant/translations
    dtk_translations.files = $$PWD/../../translations/*.qm
    INSTALLS += dtk_translations
}
