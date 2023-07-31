set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1) # Don't delete TS files

find_package(Qt6 COMPONENTS LinguistTools REQUIRED)

file(GLOB TS_FILES "*.ts")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/*.cpp")

qt_create_translation(QM_FILES ${SOURCES} ${TS_FILES})
add_custom_target(tests_translations ALL DEPENDS ${QM_FILES})
