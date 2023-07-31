# General
project(gui VERSION ${VERSION})
add_definitions(-DGUI=1)

# Enable Breakpad and QScintilla in Release mode only
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	if(NOT DEFINED USE_BREAKPAD)
		set(USE_BREAKPAD 1)
	endif()
	if(NOT DEFINED USE_QSCINTILLA)
		set(USE_QSCINTILLA 1)
	endif()
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable Google Breakpad if the provided folder does not exist
if(USE_BREAKPAD)
	if(NOT DEFINED BREAKPAD)
		if(WIN32)
			set(BREAKPAD "D:/bin/google-breakpad")
		elseif(UNIX)
			set(BREAKPAD "~/Programmation/google-breakpad")
		endif()
	endif()
	if((NOT DEFINED BREAKPAD) OR (NOT EXISTS "${BREAKPAD}"))
		message(WARNING "Provided Google Breakpad directory does not exist, disabling Google Breakpad: ${BREAKPAD}")
		set(USE_BREAKPAD 0)
	endif()
endif()

# Nightly version settings
if((DEFINED NIGHTLY) AND (NIGHTLY MATCHES "1"))
	add_definitions(-DNIGHTLY=1)
	add_definitions(-DNIGHTLY_COMMIT="${COMMIT}")
	message(STATUS "Configuring nightly with commit '${COMMIT}'")
endif()

# Qt libraries
find_package(Qt6 COMPONENTS Core Concurrent Gui Network Multimedia MultimediaWidgets Sql Widgets Xml REQUIRED)
set(QT_LIBRARIES Qt6::Core Qt6::Concurrent Qt6::Gui Qt6::Multimedia Qt6::MultimediaWidgets Qt6::Network Qt6::Sql Qt6::Widgets Qt6::Xml)

# Android specials
if(ANDROID)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt6::CorePrivate)
endif()

# QScintilla
if(USE_QSCINTILLA)
	find_package(QScintilla)
	if(QSCINTILLA_FOUND)
		message(STATUS "Building with QScintilla2 support")
		add_definitions(-DUSE_QSCINTILLA=1)
		list(APPEND LIBS ${QSCINTILLA_LIBRARY})
	else()
		message(WARNING "QScintilla2 not found")
	endif()
endif()

find_program(JQ_EXISTS "jq")
if(JQ_EXISTS)
	execute_process(COMMAND jq -r "[.contributors[].login] | @tsv" "${CMAKE_CURRENT_LIST_DIR}/../../.all-contributorsrc" OUTPUT_VARIABLE CONTRIBUTORS)
	string(STRIP "${CONTRIBUTORS}" CONTRIBUTORS)
else()
	message(WARNING "No contributors list passed and 'jq' not found")
endif()
add_definitions(-DCONTRIBUTORS="${CONTRIBUTORS}")

# Input
file(GLOB SOURCES
	"src/main/main.cpp"
	"src/*.cpp"
	"src/batch/*.cpp"
	"src/custom-buttons/*.cpp"
	"src/docks/*.cpp"
	"src/settings/*.cpp"
	"src/sources/*.cpp"
	"src/tabs/*.cpp"
	"src/threads/*.cpp"
	"src/ui/*.cpp"
	"src/updater/*.cpp"
	"src/utils/rename-existing/*.cpp"
	"src/utils/blacklist-fix/*.cpp"
	"src/utils/empty-dirs-fix/*.cpp"
	"src/utils/md5-database-converter/*.cpp"
	"src/utils/md5-fix/*.cpp"
	"src/utils/tag-loader/*.cpp"
	"src/viewer/*.cpp"
	"src/viewer/players/*.cpp"
	"resources/*.rc"
	"resources/*.qrc")
file(GLOB_RECURSE FORMS "src/*.ui")
include_directories("src/" "../lib/src/" "../lib/" "../cli/src/" ".." ".")

# Windows specials
if(WIN32)
	file(GLOB_RECURSE WINEXTRAS_SOURCES "vendor/winextras/*.cpp")
	set(SOURCES ${SOURCES} ${WINEXTRAS_SOURCES})
	list(APPEND LIBS "dwmapi.lib")
endif()

# Breakpad files
if(USE_BREAKPAD)
	file(GLOB BREAKPAD_SOURCES "src/crashhandler/*.cpp")
	set(SOURCES ${SOURCES} ${BREAKPAD_SOURCES})
	add_definitions(-DUSE_BREAKPAD=1)
	if(WIN32)
		set(CMAKE_LFLAGS_RELEASE "${CMAKE_LFLAGS_RELEASE} /INCREMENTAL:NO /DEBUG")
		set(CMAKE_CFLAGS_RELEASE "${CMAKE_CFLAGS_RELEASE} -O2 -MD -zi")
		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			list(APPEND LIBS
				"${BREAKPAD}/src/client/windows/Release/lib/common.lib"
				"${BREAKPAD}/src/client/windows/Release/lib/crash_generation_client.lib"
				"${BREAKPAD}/src/client/windows/Release/lib/exception_handler.lib")
		elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
			list(APPEND LIBS
				"${BREAKPAD}/src/client/windows/Debug/lib/common.lib"
				"${BREAKPAD}/src/client/windows/Debug/lib/crash_generation_client.lib"
				"${BREAKPAD}/src/client/windows/Debug/lib/exception_handler.lib")
		endif()
	elseif(UNIX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
		list(APPEND LIBS "${BREAKPAD}/src/client/linux/libbreakpad_client.a")
	endif()
	message(STATUS "Using Google Breakpad from ${BREAKPAD}")
	include_directories(${BREAKPAD}/src)
endif()

if(WIN32)
	qt_add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${FORMS} ${UTILS_SOURCES})
else()
	qt_add_executable(${PROJECT_NAME} ${SOURCES} ${FORMS} ${UTILS_SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIBRARIES} ${LIBS} lib "cli-lib")

if(ANDROID)
	# SSL
	if(USE_SSL)
		message(STATUS "Compiling with SSL support")
		include("${CMAKE_CURRENT_LIST_DIR}/../cmake/android_openssl/android_openssl.cmake")
		add_android_openssl_libraries(${PROJECT_NAME})
		get_target_property(ANDROID_EXTRA_LIBS ${PROJECT_NAME} QT_ANDROID_EXTRA_LIBS)
		message(STATUS "Android libs: ${ANDROID_EXTRA_LIBS}")
	endif()

	# Generate static package directory
	set(ANDROID_PACKAGE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/android_package")
	file(REMOVE_RECURSE "${ANDROID_PACKAGE_SOURCES}")
	file(MAKE_DIRECTORY "${ANDROID_PACKAGE_SOURCES}")
	file(COPY "${CMAKE_CURRENT_LIST_DIR}/../dist/android/" DESTINATION "${ANDROID_PACKAGE_SOURCES}")
	file(COPY "${CMAKE_CURRENT_LIST_DIR}/../dist/common/" DESTINATION "${ANDROID_PACKAGE_SOURCES}/assets")

	# Add target for compiled assets
	add_custom_target(android_copy_assets
		DEPENDS sites translations
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_LIST_DIR}/../sites/" "${ANDROID_PACKAGE_SOURCES}/assets/sites"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${ANDROID_PACKAGE_SOURCES}/assets/sites/node_modules/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/../languages/" "${ANDROID_PACKAGE_SOURCES}/assets/languages"
	)
	add_dependencies(${PROJECT_NAME} android_copy_assets)

	# Set Android properties
	set_target_properties(${PROJECT_NAME} PROPERTIES
		QT_ANDROID_PACKAGE_SOURCE_DIR ${ANDROID_PACKAGE_SOURCES}
		QT_ANDROID_VERSION_NAME ${ANDROID_APP_VERSION_NAME}
		QT_ANDROID_VERSION_CODE ${ANDROID_APP_VERSION_CODE}
	)
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set_target_properties(${PROJECT_NAME} PROPERTIES QT_ANDROID_DEPLOY_RELEASE ON)
	endif()
	# NAME ${ANDROID_APP_NAME}
	# PACKAGE_NAME ${ANDROID_APP_PACKAGE_NAME}

	# Sign package if the keystore was provided
	if(DEFINED ENV{QT_ANDROID_KEYSTORE_PATH})
		set(QT_ANDROID_SIGN_APK ON)
	else()
		message(WARNING "No keystore provided, APK signing disabled")
	endif()
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/gui/$<0:>" OUTPUT_NAME "Grabber")
	install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
endif()

# Pre-compiled header
if(USE_PCH)
	cotire(${PROJECT_NAME})
endif()

# Tests
add_subdirectory(tests)
