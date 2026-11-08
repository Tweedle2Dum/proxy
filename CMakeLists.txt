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
)

target_include_directories(proxy_core PUBLIC include)

add_executable(echo_server src/echo_server.cpp)
target_link_libraries(echo_server PRIVATE proxy_core)


# general utils
find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
  file(GLOB_RECURSE PROXY_FORMAT_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/include/*.hpp
  )
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${PROXY_FORMAT_FILES}
  )
endif()
