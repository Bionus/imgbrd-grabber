# General
project(gui)
add_definitions(-DGUI=1)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(USE_BREAKPAD 1)
	set(USE_QSCINTILLA 1)
endif()

# Travis settings
if(DEFINED ENV{TRAVIS} OR DEFINED ENV{APPVEYOR})
	set(USE_BREAKPAD 0)
	set(USE_QSCINTILLA 0)
endif()

# Qt libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Script REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
set(QT_LIBRARIES Qt5::Core Qt5::Gui Qt5::Multimedia Qt5::Network Qt5::Script Qt5::Sql Qt5::Widgets Qt5::Xml)

# Windows specials
if(WIN32)
	find_package(Qt5WinExtras REQUIRED)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::WinExtras)
endif()

# QScintilla
if(USE_QSCINTILLA)
	find_package(QScintilla REQUIRED)
	if(QSCINTILLA_FOUND)
		add_definitions(-DUSE_QSCINTILLA=1)
		list(APPEND LIBS ${QSCINTILLA_LIBRARY})
	else()
		MESSAGE(INFO "QScintilla2 not found")
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
	"src/viewer/*.cpp"
	"../resources/*.rc"
	"../resources/*.qrc")
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
		set(BREAKPAD "D:/bin/google-breakpad")
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
		set(BREAKPAD "~/Programmation/google-breakpad")
		list(APPEND LIBS "${BREAKPAD}/src/client/linux/libbreakpad_client.a")
	endif()
	include_directories(${BREAKPAD}/src)
endif()

if (WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${FORMS} ${UTILS_SOURCES})
else()
	add_executable(${PROJECT_NAME} ${SOURCES} ${FORMS} ${UTILS_SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${LIBS} lib)
