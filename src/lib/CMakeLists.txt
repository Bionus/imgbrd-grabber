project(lib VERSION ${VERSION})

find_package(Qt6 COMPONENTS Core Concurrent Gui Network NetworkAuth Qml Sql Xml REQUIRED OPTIONAL_COMPONENTS WebEngineCore) # Qt6::Gui required for some classes such as QPixmap
set(QT_LIBRARIES Qt6::Core Qt6::Concurrent Qt6::Gui Qt6::Network Qt6::NetworkAuth Qt6::Qml Qt6::Sql Qt6::Xml)

# Optional WebEngine component
if((DEFINED NIGHTLY) AND (NIGHTLY MATCHES "1") AND TARGET Qt6::WebEngineCore)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt6::WebEngineCore)
	set(USE_WEBENGINE 1)
	add_definitions(-DUSE_WEBENGINE=1)
	message(STATUS "Compiling library with WebEngine support")
else()
	message(STATUS "Compiling library without WebEngine support")
endif()

# Android specials
if(ANDROID)
    find_package(Qt6 COMPONENTS Core CorePrivate REQUIRED)
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
