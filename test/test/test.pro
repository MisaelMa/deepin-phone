QT += core gui concurrent testlib
QT += dbus
QT += gui-private widgets-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test-deepin-phone-assistant
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget gio-unix-2.0 gio-qt udisks2-qt5 taglib


CONFIG += create_pc create_prl no_install_prl

#include(src.pri)

QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_NAME = deepin-phone-assistant
QMAKE_PKGCONFIG_DESCRIPTION = Deepin Font Manager Header Files
QMAKE_PKGCONFIG_INCDIR = $$includes.path

includes.path = $$INCLUDE_INSTALL_DIR/deepin-phone-assistant
includes.files += $$PWD/*.h

isEmpty(PREFIX) {
    PREFIX = /usr
}

isEmpty(LIB_INSTALL_DIR) {
    target.path = $$[QT_INSTALL_LIBS]
} else {
    target.path = $$LIB_INSTALL_DIR
}

isEmpty(INCLUDE_INSTALL_DIR) {
    includes.path = $$PREFIX/include/phoneassistant
} else {
    includes.path = $$INCLUDE_INSTALL_DIR/deepin-phone-assistant
}


UISRC = $$PWD/../../UIService    #用于被单元测试方便的复用
ThreadSrc = $$PWD/../../ThreadService    #用于被单元测试方便的复用

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0

include(../googletest/gtest_dependency.pri)
include(tests/test.pri)

include ($$UISRC/app/app.pri)
include ($$UISRC/base/base.pri)
include ($$UISRC/file/file.pri)
include ($$UISRC/ImageVideo/ImageVideo.pri)
include ($$UISRC/musicEbook/musicEbook.pri)
include ($$UISRC/phoneui/phoneui.pri)
include ($$UISRC/widget/widget.pri)

include ($$ThreadSrc/FileOperate/FileOperate.pri)
include ($$ThreadSrc/Mount/Mount.pri)

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

unix:!macx: LIBS += -L$$OUT_PWD/../../BasicService/Model/ -lPhoneAssistantModel

INCLUDEPATH += $$PWD/../../BasicService/Model
DEPENDPATH += $$PWD/../../BasicService/Model

unix:!macx: LIBS += -L$$OUT_PWD/../../BasicService/TranslationService/ -lPhoneAssistantTranslationService

INCLUDEPATH += $$PWD/../../BasicService/TranslationService
DEPENDPATH += $$PWD/../../BasicService/TranslationService

unix:!macx: LIBS += -L$$OUT_PWD/../../BasicService/Util/ -lPhoneAssistantUtil

INCLUDEPATH += $$PWD/../../BasicService/Util
DEPENDPATH += $$PWD/../../BasicService/Util

unix:!macx: LIBS += -L$$OUT_PWD/../../Engine/ -lPhoneAssistantEngine

INCLUDEPATH += $$PWD/../../Engine
DEPENDPATH += $$PWD/../../Engine

unix:!macx: LIBS += -L$$OUT_PWD/../../MountService/ -lPhoneAssistantMountService

INCLUDEPATH += $$PWD/../../MountService
DEPENDPATH += $$PWD/../../MountService

unix:!macx: LIBS += -L$$OUT_PWD/../../ThreadService/ -lPhoneAssistantThreadService

INCLUDEPATH += $$PWD/../../ThreadService
DEPENDPATH += $$PWD/../../ThreadService

unix:!macx: LIBS += -L$$OUT_PWD/../../UIService/ -lPhoneAssistantUIService

INCLUDEPATH += $$PWD/../../UIService
DEPENDPATH += $$PWD/../../UIService

