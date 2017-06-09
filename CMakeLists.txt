cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(ignoreMe "${QT_QMAKE_EXECUTABLE}") # Ignore "Manually-specified variables were not used by the project" warnings

# Use C++ 11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Grabber)

add_definitions(-DVERSION="\\"5.3.3\\"")
add_definitions(-DPROJECT_WEBSITE_URL="\\"https://bionus.github.io/imgbrd-grabber/\\"")
add_definitions(-DPROJECT_GITHUB_URL="\\"https://github.com/Bionus/imgbrd-grabber\\"")
add_definitions(-DPREFIX="\\"$ENV{PREFIX}\\"")

add_subdirectory(lib)
add_subdirectory(gui)
add_subdirectory(cli)
add_subdirectory(tests)
add_subdirectory(CrashReporter)
