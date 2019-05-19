# General
project(gui)
add_definitions(-DGUI=1)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(USE_BREAKPAD 1)
	set(USE_QSCINTILLA 1)
endif()

# Disable Google Breakpad and QScintilla on Linux builds
if(DEFINED ENV{TRAVIS})
	set(USE_BREAKPAD 0)
	set(USE_QSCINTILLA 0)
endif()

# Disable Google Breakpad if the provided folder does not exist
if((DEFINED BREAKPAD) AND (NOT EXISTS "${BREAKPAD}"))
	message(WARNING "Provided Google Breakpad directory does not exist, disabling Google Breakpad: ${BREAKPAD}")
	set(USE_BREAKPAD 0)
endif()

# Nightly version settings
if((DEFINED NIGHTLY) AND (NIGHTLY MATCHES "1"))
	add_definitions(-DNIGHTLY=1)
	add_definitions(-DNIGHTLY_COMMIT="${COMMIT}")
	message(STATUS "Configuring nightly with commit '${COMMIT}'")
endif()

# Qt libraries
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
set(QT_LIBRARIES Qt5::Concurrent Qt5::Core Qt5::Gui Qt5::Multimedia Qt5::Network Qt5::Sql Qt5::Widgets Qt5::Xml)

# Windows specials
if(WIN32)
	find_package(Qt5WinExtras REQUIRED)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::WinExtras)
endif()

# Android specials
if(ANDROID)
	find_package(Qt5AndroidExtras REQUIRED)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::AndroidExtras)
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

# Input
file(GLOB SOURCES
	"src/main/main.cpp"
	"src/*.cpp"
	"src/batch/*.cpp"
	"src/settings/*.cpp"
	"src/sources/*.cpp"
	"src/tabs/*.cpp"
	"src/threads/*.cpp"
	"src/ui/*.cpp"
	"src/updater/*.cpp"
	"src/utils/rename-existing/*.cpp"
	"src/utils/blacklist-fix/*.cpp"
	"src/utils/empty-dirs-fix/*.cpp"
	"src/utils/md5-fix/*.cpp"
	"src/utils/tag-loader/*.cpp"
	"src/viewer/*.cpp"
	"resources/*.rc"
	"resources/*.qrc")
file(GLOB_RECURSE FORMS "src/*.ui")
include_directories("src/" "../lib/src/" "..")

# Breakpad files
if(USE_BREAKPAD)
	file(GLOB BREAKPAD_SOURCES "src/crashhandler/*.cpp")
	set(SOURCES ${SOURCES} ${BREAKPAD_SOURCES})
	add_definitions(-DUSE_BREAKPAD=1)
	if(WIN32)
		set(CMAKE_LFLAGS_RELEASE "${CMAKE_LFLAGS_RELEASE} /INCREMENTAL:NO /DEBUG")
		set(CMAKE_CFLAGS_RELEASE "${CMAKE_CFLAGS_RELEASE} -O2 -MD -zi")
		if(NOT DEFINED BREAKPAD)
			set(BREAKPAD "D:/bin/google-breakpad")
		endif()
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
		if(NOT DEFINED BREAKPAD)
			set(BREAKPAD "~/Programmation/google-breakpad")
		endif()
		list(APPEND LIBS "${BREAKPAD}/src/client/linux/libbreakpad_client.a")
	endif()
	message(STATUS "Using Google Breakpad from ${BREAKPAD}")
	include_directories(${BREAKPAD}/src)
endif()

if(ANDROID)
	add_library(${PROJECT_NAME} SHARED ${SOURCES} ${FORMS} ${UTILS_SOURCES})
elseif(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${FORMS} ${UTILS_SOURCES})
else()
	add_executable(${PROJECT_NAME} ${SOURCES} ${FORMS} ${UTILS_SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${LIBS} lib)

if(ANDROID)
	# SSL
	if(USE_SSL)
		message(STATUS "Compiling with SSL support")
		set(OPENSSL_ANDROID "D:/Programmation/C++/Qt/OpenSSH-android/openssl-1.0.2p")
		message(STATUS "OpenSSL for Android dir: ${OPENSSL_ANDROID}")
		list(APPEND APK_LIBS "${OPENSSL_ANDROID}/libcrypto.so" "${OPENSSL_ANDROID}/libssl.so")
	endif()

	# Generate manifest file
	set(ANDROID_PACKAGE_SOURCES "${CMAKE_CURRENT_LIST_DIR}/android")
	configure_file("${ANDROID_PACKAGE_SOURCES}/AndroidManifest.xml.in" "${ANDROID_PACKAGE_SOURCES}/AndroidManifest.xml" @ONLY)

	include("qt-android-cmake/AddQtAndroidApk")
	add_qt_android_apk("${PROJECT_NAME}_apk" ${PROJECT_NAME} BUILDTOOLS_REVISION "28.0.3" DEPENDS ${APK_LIBS} PACKAGE_SOURCES ${ANDROID_PACKAGE_SOURCES})
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Grabber")
	install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
endif()

# Pre-compiled header
if(USE_PCH)
	cotire(${PROJECT_NAME})
endif()
