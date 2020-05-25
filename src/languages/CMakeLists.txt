set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) # Don't delete TS files

find_package(Qt5LinguistTools)

# Get two separate lists of TS files: one with only 'YourLanguage', and another with all other translations
set(TS_FILES_FOR_TS "${CMAKE_CURRENT_SOURCE_DIR}/YourLanguage.ts")
file(GLOB TS_FILES_FOR_QM "*.ts")
include(ListFilterRegex)
listFilterRegex(TS_FILES_FOR_QM "YourLanguage.ts$")

# Source files (used for 'location' and 'context' in TS files)
file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_LIST_DIR}/../lib/src/*.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/../gui/src/*.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/../gui/src/*.ui"
	"${CMAKE_CURRENT_LIST_DIR}/../cli/src/*.cpp")

qt5_create_translation(QM_FILES_FOR_TS ${SOURCES} ${TS_FILES_FOR_TS}) # Re-generate TS and generate QM for 'YourLanguage'
qt5_add_translation(QM_FILES ${TS_FILES_FOR_QM}) # Only generate QM for actual translations

add_custom_target(General_translations ALL DEPENDS ${QM_FILES_FOR_TS} ${QM_FILES})

if(APPLE AND UNIX)
	install(FILES ${QM_FILES} DESTINATION Grabber.app/Contents/Resources/languages)
else()
	install(FILES ${QM_FILES} DESTINATION share/Grabber/languages)
endif()
