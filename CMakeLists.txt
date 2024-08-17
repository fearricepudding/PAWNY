cmake_minimum_required(VERSION 3.10)

project(PAWNY)

add_subdirectory(candy)

add_executable(PAWNY
    src/main.cpp
    src/pawny.h
    src/pawny.cpp
    src/worker.cpp
    src/worker.h
)

find_package(Boost COMPONENTS thread system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(PAWNY PRIVATE candy ${Boost_LIBRARIES})
