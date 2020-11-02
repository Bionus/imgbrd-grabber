project(Grabber)
cmake_minimum_required(VERSION 3.2)

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(ignoreMe "${QT_QMAKE_EXECUTABLE}") # Ignore "Manually-specified variables were not used by the project" warnings
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Use C++ 11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable more compiler warnings
if(MSVC OR CMAKE_CXX_COMPILER MATCHES "cl\\.exe")
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4714 /wd4127 /wd4005 /wd4251") # Suppress some warnings
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER MATCHES "g\\+\\+\\.exe")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

project(Grabber)
if((NOT DEFINED VERSION) OR ((DEFINED NIGHTLY) AND (NIGHTLY MATCHES "1")))
	set(VERSION "7.3.2")
else()
	string(REGEX REPLACE "^v" "" VERSION "${VERSION}")
endif()
if(NOT DEFINED VERSION_PLATFORM)
	if(CMAKE_SYSTEM_PROCESSOR MATCHES "[aA][mM][dD]64")
		set(VERSION_PLATFORM "x64")
	else()
		set(VERSION_PLATFORM "x86")
	endif()
endif()
set(USE_SSL 1)

# Android settings
set(ANDROID_APP_NAME ${PROJECT_NAME})
set(ANDROID_APP_VERSION_NAME ${VERSION})
set(ANDROID_APP_VERSION_CODE 1)
set(ANDROID_APP_PACKAGE_NAME "com.bionus.grabber")

message(STATUS "Configuring for version '${VERSION}'")
message(STATUS "Configuring for platform '${VERSION_PLATFORM}' (${CMAKE_SYSTEM_PROCESSOR})")

add_definitions(-DVERSION="${VERSION}")
add_definitions(-DVERSION_PLATFORM="${VERSION_PLATFORM}")
add_definitions(-DPROJECT_WEBSITE_URL="https://bionus.github.io/imgbrd-grabber")
add_definitions(-DPROJECT_GITHUB_URL="https://github.com/Bionus/imgbrd-grabber")
add_definitions(-DSOURCE_ISSUES_URL="https://raw.githubusercontent.com/wiki/Bionus/imgbrd-grabber/SourceIssues.md")
add_definitions(-DPREFIX="${CMAKE_INSTALL_PREFIX}")

# SSL
if(USE_SSL AND (NOT ANDROID))
	message(STATUS "Compiling with SSL support")
	find_package(OpenSSL REQUIRED)
	if (OPENSSL_FOUND)
		message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")
		message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
		message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
		include_directories(${OPENSSL_INCLUDE_DIR})
		list(APPEND LIBS ${OPENSSL_LIBRARIES})
	endif()
endif()

# Enable code coverage on GCC
if((DEFINED ENV{COVERAGE}) AND CMAKE_COMPILER_IS_GNUCXX)
	include(cmake/CodeCoverage.cmake)
	setup_target_for_coverage(coverage tests coverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
endif()

# Pre-compiled headers
if(WIN32)
	set(USE_PCH 1)
else()
	set(USE_PCH 0)
endif()
if(USE_PCH)
	include(cotire/CMake/cotire)
endif()

# Load Qt5Core here as it can only be included once (QTBUG-87863)
find_package(Qt5Core REQUIRED)

add_subdirectory(lib)
add_subdirectory(gui)
add_subdirectory(gui-qml)
add_subdirectory(cli)
add_subdirectory(tests)
add_subdirectory(e2e EXCLUDE_FROM_ALL)
add_subdirectory(crash-reporter)

add_subdirectory(languages)
add_custom_target(translations DEPENDS General_translations CrashReporter_translations)
install(DIRECTORY "dist/common" DESTINATION share/Grabber)
install(DIRECTORY "sites" DESTINATION share/Grabber/sites)
install(FILES "dist/linux/Grabber.desktop" DESTINATION share/applications)

add_subdirectory(sites)
add_dependencies(gui sites)
add_dependencies(gui-qml sites)
add_dependencies(cli sites)
