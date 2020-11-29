cmake_minimum_required(VERSION 3.17)
project(boost)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRCS_G
        src/*.cpp
        src/logging/*.cpp
        )

include_directories(
        /usr/local/include
        ${PROJECT_SOURCE_DIR}/src
)


add_definitions(-DBOOST_LOG_DYN_LINK)

find_package(Boost REQUIRED COMPONENTS system filesystem thread log log_setup)
FIND_PACKAGE(Threads)

link_directories(
        /usr/local/lib
)

add_executable(boost ${SRCS_G})

target_link_libraries(boost ${Boost_LIBRARIES})