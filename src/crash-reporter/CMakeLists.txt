project(CrashReporter)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
set(QT_LIBRARIES Qt6::Core Qt6::Gui Qt6::Widgets)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.ui" "resources/*.qrc")
include_directories("src/")

if (WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
else()
	add_executable(${PROJECT_NAME} ${SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/CrashReporter/$<0:>" OUTPUT_NAME "CrashReporter")

# Translations
add_subdirectory(languages)

# Tests
add_subdirectory(tests)
