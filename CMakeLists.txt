cmake_minimum_required(VERSION 3.20)
project(opencv_portfolio VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs highgui videoio video calib3d features2d)
message(STATUS "OpenCV ${OpenCV_VERSION} found at ${OpenCV_DIR}")

add_library(portfolio_common INTERFACE)
target_include_directories(portfolio_common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/common/include)
target_compile_features(portfolio_common INTERFACE cxx_std_20)
target_link_libraries(portfolio_common INTERFACE ${OpenCV_LIBS})

if(MSVC)
    target_compile_options(portfolio_common INTERFACE /W4)
else()
    target_compile_options(portfolio_common INTERFACE -Wall -Wextra -Wpedantic)
endif()

add_subdirectory(01-defect-inspection)
add_subdirectory(02-label-localization)
add_subdirectory(03-video-analytics)
add_subdirectory(tools)
