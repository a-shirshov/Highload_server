cmake_minimum_required(VERSION 3.1)
project(Highload_DZ2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(server STATIC lib/server/server.cpp)
target_include_directories(server PUBLIC lib/server)

add_library(response STATIC lib/server/response.cpp)
target_include_directories(response PUBLIC lib/server)

add_executable(main main.cpp)

target_link_libraries(main Threads::Threads)
target_link_libraries(main server)
target_link_libraries(main response)