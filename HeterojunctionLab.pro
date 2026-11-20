QT       += core gui widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17
TARGET = HeterojunctionLab
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    heterojunction_model.cpp \
    diagram_plot_widget.cpp \
    band_blocks_widget.cpp \
    potential_plot_widget.cpp

HEADERS += \
    mainwindow.h \
    heterojunction_model.h \
    diagram_plot_widget.h \
    band_blocks_widget.h \
    potential_plot_widget.h

RESOURCES += \
    resources.qrc

RC_ICONS = app_icon.ico

win32 {
    QMAKE_CXXFLAGS += -Wall
}
