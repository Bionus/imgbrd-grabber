set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) # Don't delete TS files

find_package(Qt5LinguistTools)

file(GLOB TS_FILES "*.ts")
file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/../lib/src/*.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/../gui/src/*.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/../gui/src/*.ui"
	"${CMAKE_CURRENT_LIST_DIR}/../cli/src/*.cpp")
qt5_create_translation(QM_FILES ${SOURCES} ${TS_FILES})
add_custom_target(General_translations DEPENDS ${QM_FILES})

if(APPLE AND UNIX)
	install(FILES ${QM_FILES} DESTINATION Grabber.app/Contents/Resources/languages)
else()
	install(FILES ${QM_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/languages)
endif()
