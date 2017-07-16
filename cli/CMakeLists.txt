project(cli)

add_definitions(-DCLI=1)

find_package(Qt5Core REQUIRED)
set(QT_LIBRARIES Qt5::Core)

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories("src/" "../lib/src/" "..")

add_executable("Grabber-cli" ${SOURCES})
target_link_libraries("Grabber-cli" ${QT_LIBRARIES} lib)
install(TARGETS "Grabber-cli" RUNTIME DESTINATION bin)