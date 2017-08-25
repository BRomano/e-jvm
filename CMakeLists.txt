cmake_minimum_required(VERSION 3.6)   # CMake version check
project(e-jvm)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 11)            # Enable c++11 standard

set(BOOST_ROOT C:/boost_1_61_0/boost)
set(Boost_INCLUDE_DIR C:/boost_1_61_0)
set(BOOST_LIBRARYDIR C:/boost_1_61_0/bin.v2/libs/)


#link_directories(${Boost_LIBRARY_DIR})

file(GLOB SOURCES src/*.h src/*.cpp)

set(SOURCE_FILES main.cpp)
add_subdirectory(src)

set(Boost_DEBUG                  OFF)
set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED system filesystem)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCES})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()
