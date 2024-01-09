cmake_minimum_required(VERSION 3.0)
project(BoostAsioExample)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Include Boost headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../dependencies/boost_1_75_0)

# Add your application files
add_executable(server server.cpp)
add_executable(client client.cpp)

find_package(Threads REQUIRED)
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT})