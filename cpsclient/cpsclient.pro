TEMPLATE = app

TARGET = cpsclient

CONFIG += debug_and_release

QT += core opengl

equals(QT_MAJOR_VERSION, 5) {
    QT += widgets
}

equals(QT_MAJOR_VERSION, 4) {
    QT += gui
}

HEADERS += \
    src/cameralist.h \
    src/eventthread.h \
    src/histogram.h \
    src/imageinfodlg.h \
    src/mainview.h \
    src/paintlabel.h \
    src/properties.h \
    src/qsliderex.h \
    src/qtui.h \
    src/version.h \
    src/cps.h

SOURCES += \
    src/cameralist.cpp \
    src/eventthread.cpp \
    src/histogram.cpp \
    src/imageinfodlg.cpp \
    src/main.cpp \
    src/mainview.cpp \
    src/paintlabel.cpp \
    src/properties.cpp \
    src/qsliderex.cpp \
    src/tabcamera.cpp \
    src/tabimage.cpp


FORMS += \
    src/histogram.ui \
    src/imageinfodlg.ui \
    src/mainview.ui \
    src/properties.ui

RESOURCES += \
    mainview.qrc

DEFINES += __user__

!win32 {
    # Linux
    DEFINES += __LINUX__
    LIBS += -lueye_api
} else {
    # Windows
    !contains(QMAKE_TARGET.arch, x86_64) {
        LIBS += -lueye_api
    } else {
        LIBS += -lueye_api_64
    }
}

INCLUDEPATH += \
    . \
    include \
    src

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
CONFIG += c++11 \
          gnu++11
