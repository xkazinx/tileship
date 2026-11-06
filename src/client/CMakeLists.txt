# The tileship client for a web browser (WebAssembly, Emscripten). Windows builds
# the client from client.vcxproj (build.ps1); this file is the web one, run by
# build_web.ps1 against kazilib's SFML web build (kazilib web/build_deps.ps1):
#   emcmake cmake -S src/client -B <build> -G Ninja -DSFML_DIR=...
# Output: index.html / .js / .wasm - the folder the server's gateway serves
# (deploy\server\web\) and kazin.dev/projects/tileship.
cmake_minimum_required(VERSION 3.21)
project(tileship_web CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(KAZILIB ${TS_SRC}/dep/kazilib)
set(SFML_STATIC_LIBRARIES ON)
find_package(SFML 3 REQUIRED COMPONENTS Graphics)
include(${KAZILIB}/web/kazilib_web.cmake)

file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(FILTER CLIENT_SOURCES EXCLUDE REGEX "stdafx\\.cpp$")
set(COMMON_SOURCES ${TS_SRC}/common/common.cpp ${TS_SRC}/common/random.cpp)

add_executable(tileship ${CLIENT_SOURCES} ${COMMON_SOURCES})
kazilib_web_target(tileship)
# ImGui and ImGui-SFML (kazilib's; in a browser it draws through SFML)
kazilib_web_imgui(tileship)
target_compile_definitions(tileship PRIVATE CLIENT)
# Before kazilib's (its lib/ml/src has a stdafx.h of its own)
target_include_directories(tileship BEFORE PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${TS_SRC}/common)
target_link_libraries(tileship PRIVATE SFML::Graphics)
