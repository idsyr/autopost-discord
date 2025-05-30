cmake_minimum_required (VERSION 3.6)
project(sleepy-discord)

option(BUILD_EXAMPLES                "Build examples of Sleepy Discord and all dependencies"              OFF)
option(SLEEPY_DISCORD_BUILD_EXAMPLES "Build examples of Sleepy Discord"                                   OFF)
option(ONLY_SLEEPY_DISCORD           "Sleepy Discord but none of the dependencies, except build in onces" OFF)
option(ENABLE_VOICE                  "Enable voice support"                                               OFF)
if (NOT ONLY_SLEEPY_DISCORD)
	option(AUTO_DOWNLOAD_LIBRARY         "Automatically download sleepy discord standard config dependencies" OFF)
	option(SLEEPY_VCPKG                  "VCPKG with Sleepy Discord"                                          OFF)
	option(USE_CPR                       "Use CPR http library"                                               ON )
	option(USE_ASIO                      "Use ASIO network and I/O library (Used for UDP and TCP)"            ON)
	option(USE_BOOST_ASIO                "Same as USE_ASIO but for boost library"                             OFF)
	option(USE_LIBOPUS                   "Use Opus audio codec library"                                       OFF)
	option(USE_LIBSODIUM                 "Use libsodium cryptography library"                                 OFF)
	option(USE_ZLIB_NG                   "Use zlib-ng for data compression"                                   OFF)
	option(USE_ZLIB                      "Use zlib for data compression"                                      OFF)
	#option(USE_Boost_BEAST               "Use Boost Beast for http library"                                   OFF)
	#option(USE_IXWEBSOCKET               "Use IXWebSocket for websockets"                                     OFF)
endif()

#Define a variable to use to check if this file has been executed
set(SLEEPY_DISCORD_CMAKE ON)

#set variables based on other variables
if(BUILD_EXAMPLES)
	set(SLEEPY_DISCORD_BUILD_EXAMPLES ON)
endif()

if(ENABLE_VOICE)
	set(USE_LIBOPUS ON)
	set(USE_LIBSODIUM ON)
endif()

if(USE_ZLIB_NG AND USE_ZLIB)
	message(FATAL_ERROR "can't use zlib and zlib-ng at the same time")
endif()

find_package(Git)

# Find and Download libraries
if(AUTO_DOWNLOAD_LIBRARY)

	if(NOT Git_FOUND)
		string(CONCAT GIT_NOT_FOUND_WARNING
			"GIT was not found, CMake list will likely fail. "
			"Please make sure that GIT is inside your PATH variable")
		message(WARNING ${GIT_NOT_FOUND_WARNING})
	endif()

	#to do find librarys before downloading them

	include(ExternalProject)
	include(buildtools/cmake/DownloadProject.cmake)

endif()

# Configure libraries
if(USE_CPR)
	if(SLEEPY_VCPKG)
		find_package(cpr CONFIG REQUIRED)
		set(CPR_LIBRARIES "cpr")
	else()
		if(AUTO_DOWNLOAD_LIBRARY)
			#to do install libssl-dev
			download_project(
				PROJ           cpr
				GIT_REPOSITORY https://github.com/whoshuu/cpr.git
				GIT_TAG        41fbaca
				SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/cpr
				UPDATE_DISCONNECTED 1
			)
		endif()

		#the cmake test has issues on visual studio so disable them
		if(NOT DEFINED BUILD_CPR_TESTS)
			set(BUILD_CPR_TESTS OFF CACHE BOOL "")
		endif()

		set(BUILD_CURL_EXE OFF CACHE BOOL "")
		set(CURL_STATICLIB ON CACHE BOOL "")
		set(CPR_BUILD_TESTS_SSL OFF CACHE BOOL "")

		#set what ssl library to use for curl based on OS
		if(NOT DEFINED CMAKE_USE_OPENSSL AND WIN32)
			set(USE_WINSSL ON CACHE BOOL "")
			set(USE_OPENSSL OFF CACHE BOOL "")
		endif()

		if(NOT DEFINED BUILD_SHARED_LIBS AND WIN32)
			set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
		endif()

		#using cmake to configure curl has issues
		#so use system curl
		set(USE_SYSTEM_CURL ON CACHE BOOL "")

		add_subdirectory(deps/cpr)

		if(NOT DEFINED cpr_SOURCE_DIR)
			set(cpr_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/cpr)
		endif()
	endif()
endif()

if(USE_ASIO)
	if(SLEEPY_VCPKG)
		find_package(asio CONFIG REQUIRED)
	else()
		if(AUTO_DOWNLOAD_LIBRARY)
			download_project(
				PROJ           asio
				GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
				GIT_TAG        master
				SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/asio
				UPDATE_DISCONNECTED 1
			)
		endif()

		if(NOT DEFINED asio_SOURCE_DIR)
			set(asio_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/asio)
		endif()
	endif()
endif()

if(USE_LIBOPUS)
	if(SLEEPY_VCPKG)
		find_package(Opus CONFIG REQUIRED)
	else()
		if(AUTO_DOWNLOAD_LIBRARY)
			#needs libtool
			download_project(
				PROJ           opus
				GIT_REPOSITORY https://gitlab.xiph.org/xiph/opus.git
				GIT_TAG        7628d844
				SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/opus
				UPDATE_DISCONNECTED 1
			)
		endif()

		add_subdirectory(deps/opus)

		if(NOT DEFINED opus_SOURCE_DIR)
			set(opus_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/opus)
		endif()
	endif()
endif()

if(USE_LIBSODIUM)
	if(NOT WIN32 AND NOT SLEEPY_VCPKG)
		if(AUTO_DOWNLOAD_LIBRARY)
			download_project(
				PROJ           libsodium
				GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
				GIT_TAG        stable
				SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/libsodium
				UPDATE_DISCONNECTED 1
			)
			ExternalProject_Add(libsodium-make
				SOURCE_DIR          "${libsodium_SOURCE_DIR}"
				BINARY_DIR          "${libsodium_BINARY_DIR}"
				PATCH_COMMAND       "${libsodium_SOURCE_DIR}/autogen.sh"
				CONFIGURE_COMMAND   "${libsodium_SOURCE_DIR}/configure"
				BUILD_COMMAND       "make"
				INSTALL_COMMAND     "cp" "-TR" "${libsodium_BINARY_DIR}/src/libsodium/include/" "${libsodium_SOURCE_DIR}/src/libsodium/include/"
				TEST_COMMAND        ""
			)
		else()
			set(libsodium_SOURCE_DIR ${PROJECT_SOURCE_DIR}/deps/libsodium)
		endif()
	endif()
endif()

if(USE_ZLIB_NG)
	if(AUTO_DOWNLOAD_LIBRARY)
		download_project(
			PROJ           zlib-ng
			GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
			GIT_TAG        develop
			SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/zlib-ng
			UPDATE_DISCONNECTED 1
		)
	endif()

	add_subdirectory(deps/zlib-ng)
endif()

if(USE_ZLIB)
	find_package(ZLIB REQUIRED)
	#to do add auto download
endif()

if (USE_BEAST)
	if(SLEEPY_VCPKG)
		find_package(Boost REQUIRED COMPONENTS beast)
	else()
		if(AUTO_DOWNLOAD_LIBRARY)
			download_project(
				PROJ           beast
				GIT_REPOSITORY https://github.com/boostorg/beast.git
				GIT_TAG        master
				SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/beast
				UPDATE_DISCONNECTED 1
			)
		endif()
	endif()

	add_subdirectory(deps/beast)
endif()

if (USE_IXWEBSOCKET)
	if (SLEEPY_VCPKG)
		find_path(IXWEBSOCKET_INCLUDE_DIR ixwebsocket/IXWebSocket.h)
		find_library(IXWEBSOCKET_LIBRARY ixwebsocket)
	else()
		if (AUTO_DOWNLOAD_LIBRARY)
			download_project(
				PROJ           IXWebSocket
				GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
				GIT_TAG        master
				SOURCE_DIR     ${PROJECT_SOURCE_DIR}/deps/ixwebsocket
				UPDATE_DISCONNECTED 1
			)
		endif()

		add_subdirectory(deps/ixwebsocket)
		set(IXWEBSOCKET_LIBRARY ixwebsocket)
		set(IXWEBSOCKET_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/deps/ixwebsocket/ixwebsocket)
	endif()
endif ()

# Get Version Info
# Not needed for vcpkg
if(NOT SLEEPY_VCPKG)
	if(Git_FOUND)
		execute_process(
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
			COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
			OUTPUT_VARIABLE SLEEPY_DISCORD_VERSION_BUILD
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		execute_process(
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
			COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
			OUTPUT_VARIABLE SLEEPY_DISCORD_VERSION_BRANCH
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		execute_process(
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
			COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
			OUTPUT_VARIABLE SLEEPY_DISCORD_VERSION_HASH
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if (SLEEPY_DISCORD_VERSION_BRANCH STREQUAL "master")
			set(SLEEPY_DISCORD_VERSION_IS_MASTER "1")
		else()
			set(SLEEPY_DISCORD_VERSION_IS_MASTER "0")
		endif()
		execute_process(
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} 
			COMMAND ${GIT_EXECUTABLE} describe --always --long --dirty
			OUTPUT_VARIABLE SLEEPY_DISCORD_VERSION_DESCRIPTION
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if(
			NOT SLEEPY_DISCORD_VERSION_DESCRIPTION STREQUAL ""
		)
			set(SLEEPY_DISCORD_VERSION_DESCRIPTION_CONCAT " ")
		else()
			set(SLEEPY_DISCORD_VERSION_DESCRIPTION_CONCAT "")
		endif()
		if(${SLEEPY_DISCORD_VERSION_BUILD})
			configure_file(
				"${PROJECT_SOURCE_DIR}/include/sleepy_discord/version.h.in"
				"${PROJECT_SOURCE_DIR}/include/sleepy_discord/version.h"
			)
			set(GIT_INFO_FOUND TRUE)
		else()
			set(GIT_INFO_FOUND FALSE)
		endif()
	else()
		set(GIT_INFO_FOUND FALSE)
	endif()

	if(NOT GIT_INFO_FOUND)
		configure_file(
			"${PROJECT_SOURCE_DIR}/include/sleepy_discord/version-non.h.in"
			"${PROJECT_SOURCE_DIR}/include/sleepy_discord/version.h"
			COPYONLY
		)
	endif()
endif()

# Add Subdirectories
add_subdirectory(sleepy_discord)
if (SLEEPY_DISCORD_BUILD_EXAMPLES)
	add_subdirectory(examples/hello)
	add_subdirectory(examples/slash-commands)
	if (ENABLE_VOICE)
		add_subdirectory(examples/sound-player)
	endif()
endif()
