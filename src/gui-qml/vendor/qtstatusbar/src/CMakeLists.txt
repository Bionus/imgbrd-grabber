cmake_minimum_required(VERSION 3.1)
project(StatusBar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED
    Core
    Gui
    )
    
# prevent error "You must build your code with position independent code if Qt was built with..
if (Qt6_POSITION_INDEPENDENT_CODE)
  SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(CMAKE_AUTOMOC ON)

set(STATUSBAR_COMMON_SOURCES 
statusbar.h
statusbar.cpp
)

if (ANDROID)
set(STATUSBAR_PLATFORM_SOURCES statusbar_android.cpp)
elseif (CMAKE_SYSTEM_NAME STREQUAL iOS)
set(STATUSBAR_PLATFORM_SOURCES statusbar_ios.mm)
set(STATUSBAR_PLATFORM_LIBS "-framework UIKit")
else()
set(STATUSBAR_PLATFORM_SOURCES statusbar_dummy.cpp)
endif()
find_package(Qt6 COMPONENTS Core REQUIRED)

add_library(${PROJECT_NAME} OBJECT
    ${STATUSBAR_COMMON_SOURCES}
    ${STATUSBAR_PLATFORM_SOURCES}
    )
target_link_libraries(StatusBar PRIVATE Qt6::Core Qt6::Gui ${STATUSBAR_PLATFORM_LIBS})

target_include_directories(StatusBar PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    $<TARGET_PROPERTY:Qt6::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt6::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    )
