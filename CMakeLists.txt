cmake_minimum_required(VERSION 3.19)

project(icon-theme-viewer
    VERSION 0.1
    LANGUAGES CXX
    HOMEPAGE_URL "https://pedrolcl.github.io/"
)

set(APPDOMAIN "io.github.pedrolcl")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED)
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Widgets)

set(PROJECT_SOURCES
    main.cpp
    framelesswindow.h
    framelesswindow.cpp
    freedesktoptheme.h
    freedesktoptheme.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
)

qt_add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    README.md
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    APPDOMAIN=${APPDOMAIN}
    APPNAME=${PROJECT_NAME}
    APPVERSION=${PROJECT_VERSION}
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
