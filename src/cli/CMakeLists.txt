project(cli VERSION ${VERSION})

add_definitions(-DCLI=1)

find_package(Qt6 COMPONENTS Core Gui Network REQUIRED)
set(QT_LIBRARIES Qt6::Core Qt6::Gui Qt6::Network)

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories("src/" "../lib/src/" "..")

# CLI library (to be included in the CLI executable)
add_library(${PROJECT_NAME}-lib STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME}-lib ${QT_LIBRARIES} ${LIBS})

# CLI executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} lib)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/cli/$<0:>" OUTPUT_NAME "Grabber-cli")
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Tests
add_subdirectory(tests)
