cmake_minimum_required(VERSION 3.16)
project(BarcodeProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)

add_library(encoder src/Encoder.cpp)
target_include_directories(encoder PUBLIC src)

add_library(imagegenerator src/BarcodeImageGenerator.cpp)
target_include_directories(imagegenerator PUBLIC src)
target_link_libraries(imagegenerator PUBLIC Qt${QT_VERSION_MAJOR}::Gui)

add_executable(barcode_generator src/main.cpp)
target_link_libraries(barcode_generator PRIVATE encoder imagegenerator)

enable_testing()

add_executable(encoder_tests tests/test_encoder.cpp)
target_link_libraries(encoder_tests PRIVATE encoder)
add_test(NAME encoder_tests COMMAND encoder_tests)

add_executable(image_tests tests/test_image.cpp)
target_link_libraries(image_tests PRIVATE encoder imagegenerator)
add_test(NAME image_tests COMMAND image_tests)
