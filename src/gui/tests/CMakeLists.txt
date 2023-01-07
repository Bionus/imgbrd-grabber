project(gui-tests)

# General
enable_testing()
add_definitions(-DTEST=1)
if(DEFINED HEADLESS)
	add_definitions(-DHEADLESS=1)
endif()

# Sources
include(ListFilterRegex)
listFilterRegex(SOURCES "main.cpp")
file(GLOB_RECURSE SOURCES_TESTS "./*.cpp" "../../tests/src/common/*.cpp")
include_directories(${PROJECT_NAME} PUBLIC "." "../../tests/" "../../tests/src/" "../../tests/src/common/")

# Executable
add_executable(${PROJECT_NAME} ${SOURCES} ${FORMS} ${UTILS_SOURCES} ${SOURCES_TESTS})
add_test(${PROJECT_NAME} ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${LIBS} lib)

# Pre-compiled header
if(USE_PCH)
	cotire(${PROJECT_NAME})
endif()
