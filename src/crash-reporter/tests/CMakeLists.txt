project(crash-reporter-tests)

# General
enable_testing()
add_definitions(-DTEST=1)

# Sources
include(ListFilterRegex)
listFilterRegex(SOURCES "main.cpp")
file(GLOB_RECURSE SOURCES_TESTS "./*.cpp" "../../tests/src/*.cpp")
include_directories(${PROJECT_NAME} PUBLIC "." "../../lib/src/" "../../tests/" "../../tests/src/")

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${FORMS} ${UTILS_SOURCES} ${SOURCES_TESTS})
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${LIBS} lib Catch2::Catch2)

# Pre-compiled header
if(USE_PCH)
	cotire(${PROJECT_NAME})
endif()
