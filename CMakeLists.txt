cmake_minimum_required(VERSION 2.8.12)

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

project(Grabber)
set(USE_SSL 1)

add_definitions(-DVERSION="5.4.0a1")
add_definitions(-DPROJECT_WEBSITE_URL="https://bionus.github.io/imgbrd-grabber/")
add_definitions(-DPROJECT_GITHUB_URL="https://github.com/Bionus/imgbrd-grabber")
add_definitions(-DPREFIX="$ENV{PREFIX}")

# SSL
if(USE_SSL)
	message("Compiling with SSL support")
	find_package(OpenSSL REQUIRED)
	if (OPENSSL_FOUND)
		message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
		message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")
		include_directories(${OPENSSL_INCLUDE_DIR})
		list(APPEND LIBS ${OPENSSL_LIBRARIES})
	endif()
endif()

# Enable code coverage on the Travis Linux build
if((DEFINED ENV{TRAVIS}) AND UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX)
	include(cmake/CodeCoverage.cmake)
	setup_target_for_coverage(coverage tests coverage)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
endif()

add_subdirectory(lib)
add_subdirectory(gui)
add_subdirectory(cli)
add_subdirectory(tests)
add_subdirectory(CrashReporter)

add_subdirectory(languages)
add_custom_target(translations DEPENDS General_translations CrashReporter_translations)
