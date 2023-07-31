# General
project(gui-qml VERSION ${VERSION})
add_definitions(-DGUI=1)
add_subdirectory(languages)

# Qt libraries
find_package(Qt6 COMPONENTS Core Core5Compat Quick Network REQUIRED)
set(QT_LIBRARIES Qt6::Core Qt6::Core5Compat Qt6::Quick Qt6::Network)

# Android specials
if(ANDROID)
	set(QT_LIBRARIES ${QT_LIBRARIES} Qt6::CorePrivate)
endif()

# Input
file(GLOB_RECURSE SOURCES "src/*" "resources/*.qrc")
include_directories("src/" "../lib/src/" "../lib/" "..")

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
if(NOT WIN32)
	listFilterRegex(SOURCES "windows/")
else()
	list(APPEND LIBS "WindowsApp.lib")
endif()

qt_add_executable(${PROJECT_NAME} ${SOURCES} ${STATUSBAR_SOURCES} ${UTILS_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIBRARIES} ${LIBS} lib)

if(ANDROID)
	# SSL
	if(USE_SSL)
		message(STATUS "Compiling with SSL support")
		include("${CMAKE_CURRENT_LIST_DIR}/../cmake/android_openssl/android_openssl.cmake")
		add_android_openssl_libraries(${PROJECT_NAME})
		get_target_property(ANDROID_EXTRA_LIBS ${PROJECT_NAME} QT_ANDROID_EXTRA_LIBS)
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
	add_dependencies(${PROJECT_NAME} android_copy_assets_qml)

	# Set Android properties
	set_target_properties(${PROJECT_NAME} PROPERTIES
		QT_ANDROID_PACKAGE_SOURCE_DIR ${ANDROID_PACKAGE_SOURCES}
		QT_ANDROID_VERSION_NAME ${ANDROID_APP_VERSION_NAME}
		QT_ANDROID_VERSION_CODE ${ANDROID_APP_VERSION_CODE}
	)
	if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set_target_properties(${PROJECT_NAME} PROPERTIES QT_ANDROID_DEPLOY_RELEASE ON)
	endif()
	# NAME ${ANDROID_APP_NAME}
	# PACKAGE_NAME ${ANDROID_APP_PACKAGE_NAME}

	# Sign package if the keystore was provided
	if(DEFINED ENV{QT_ANDROID_KEYSTORE_PATH})
		set(QT_ANDROID_SIGN_APK ON)
	else()
		message(WARNING "No keystore provided, APK signing disabled")
	endif()
endif()
