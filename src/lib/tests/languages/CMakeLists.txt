set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) # Don't delete TS files

find_package(Qt6 COMPONENTS LinguistTools REQUIRED)

file(GLOB TS_FILES "*.ts")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*.cpp")

qt_add_lupdate(TS_FILES ${TS_FILES} SOURCES ${SOURCES})
qt_add_lrelease(TS_FILES ${TS_FILES} EXCLUDE_FROM_ALL)

add_custom_target(tests_translations)
add_dependencies(tests_translations ${PROJECT_NAME}_lupdate ${PROJECT_NAME}_lrelease)
