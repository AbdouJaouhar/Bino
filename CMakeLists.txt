cmake_minimum_required(VERSION 3.22)
project(bino_vr_system CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(OpenCV REQUIRED)
find_package(depthai CONFIG REQUIRED)

include_directories(include)

add_executable(bino_app
    src/main.cpp
)

target_link_libraries(bino_app
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    ${OpenCV_LIBS}
    depthai::core
)
