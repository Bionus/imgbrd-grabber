project(cli)

add_definitions(-DCLI=1)

find_package(Qt5Network REQUIRED)
set(QT_LIBRARIES Qt5::Core Qt5::Network)

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories("src/" "../lib/src/" "..")

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} lib)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/cli/$<0:>" OUTPUT_NAME "Grabber-cli")
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)