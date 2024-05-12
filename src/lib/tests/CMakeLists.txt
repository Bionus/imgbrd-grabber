project(lib-tests)

# General
enable_testing()
add_definitions(-DTEST=1)
if(DEFINED HEADLESS)
	add_definitions(-DHEADLESS=1)
endif()

find_package(Qt6 COMPONENTS Core Gui Test Widgets REQUIRED)
set(QT_LIBRARIES Qt6::Core Qt6::Gui Qt6::Test Qt6::Widgets)

# Sources
file(GLOB_RECURSE SOURCES "src/*.cpp" "../../tests/src/*.cpp")
include_directories("src/" "." "../src/" "../vendor/lexbor/source/" "../../tests/" "../../tests/src/")

# Executable
add_executable(${PROJECT_NAME} ${SOURCES})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} lib Catch2::Catch2)

add_subdirectory(languages)

# Pre-compiled header
if(USE_PCH)
	cotire(${PROJECT_NAME})
endif()
