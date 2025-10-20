cmake_minimum_required(VERSION 3.10)

project(PAWNY)

add_subdirectory(candy)

add_executable(PAWNY
    src/main.cpp
    src/pawny.cpp
    src/server.cpp
    src/File.cpp
    src/Interface.cpp
    src/FrameQueue.cpp
)

find_package(Boost COMPONENTS thread filesystem system chrono program_options REQUIRED)
find_package(Curses REQUIRED)
include_directories(${Boost_INCLUDE_DIR} ${CURSES_INCLUDE_DIR})
target_link_libraries(PAWNY PRIVATE candy ${Boost_LIBRARIES} ${CURSES_LIBRARIES})
