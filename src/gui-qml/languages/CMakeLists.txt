set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) # Don't delete TS files

find_package(Qt6 COMPONENTS LinguistTools REQUIRED)

# Get two separate lists of TS files: one with only 'YourLanguage', and another with all other translations
 file(GLOB TS_FILES_FOR_TS "*.ts")
file(GLOB TS_FILES_FOR_QM "*.ts")
include(ListFilterRegex)
listFilterRegex(TS_FILES_FOR_QM "YourLanguage.ts$")

# Source files (used for 'location' and 'context' in TS files)
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*")

qt_add_lupdate(TS_FILES ${TS_FILES_FOR_TS} SOURCES ${SOURCES})  # Re-generate TS and generate QM for 'YourLanguage'
qt_add_lrelease(TS_FILES ${TS_FILES_FOR_QM} EXCLUDE_FROM_ALL)  # Only generate QM for actual translations

add_custom_target("gui-qml_translations")
add_dependencies("gui-qml_translations" ${PROJECT_NAME}_lupdate ${PROJECT_NAME}_lrelease)
