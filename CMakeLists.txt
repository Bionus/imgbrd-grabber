cmake_minimum_required(VERSION 3.6.0)

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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4714 /wd4127") # Suppress some warnings
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

project(Grabber)
set(USE_SSL 1)

add_definitions(-DVERSION="6.0.3")
add_definitions(-DPROJECT_WEBSITE_URL="https://bionus.github.io/imgbrd-grabber/")
add_definitions(-DPROJECT_GITHUB_URL="https://github.com/Bionus/imgbrd-grabber")
add_definitions(-DSOURCE_ISSUES_URL="https://raw.githubusercontent.com/wiki/Bionus/imgbrd-grabber/SourceIssues.md")
add_definitions(-DPREFIX="${CMAKE_INSTALL_PREFIX}")

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
	add_definitions(-DTRAVIS=1)
	if("$ENV{TRAVIS_OS_NAME}" STREQUAL "osx")
		add_definitions(-DTRAVIS_OS_OSX=1)
	elseif("$ENV{TRAVIS_OS_NAME}" STREQUAL "linux")
		add_definitions(-DTRAVIS_OS_LINUX=1)
	endif()
endif()

add_subdirectory(lib)
add_subdirectory(gui)
add_subdirectory(cli)
add_subdirectory(tests)
add_subdirectory(e2e EXCLUDE_FROM_ALL)
add_subdirectory(CrashReporter)

add_subdirectory(languages)
add_custom_target(translations DEPENDS General_translations CrashReporter_translations)
install(DIRECTORY release/ DESTINATION share/Grabber/
		PATTERN "*.desktop" EXCLUDE
		PATTERN "*.exe" EXCLUDE)
install(FILES "release/Grabber.desktop" DESTINATION share/applications/)

# Transpile TS sites into JS
set(SITES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/release/sites")
set(NODE_MODULES_DIR "${SITES_DIR}/node_modules")
file(GLOB_RECURSE SITES_TS_FILES "${SITES_DIR}/**.ts")
list(FILTER SITES_TS_FILES EXCLUDE REGEX ".*.d.ts$")
list(FILTER SITES_TS_FILES EXCLUDE REGEX "node_modules")
add_custom_command(
	OUTPUT ${NODE_MODULES_DIR}
	DEPENDS "${SITES_DIR}/package.json"
	COMMAND npm install
	WORKING_DIRECTORY ${SITES_DIR}
	COMMENT "Installing npm packages..."
)
add_custom_command(
	OUTPUT JavaScript_sites
	COMMAND npm run build
	DEPENDS ${NODE_MODULES_DIR} ${SITES_TS_FILES}
	WORKING_DIRECTORY ${SITES_DIR}
	COMMENT "Transpiling TypeScript sources into JavaScript..."
)
add_custom_target(sites ALL DEPENDS JavaScript_sites)
