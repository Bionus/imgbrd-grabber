# General
project(gui-qml)
add_definitions(-DGUI=1)
add_subdirectory(languages)

# Qt libraries
find_package(Qt5Quick REQUIRED)
find_package(Qt5Network REQUIRED)
set(QT_LIBRARIES Qt5::Core Qt5::Quick Qt5::Network)

# Android specials
if(ANDROID)
	find_package(Qt5AndroidExtras REQUIRED)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::AndroidExtras)
endif()

# Input
file(GLOB_RECURSE SOURCES "src/*" "resources/*.qrc")
include_directories("src/" "../lib/src/" "..")

# StatusBar
set(STATUSBAR_ROOT "${CMAKE_CURRENT_LIST_DIR}/vendor/qtstatusbar/src")
set(STATUSBAR_SOURCES "${STATUSBAR_ROOT}/statusbar.h" "${STATUSBAR_ROOT}/statusbar.cpp")
include_directories(${STATUSBAR_ROOT})
if (ANDROID)
	set(STATUSBAR_SOURCES ${STATUSBAR_SOURCES} "${STATUSBAR_ROOT}/statusbar_android.cpp")
else()
	set(STATUSBAR_SOURCES ${STATUSBAR_SOURCES} "${STATUSBAR_ROOT}/statusbar_dummy.cpp")
endif()

# Exclude platform-specific files from compilation
include(ListFilterRegex)
if(NOT ANDROID)
	listFilterRegex(SOURCES "android/")
endif()
if(NOT IOS)
	listFilterRegex(SOURCES "ios/")
endif()

if(ANDROID)
	add_library(${PROJECT_NAME} SHARED ${SOURCES} ${STATUSBAR_SOURCES} ${UTILS_SOURCES})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${STATUSBAR_SOURCES} ${UTILS_SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${LIBS} lib)

if(ANDROID)
	# SSL
	if(USE_SSL)
		message(STATUS "Compiling with SSL support")
		include("${CMAKE_CURRENT_LIST_DIR}/../cmake/android_openssl/CMakeLists.txt")
		message(STATUS "Android libs: ${ANDROID_EXTRA_LIBS}")
	endif()

	# Generate static package directory
	set(ANDROID_PACKAGE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/android_package")
	file(REMOVE_RECURSE "${ANDROID_PACKAGE_SOURCES}")
	file(MAKE_DIRECTORY "${ANDROID_PACKAGE_SOURCES}")
	file(COPY "${CMAKE_CURRENT_LIST_DIR}/../dist/android/" DESTINATION "${ANDROID_PACKAGE_SOURCES}")
	file(COPY "${CMAKE_CURRENT_LIST_DIR}/../dist/common/" DESTINATION "${ANDROID_PACKAGE_SOURCES}/assets")

	# Add target for compiled assets
	file(RELATIVE_PATH ANDROID_PACKAGE_SOURCES_REL ${CMAKE_CURRENT_LIST_DIR} ${ANDROID_PACKAGE_SOURCES})
	add_custom_target(android_copy_assets_qml
		DEPENDS sites gui-qml_translations
		COMMAND rsync -ar --exclude-from="../sites/exclude.txt" "../sites" "${ANDROID_PACKAGE_SOURCES_REL}/assets" WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/languages/" "${ANDROID_PACKAGE_SOURCES}/assets/languages"
	)

	# Add target
	include("qt-android-cmake/AddQtAndroidApk")
	add_qt_android_apk("${PROJECT_NAME}_apk" ${PROJECT_NAME}
		NAME ${ANDROID_APP_NAME}
		VERSION_CODE ${ANDROID_APP_VERSION_CODE}
		PACKAGE_NAME ${ANDROID_APP_PACKAGE_NAME}
		PACKAGE_SOURCES ${ANDROID_PACKAGE_SOURCES}
		KEYSTORE "${CMAKE_CURRENT_LIST_DIR}/../../release.jks" release
		KEYSTORE_PASSWORD ${KEYSTORE_PASSWORD}
		DEPENDS ${ANDROID_EXTRA_LIBS})
	add_dependencies("${PROJECT_NAME}_apk" android_copy_assets_qml)

	# Override build.gradle
	configure_file("${CMAKE_CURRENT_LIST_DIR}/../dist/android/build.gradle.in" "${QT_ANDROID_APP_BINARY_DIR}/build.gradle" @ONLY)
endif()
