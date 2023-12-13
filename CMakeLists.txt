cmake_minimum_required(VERSION 3.5)
include(ExternalProject)

project(spessmod)

add_executable(
	spessmod main.c
	capture_setup.c
	capture_setup.h
	packet_processing.c
	packet_processing.h
	known_registers.c
	known_registers.h
	)
	
set(CMAKE_CXX_FLAGS_DEBUG "-g")

set(LIBPCAP_NAME libpcap)
set(LIBPCAP_GIT_URL https://github.com/the-tcpdump-group/libpcap)
set(LIBPCAP_GIT_BRANCH libpcap-1.10)

ExternalProject_Add(
	${LIBPCAP_NAME}
	PREFIX ${CMAKE_BINARY_DIR}/external
	GIT_REPOSITORY ${LIBPCAP_GIT_URL}
	GIT_TAG ${LIBPCAP_GIT_BRANCH}
	CMAKE_ARGS -DDISABLE_DBUS=ON -DBUILD_WITH_LIBNL=FALSE -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external
)


add_dependencies(spessmod ${LIBPCAP_NAME})

ExternalProject_Get_Property(${LIBPCAP_NAME} SOURCE_DIR)

#set_target_properties(${LIBPCAP_NAME} PROPERTIES
#	IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/external/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${LIBPCAP_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
#	INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_DIR}/include"
#)

target_link_libraries(spessmod PRIVATE ${CMAKE_BINARY_DIR}/external/lib/libpcap.a)
