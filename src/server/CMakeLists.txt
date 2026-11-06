# The tileship server for Linux (the deployed one: kazin.dev/projects/tileship).
# Windows builds the server from server.vcxproj (build.ps1); this file is for
# Linux, built in a container by build_linux.ps1 (Ubuntu 26.04, as the server,
# with libmysqlclient-dev):
#   cmake -S src/server -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build
cmake_minimum_required(VERSION 3.21)
project(tileship_server CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(KAZILIB ${TS_SRC}/dep/kazilib)

file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(FILTER SERVER_SOURCES EXCLUDE REGEX "stdafx\\.cpp$")
set(COMMON_SOURCES ${TS_SRC}/common/common.cpp ${TS_SRC}/common/random.cpp)

# kazilib: enet (the Unix socket layer), the packet layer, the socket, the
# WebSocket gateway, the log's output
set(KAZILIB_SOURCES
	${KAZILIB}/enet/callbacks.cpp
	${KAZILIB}/enet/compress.cpp
	${KAZILIB}/enet/host.cpp
	${KAZILIB}/enet/list.cpp
	${KAZILIB}/enet/packet.cpp
	${KAZILIB}/enet/peer.cpp
	${KAZILIB}/enet/protocol.cpp
	${KAZILIB}/enet/unix.cpp
	${KAZILIB}/lib/shared/src/net/net_packet.cpp
	${KAZILIB}/lib/shared/src/net/socket.cpp
	${KAZILIB}/lib/shared/src/net/ws_gateway.cpp
	${KAZILIB}/lib/ml/src/log_out.cpp
)

add_executable(tileship_server ${SERVER_SOURCES} ${COMMON_SOURCES} ${KAZILIB_SOURCES})
target_include_directories(tileship_server PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${TS_SRC}/common
	${KAZILIB}/lib/shared/include
	${KAZILIB}/lib/ml/include
	${KAZILIB}/enet/include
	${KAZILIB}/lib/ml/src
)
target_compile_definitions(tileship_server PRIVATE HAS_SOCKLEN_T=1)

# The MySQL C API: libmysqlclient (apt libmysqlclient-dev; the box runs it from
# libmysqlclient24, install.sh)
find_package(PkgConfig REQUIRED)
pkg_check_modules(MYSQL REQUIRED IMPORTED_TARGET mysqlclient)

find_package(Threads REQUIRED)
target_link_libraries(tileship_server PRIVATE PkgConfig::MYSQL Threads::Threads)
# The C++ runtime inside (no libstdc++ version needed there)
target_link_options(tileship_server PRIVATE -static-libstdc++ -static-libgcc)
