project(tests)

enable_testing()
add_definitions(-DTEST=1)
if(DEFINED HEADLESS)
	add_definitions(-DHEADLESS=1)
endif()

find_package(Qt5 COMPONENTS Gui Test Widgets REQUIRED)
set(QT_LIBRARIES Qt5::Core Qt5::Gui Qt5::Test Qt5::Widgets)

file(GLOB_RECURSE SOURCES "src/*.cpp")
include(ListFilterRegex)
listFilterRegex(SOURCES "vendor")
include_directories("src/" "src/common/" "../lib/src/" "../lib/vendor/lexbor/source/")

add_executable(${PROJECT_NAME} ${SOURCES})
add_test(${PROJECT_NAME} ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} lib)

add_subdirectory(languages)

# Pre-compiled header
if(USE_PCH)
	cotire(${PROJECT_NAME})
endif()
