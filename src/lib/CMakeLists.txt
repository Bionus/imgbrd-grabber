project(lib VERSION ${VERSION})

find_package(Qt5 COMPONENTS Concurrent Gui Network NetworkAuth Qml Sql Xml REQUIRED) # Qt5::Gui required for some classes such as QPixmap
set(QT_LIBRARIES Qt5::Concurrent Qt5::Core Qt5::Gui Qt5::Network Qt5::NetworkAuth Qt5::Qml Qt5::Sql Qt5::Xml)

# Android specials
if(ANDROID)
	find_package(Qt5 COMPONENTS AndroidExtras REQUIRED)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::AndroidExtras)
endif()

# Lexbor
add_subdirectory(vendor/lexbor)
set(LIBS ${LIBS} lexbor_static)
include_directories("vendor/lexbor/source/")

include(ListFilterRegex)
file(GLOB_RECURSE SOURCES "src/*.cpp" "vendor/*.cpp" "vendor/*.c")
listFilterRegex(SOURCES "vendor/lexbor/")
include_directories("src/" ".")

if(WIN_FILE_PROPS)
	set(LIBS ${LIBS} propsys)
else()
	listFilterRegex(SOURCES "windows-file-property.*")
	listFilterRegex(SOURCES "windows-url-protocol.*")
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${LIBS})

# Pre-compiled header
if(USE_PCH)
	cotire(${PROJECT_NAME})
endif()

# Tests
add_subdirectory(tests)
