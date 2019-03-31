cmake_minimum_required(VERSION 3.2)

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
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-macro-redefined -pedantic")
endif()

project(Grabber)
if((NOT DEFINED VERSION) OR ((DEFINED NIGHTLY) AND (NIGHTLY MATCHES "1")))
	set(VERSION "7.0.0")
else()
	string(REGEX REPLACE "^v" "" VERSION "${VERSION}")
endif()
set(USE_SSL 1)

# Android settings
set(ANDROID_APP_NAME ${PROJECT_NAME})
set(ANDROID_APP_VERSION_NAME ${VERSION})
set(ANDROID_APP_VERSION_CODE 1)
set(ANDROID_APP_PACKAGE_NAME "com.bionus.grabber")

message(STATUS "Configuring for version '${VERSION}'")

add_definitions(-DVERSION="${VERSION}")
add_definitions(-DPROJECT_WEBSITE_URL="https://bionus.github.io/imgbrd-grabber")
add_definitions(-DPROJECT_GITHUB_URL="https://github.com/Bionus/imgbrd-grabber")
add_definitions(-DSOURCE_ISSUES_URL="https://raw.githubusercontent.com/wiki/Bionus/imgbrd-grabber/SourceIssues.md")
add_definitions(-DPREFIX="${CMAKE_INSTALL_PREFIX}")

# SSL
if(USE_SSL AND (NOT ANDROID))
	message(STATUS "Compiling with SSL support")
	find_package(OpenSSL REQUIRED)
	if (OPENSSL_FOUND)
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

# Replicate the behavior of "list(FILTER ${LIST} EXCLUDE REGEX ${REGEX})" on older CMake versions
function(listFilterRegex LIST REGEX)
	foreach(ITEM ${${LIST}})
		if(NOT ITEM MATCHES "${REGEX}")
			list(APPEND OUTPUT ${ITEM})
		endif()
	endforeach()
	set(${LIST} ${OUTPUT} PARENT_SCOPE)
endfunction()

# Transpile TS sites into JS
set(SITES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/release/sites")
file(GLOB_RECURSE SITES_TS_FILES "${SITES_DIR}/**.ts")
listFilterRegex(SITES_TS_FILES ".*.d.ts$")
listFilterRegex(SITES_TS_FILES "node_modules")
add_custom_command(
	OUTPUT NPM_modules
	DEPENDS "${SITES_DIR}/package.json"
	COMMAND npm install
	WORKING_DIRECTORY ${SITES_DIR}
	COMMENT "Installing npm packages..."
)
add_custom_command(
	OUTPUT JavaScript_sites
	COMMAND npm run build
	DEPENDS NPM_modules ${SITES_TS_FILES}
	WORKING_DIRECTORY ${SITES_DIR}
	COMMENT "Transpiling TypeScript sources into JavaScript..."
)
add_custom_target(sites ALL DEPENDS JavaScript_sites)
