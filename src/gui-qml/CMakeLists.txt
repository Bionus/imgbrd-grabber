# General
project(gui-qml)
add_definitions(-DGUI=1)

# Qt libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Network REQUIRED)
set(QT_LIBRARIES Qt5::Core Qt5::Quick Qt5::Network)

# Input
file(GLOB_RECURSE SOURCES "src/*")
include_directories("src/" "../lib/src/" "..")

add_executable(${PROJECT_NAME} ${SOURCES} ${UTILS_SOURCES})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${LIBS} lib)
