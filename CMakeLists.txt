cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(FRONTEND_SOURCES
    frontend/main.cpp
    frontend/mainwindow.cpp
    frontend/mainwindow.h
    frontend/mainwindow.ui
)

set(BACKEND_SOURCES
    backend/BarcodeDecoder.cpp
    backend/BarcodeDecoder.h
    backend/BarcodeImage.cpp
    backend/BarcodeImage.h
    backend/ImageProcessor.cpp
    backend/ImageProcessor.h
    backend/Integration.cpp
)

set(PROJECT_SOURCES
    ${FRONTEND_SOURCES}
    ${BACKEND_SOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(untitled
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(untitled SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(untitled
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(untitled PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/backend
    ${CMAKE_CURRENT_SOURCE_DIR}/frontend
)

target_link_libraries(untitled PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.untitled)
endif()
set_target_properties(untitled PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS untitled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(untitled)
endif()
