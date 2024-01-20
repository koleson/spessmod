cmake_minimum_required(VERSION 3.8)
include(ExternalProject)

project(spessmod)

# note to self:  setting a default value of ON within `option` does not seem to work.
# kmo 20 jan 2024 00h26
option(INFLUX_LOGGING "Log data to InfluxDB")
# set(INFLUX_LOGGING ON)

file(GLOB MAIN_SOURCES src/main/*.c)
file(GLOB MAIN_HEADERS src/main/*.h)

add_executable(
	spessmod
	${MAIN_SOURCES}
	)
	
set(CMAKE_CXX_FLAGS_DEBUG "-g")


# pcap
set(LIBPCAP_NAME pcap)
set(LIBPCAP_GIT_URL https://github.com/the-tcpdump-group/libpcap)
set(LIBPCAP_GIT_BRANCH libpcap-1.10)

ExternalProject_Add(
	${LIBPCAP_NAME}
	PREFIX ${CMAKE_BINARY_DIR}/external/pcap
	GIT_REPOSITORY ${LIBPCAP_GIT_URL}
	GIT_TAG ${LIBPCAP_GIT_BRANCH}
	CMAKE_ARGS -DDISABLE_DBUS=ON -DBUILD_WITH_LIBNL=FALSE -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/pcap
)

ExternalProject_Get_Property(${LIBPCAP_NAME} SOURCE_DIR)

set_target_properties(${LIBPCAP_NAME} PROPERTIES
	IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/external/pcap/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${LIBPCAP_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}"
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/external/pcap/include"
)

get_target_property(PCAP_LIB_FILE ${LIBPCAP_NAME} IMPORTED_LOCATION)
get_target_property(PCAP_INCLUDE_DIR ${LIBPCAP_NAME} INTERFACE_INCLUDE_DIRECTORIES)


if(INFLUX_LOGGING)
	target_compile_definitions(spessmod PRIVATE INFLUX_LOGGING=1)
	message(INFO "building with influx logging support!")
	# libcurl - allows logging to influxdb via http(s) line protocol	
	ExternalProject_Add(
    curl
    PREFIX ${CMAKE_BINARY_DIR}/external/curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG "curl-8_5_0"
    CMAKE_ARGS 
				-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/curl 
				-DBUILD_SHARED_LIBS=OFF 
				-DHTTP_ONLY=ON
				-DCURL_ENABLE_SSL=OFF -DCURL_USE_LIBPSL=OFF -DCURL_USE_LIBSSH2=OFF
				-DUSE_LIBIDN2=OFF														# internationalized domain names
				-DCMAKE_DISABLE_FIND_PACKAGE_ZLIB=TRUE			# lightweight/static linking currently more important				
	)


	set_target_properties(curl PROPERTIES
			IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/external/curl/lib/${CMAKE_STATIC_LIBRARY_PREFIX}curl${CMAKE_STATIC_LIBRARY_SUFFIX}"
			INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/external/curl/include"
	)
	get_target_property(CURL_LIB_FILE curl IMPORTED_LOCATION)
	get_target_property(CURL_INCLUDE_DIR curl INTERFACE_INCLUDE_DIRECTORIES)

	target_include_directories(spessmod PRIVATE ${CURL_INCLUDE_DIR})
	add_dependencies(spessmod curl)
	target_link_libraries(spessmod PRIVATE ${CURL_LIB_FILE})
	

	file(GLOB LOGGING_SOURCES src/logging/*.c)
	file(GLOB LOGGING_HEADERS src/logging/*.h)
	target_include_directories(spessmod PRIVATE src/logging)
	target_sources(spessmod PRIVATE ${LOGGING_SOURCES})
else()
	message(INFO "building WITHOUT influx logging support!")
endif() # INFLUX_LOGGING



target_include_directories(spessmod PRIVATE 
			${PCAP_INCLUDE_DIR}
			src/main/include)

add_dependencies(spessmod 
			${LIBPCAP_NAME}
)

target_link_libraries(spessmod PRIVATE 
			${PCAP_LIB_FILE}
)
