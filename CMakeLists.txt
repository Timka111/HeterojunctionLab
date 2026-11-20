cmake_minimum_required(VERSION 3.16)
project(HeterojunctionLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

add_executable(HeterojunctionLab
    main.cpp
    mainwindow.h
    mainwindow.cpp
    heterojunction_model.h
    heterojunction_model.cpp
    diagram_plot_widget.h
    diagram_plot_widget.cpp
    band_blocks_widget.h
    band_blocks_widget.cpp
    potential_plot_widget.h
    potential_plot_widget.cpp
    resources.qrc
)

target_link_libraries(HeterojunctionLab PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
