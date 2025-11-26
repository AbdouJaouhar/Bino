cmake_minimum_required(VERSION 3.28)

project(Bino LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(depthai CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(bino_app
    src/main.cpp
)

target_link_libraries(bino_app
    Qt6::Widgets
)

add_executable(test_camera_connectivity
    tools/test_camera_connectivity.cpp
)

target_link_libraries(test_camera_connectivity
    depthai::core
    ${OpenCV_LIBS}
)
