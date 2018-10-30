project(tests)

enable_testing()
add_definitions(-DTEST=1)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Widgets REQUIRED)
set(QT_LIBRARIES Qt5::Core Qt5::Gui Qt5::Test Qt5::Widgets)

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories("src/" "../lib/src/")

add_executable(${PROJECT_NAME} ${SOURCES})
add_test(${PROJECT_NAME} ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} lib)

add_subdirectory(languages)

# Pre-compiled header
if(USE_PCH)
	include(PrecompiledHeader)
	add_precompiled_header(${PROJECT_NAME} "src/pch.h" FORCEINCLUDE SOURCE_CXX "${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp")
endif()
