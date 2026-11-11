cmake_minimum_required(VERSION 3.19)
project(increment111 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)
qt_standard_project_setup()

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
    ${OpenCV_INCLUDE_DIRS}
)
install(TARGETS increment111
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET increment111
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
