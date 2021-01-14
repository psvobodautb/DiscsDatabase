cmake_minimum_required(VERSION 3.5)

project(mp LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED Sql)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED Sql)

if(ANDROID)
  add_library(mp SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )
else()
  add_executable(mp
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    darkorange/darkorange.qrc
  )
endif()

target_link_libraries(mp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
