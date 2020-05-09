#-------------------------------------------------
#
# Project created by QtCreator 2018-11-22T10:24:29
#
#-------------------------------------------------

QT       += core widgets websockets webenginewidgets sql

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = site_cps
TEMPLATE = app

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
    src/json/json_parser.cc \
    src/json/json_scanner.cc \
    src/json/json_scanner.cpp \
    src/json/parser.cpp \
    src/json/parserrunnable.cpp \
    src/json/qobjecthelper.cpp \
    src/json/serializer.cpp \
    src/json/serializerrunnable.cpp \
    src/main.cpp \
    src/config.cpp \
    src/visualizer_app.cpp \
    src/paintlabel.cpp \
    src/failed_panel.cpp \
    src/panel.cpp \
    src/panel_dock_widget.cpp \
    src/panel_factory.cpp \
    src/widget_geometry_change_detector.cpp \
    src/configinfo.cpp \
    src/urlutil.cpp \
    src/setting_panel.cpp \
    src/task_configuration_panel.cpp \
    src/visualization_frame.cpp \
    src/push_button.cpp \
    src/create_task_panel.cpp \
    src/scaled_image_widget.cpp \
    src/screenshot_dialog.cpp \
    src/taskrecordmanager.cpp \
    src/cps_task_parser.cpp \
    src/common.cpp \
    src/task_detail_panel.cpp \
    src/logsave.cpp \
    src/robot_panel.cpp



HEADERS += \
    include/json/FlexLexer.h \
    include/json/json_parser.hh \
    include/json/json_parser.yy \
    include/json/json_scanner.h \
    include/json/json_scanner.yy \
    include/json/location.hh \
    include/json/parser.h \
    include/json/parser_p.h \
    include/json/parserrunnable.h \
    include/json/position.hh \
    include/json/qjson_debug.h \
    include/json/qjson_export.h \
    include/json/qobjecthelper.h \
    include/json/serializer.h \
    include/json/serializerrunnable.h \
    include/json/stack.hh \
    include/site_cps/common_config.h \
    include/site_cps/config.h \
    include/site_cps/cps.h \
    include/site_cps/qtui.h \
    include/site_cps/visualizer_app.h \
    include/site_cps/paintlabel.h \
    include/site_cps/class_id_recording_factory.h \
    include/site_cps/factory.h \
    include/site_cps/failed_panel.h \
    include/site_cps/panel.h \
    include/site_cps/panel_dock_widget.h \
    include/site_cps/panel_factory.h \
    include/site_cps/pluginlib_factory.h \
    include/site_cps/widget_geometry_change_detector.h \
    include/site_cps/window_manager_interface.h \
    include/site_cps/load_resource.h \
    include/site_cps/configinfo.h \
    include/site_cps/urlutil.h \
    include/site_cps/setting_panel.h \
    include/site_cps/task_configuration_panel.h \
    include/site_cps/visualization_frame.h \
    include/site_cps/push_button.h \
    include/site_cps/create_task_panel.h \
    include/site_cps/scaled_image_widget.h \
    include/site_cps/screenshot_dialog.h \
    include/site_cps/taskrecordmanager.h \
    include/site_cps/cps_task_parser.h \
    include/site_cps/common.h \
    include/site_cps/task_detail_panel.h \
    include/site_cps/logsave.h \
    include/site_cps/robot_panel.h



FORMS += \
    ui/setting.ui \
    ui/task_configuration.ui

RESOURCES += \
    resources/images.qrc

INCLUDEPATH += \
    . \
    include/json/ \
    include/site_cps/ \
    src/json/ \
    src

#LIBS += ./libyaml-cpp.so
#INCLUDEPATH += ./include/yaml-cpp

CONFIG += debug_and_release \
          c++11 \
          gnu++11

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT \
                                            _GLIBCXX_USE_CXX11_ABI=0


