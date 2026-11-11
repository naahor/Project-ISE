
cmake_minimum_required(VERSION 3.19)
project(increment111 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

set(OpenCV_DIR "C:/Users/HADI/Downloads/opencv/build")
find_package(OpenCV REQUIRED)

set(CMAKE_AUTOMOC ON)

qt_add_executable(increment111
    WIN32 MACOSX_BUNDLE
    Frontend/main.cpp
    Frontend/mainwindow.h
    Frontend/mainwindow.cpp
    Backend/Encoder/Encoder.h
    Backend/Encoder/Encoder.cpp
    Backend/imageGenerator/BarcodeImageGenerator.h
    Backend/imageGenerator/BarcodeImageGenerator.cpp
    Backend/Decoder.h
    Backend/Decoder.cpp
    Backend/ImageProcessor.h
    Backend/ImageProcessor.cpp
)

target_link_libraries(increment111
    PRIVATE
        Qt::Core
        Qt::Widgets
        ${OpenCV_LIBS}
)

target_include_directories(increment111 PRIVATE
    Backend
    Backend/imageGenerator
    Frontend
    "C:/Users/HADI/Downloads/opencv/build/include"
    ${OpenCV_INCLUDE_DIRS}
)
