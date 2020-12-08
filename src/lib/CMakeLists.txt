project(lib)

find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Gui REQUIRED) # For some classes such as QPixmap
find_package(Qt5Network REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Xml REQUIRED)
set(QT_LIBRARIES Qt5::Concurrent Qt5::Core Qt5::Gui Qt5::Network Qt5::Qml Qt5::Sql Qt5::Xml)

# Android specials
if(ANDROID)
	find_package(Qt5AndroidExtras REQUIRED)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::AndroidExtras)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
include_directories("src/")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${LIBS})

# Pre-compiled header
if(USE_PCH)
	cotire(${PROJECT_NAME})
endif()
