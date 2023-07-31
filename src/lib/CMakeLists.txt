project(lib VERSION ${VERSION})

find_package(Qt6 COMPONENTS Core Concurrent Gui Network NetworkAuth Qml Sql Xml REQUIRED) # Qt6::Gui required for some classes such as QPixmap
set(QT_LIBRARIES Qt6::Core Qt6::Concurrent Qt6::Gui Qt6::Network Qt6::NetworkAuth Qt6::Qml Qt6::Sql Qt6::Xml)

# Android specials
if(ANDROID)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt6::CorePrivate)
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
