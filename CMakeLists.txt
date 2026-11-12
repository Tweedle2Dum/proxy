cmake_minimum_required(VERSION 3.28)
project(proxy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(PROXY_SANITIZE "Build with AddressSanitizer + UBSan" OFF)
if(PROXY_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

add_compile_options(-Wall -Wextra -Wpedantic -Werror)


add_library(proxy_core src/file_descriptor.cpp
                       src/socket.cpp
                       src/forward.cpp
)

target_include_directories(proxy_core PUBLIC include)

add_executable(echo_server examples/echo_server.cpp)
target_link_libraries(echo_server PRIVATE proxy_core)

find_package(Threads REQUIRED)
add_executable(proxy apps/proxy.cpp)
target_link_libraries(proxy PRIVATE proxy_core Threads::Threads)
